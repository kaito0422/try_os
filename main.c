#include "./include/display.h"
#include "./include/kernel.h"
#include "./include/types.h"
#include "./include/font.h"
#include "./include/memory.h"
#include "./include/sheet.h"
#include "./include/window.h"
#include "./include/timer.h"
#include "./include/multitask.h"

#define PORT_KEYDAT		0x0060
#define MEMMAN_ADDR		0x00900000

void vector32_handler_c(void);
void vector33_handler_c(void);
void vector44_handler_c(void);
void enable_keyboard(void);
void enable_mouse(void);

void task_b_main(void);
void task_c_main(void);
void task_more_0(void);
void task_more_1(void);
void task_more_2(void);

int mouse_in_sheet(struct SHEET *sheet);

struct fifo_key {
	unsigned char buf[128];
	int w, r, size, num;
}mykey;

struct fifo_mouse {
	unsigned char buf[1024];
	int w, r, size, num;
}mymouse;

struct mouse_desc {
	char mouse_data1, mouse_data2, mouse_data3;
	int x, y, btn;
}mouse;

struct MEMMAN *memctl = (struct MEMMAN *)MEMMAN_ADDR;
struct SHEET_CONTROLLER *shtctl;
struct TIMER_CONTROLLER *timerctl;
struct TASK_CONTROLLER *taskctl;

struct TIMER *task_timer;
struct FIFO8 task_timer_fifo;
char task_timer_buf[8];
unsigned char *background_buf;
struct SHEET *sheet_background;

int Scrn_vram = 0xa0000;
int Scrn_x_size = 320;
int Scrn_y_size = 200;

int m_x, m_y;

void kmain(void)
{
	int mem_size = 0, mem_free_size = 0;
	
	struct TASK *task_main;

	enable_keyboard();	
	enable_mouse();
	
	io_cli();
	
	/* 内存测试 */
	mem_size = mem_test(0x400000, 0xafffffff) >> 20;
	mem_man_init(memctl);
	mem_free(memctl, 0x00600000, 0x000fffff);
	mem_free(memctl, 0x00780000, 0x00ffffff);
	mem_free_size = mem_total(memctl) >> 10 >> 10;
	
	/* 鼠标和键盘缓冲区相关设置 */
	mykey.w = mykey.r = mykey.num = 0;
	mykey.size = 128;
	mymouse.w = mymouse.r = mymouse.num = 0;
	mymouse.size = 1024;
	m_x = (Scrn_x_size / 2) - 4;
	m_y = (Scrn_y_size / 2) - 6;
	
	/* 分配图层数据信息缓存区 */
	background_buf = (unsigned char *)mem_alloc_4k(memctl, 320*200);
	/* 填写图层显示数据 */
	load_back(background_buf);
	load_str(background_buf, 0, 10, 7, "memory     MB free     MB");
	load_int(background_buf, 7, 10, mem_size);
	load_int(background_buf, 19, 10, mem_free_size);
	
	/* 初始化图层控制器 */
	sheet_controller_init((unsigned char *)Scrn_vram, Scrn_x_size, Scrn_y_size);
	sheet_background = sheet_alloc();
	sheet_init(sheet_background, background_buf, Scrn_x_size, Scrn_y_size, 99);
	sheet_location(sheet_background, 0, 0);
	sheet_height(sheet_background, 0);
	refresh_map(0, 0, Scrn_x_size, Scrn_y_size, 0);
	sheet_refresh(0, 0, Scrn_x_size, Scrn_y_size, 0, 0);
	
	fifo8_init(&task_timer_fifo, 8, task_timer_buf);
	/* 分配定时器控制器的地址空间 */
	timerctl = (struct TIMER_CONTROLLER *)mem_alloc_4k(memctl, sizeof(struct TIMER_CONTROLLER));
	/* 初始化定时器控制器 */
	timer_controller_init(timerctl);
	task_timer = (struct TIMER *)mem_alloc_4k(memctl, sizeof(struct TIMER));	/* 分配一个定时器用来任务切换 */
	task_timer = timer_alloc(timerctl);
	timer_init(task_timer, &task_timer_fifo, 1);
	timer_settime(timerctl, task_timer, 2);
	/* 使用定时器端口 */
	io_out8(PIC0_IMR, 0xf8);
	io_out8(PIC1_IMR, 0xef);
	
	/* 为任务控制器分配地址空间 */
	taskctl = (struct TASK_CONTROLLER *)mem_alloc_4k(memctl, sizeof(struct TASK_CONTROLLER *));
	/* 初始化任务控制器 */
	task_ctl_init();
	/* 从任务控制器里面分配出一个任务结构来使用 */
	task_main = task_alloc();
	/* 初始化任务函数 */
	task_init(task_main, (int)task_b_main);
	/* 运行一个任务 */
	task_run(task_main);
	
	io_sti();
	
	while(1);
}

void task_b_main(void)
{
	/* 定义鼠标图层 和 图层数据缓冲区*/
	struct SHEET *sheet_mouse, *sheet_win, *task_win[3];
	unsigned char *mouse_buf, *win_buf, *task_win_buf[3];
	char mouse_tmp, key_tmp;
	int mouse_phase = 0;
	int m_x_old, m_y_old, win_x_old, win_y_old;
	int win_tmp, win0_tmp, win1_tmp, win2_tmp, mouse_btn_tmp;
	int now_win;
	int i, tmp0, tmp1, tmp2, tmp3, tmp4;
	
	/* 尝试再添加一个任务 */
	struct TASK *task_c, *task_more[3];
	
	sheet_win = sheet_alloc();
	win_buf = (unsigned char *)mem_alloc_4k(memctl, 110*(60+16));
	sheet_init(sheet_win, win_buf, 110, 60+16, 99);
	new_win(sheet_win, 110, 60+16, "kaito", 0);
	sheet_location(sheet_win, 20, 20);
	sheet_height(sheet_win, 4);
	sheet_refresh(sheet_win->x_start, sheet_win->y_start, sheet_win->x_start + sheet_win->x_size, sheet_win->y_start + sheet_win->y_size, 1, 1);
	
	/* 分配鼠标图层空间和缓冲区空间 */
	sheet_mouse = sheet_alloc();
	mouse_buf = (unsigned char *)mem_alloc_4k(memctl, 8*12);
	load_mouse(mouse_buf);
	sheet_init(sheet_mouse, mouse_buf, 8, 12, 99);
	sheet_location(sheet_mouse, (Scrn_x_size / 2) - 4, (Scrn_y_size / 2) - 6);
	sheet_height(sheet_mouse, 5);
	sheet_refresh(sheet_mouse->x_start, sheet_mouse->y_start, sheet_mouse->x_start + sheet_mouse->x_size, sheet_mouse->y_start + sheet_mouse->y_size, sheet_mouse->height, sheet_mouse->height);
	
	task_win[0] = sheet_alloc();
	task_win[1] = sheet_alloc();
	task_win[2] = sheet_alloc();
	task_win_buf[0] = (unsigned char *)mem_alloc_4k(memctl, 110*(60+16));
	task_win_buf[1] = (unsigned char *)mem_alloc_4k(memctl, 110*(60+16));
	task_win_buf[2] = (unsigned char *)mem_alloc_4k(memctl, 110*(60+16));
	/* 把鼠标画到鼠标缓冲区里 */
	/* 设置鼠标图层信息 */
	sheet_init(task_win[0], task_win_buf[0], 110, 60+16, 99);
	sheet_init(task_win[1], task_win_buf[1], 110, 60+16, 99);
	sheet_init(task_win[2], task_win_buf[2], 110, 60+16, 99);
	/* 绘制窗口 */
	new_win(task_win[0], 110, 60+16, "task0", 1);
	new_win(task_win[1], 110, 60+16, "task1", 1);
	new_win(task_win[2], 110, 60+16, "task2", 1);
	/* 设置鼠标起始位置 */
	sheet_location(task_win[0], 170, 20);
	sheet_location(task_win[1], 20, 116);
	sheet_location(task_win[2], 170, 116);
	/* 设置鼠标图层高度 */
	sheet_height(task_win[0], 1);
	sheet_height(task_win[1], 2);
	sheet_height(task_win[2], 3);
	/* 刷新图层 */
	sheet_refresh(0, 0, 320, 200, 0, 5);
	
	put_character(0, 14, 0, 'T');
	
	task_c = task_alloc();
	task_init(task_c, (int)task_c_main);
//	task_run(task_c);
	
	task_more[0] = task_alloc();
	task_more[1] = task_alloc();
	task_more[2] = task_alloc();
	task_init(task_more[0], (int)task_more_0);
	task_init(task_more[1], (int)task_more_1);
	task_init(task_more[2], (int)task_more_2);
//	task_run(task_more[0]);
//	task_run(task_more[1]);
//	task_run(task_more[2]);

	now_win = 4;	/* 当前为kaito窗口 */
	mouse_btn_tmp = 0;	/* 用来记录鼠标之前有没有按下 */
	
	while(1)
	{
		io_cli();
		if(mymouse.num != 0)	/* 鼠标数据处理 */
		{
			mouse_tmp = mymouse.buf[mymouse.r++];
			if(mymouse.r == 1024)
				mymouse.r = 0;
			mymouse.num--;
			
			if(mouse_phase == 0)
			{
				if(mouse_tmp == 0x08)
				{
					mouse_phase = 1;
				}
			}
			else if(mouse_phase == 1)
			{
				mouse.mouse_data1 = mouse_tmp;
				mouse_phase = 2;
			}
			else if(mouse_phase == 2)
			{
				mouse.mouse_data2 = mouse_tmp;
				mouse_phase = 3;
			}
			else if(mouse_phase == 3)
			{
				mouse.mouse_data3 = mouse_tmp;
				mouse_phase = 1;
				
				mouse.x = mouse.mouse_data1;
				mouse.y = mouse.mouse_data2;
				mouse.y = - mouse.y;
				mouse.btn = mouse.mouse_data3 & 0x7;
				
				load_character(background_buf, 2, 0, 4, 'l');
				load_character(background_buf, 3, 0, 5, 'r');
				load_character(background_buf, 4, 0, 6, 'c');
				
				if((mouse.btn & 0x01) != 0)
				{
					load_character(background_buf, 2, 0, 4, 'L');
				}
				if((mouse.btn & 0x02) != 0)
				{
					load_character(background_buf, 3, 0, 5, 'R');
				}
				if((mouse.btn & 0x04) != 0)
				{
					load_character(background_buf, 4, 0, 6, 'C');
				}
				
				sheet_refresh(2*8, 0, 5*8, 1*16, 0, 0);
				
				m_x_old = m_x;
				m_y_old = m_y;
				m_x += mouse.x;
				m_y += mouse.y;
				if(m_x < 0)
					m_x = 0;
				if(m_y < 0)
					m_y = 0;
				if(m_x > 320 - 1)
					m_x = 320 - 1;
				if(m_y > 200 - 1)
					m_y = 200 - 1;
				
				
				sheet_location(sheet_mouse, m_x, m_y);
				if(((mouse.btn & 0x01) != 0))
				{					
					for(i = 4; i > 0; i--)
					{
						win_tmp = mouse_in_sheet(shtctl->sheets[i]);
						if((win_tmp != 0) && ((now_win == i) || (mouse_btn_tmp == 0)))
						{
							if((shtctl->sheets[i]->height != 4))
							{
								sheet_height(shtctl->sheets[i], 4);
								refresh_map(0, 0, Scrn_x_size, Scrn_y_size, 0);
							
								reload_win(shtctl->sheets[4], 0);
								win_refresh(shtctl->sheets[4]);
								reload_win(shtctl->sheets[3], 1);
								win_refresh(shtctl->sheets[3]);
								reload_win(shtctl->sheets[2], 1);
								win_refresh(shtctl->sheets[2]);
								reload_win(shtctl->sheets[1], 1);
								win_refresh(shtctl->sheets[1]);
							}
							if((win_tmp == 1) && (now_win == i))
							{
								win_x_old = shtctl->sheets[i]->x_start;
								win_y_old = shtctl->sheets[i]->y_start;
								sheet_location(shtctl->sheets[i], m_x - shtctl->sheets[i]->x_size / 2, m_y - 8);
								sheet_refresh(win_x_old, win_y_old, win_x_old + shtctl->sheets[i]->x_size, win_y_old + shtctl->sheets[i]->y_size, 0, shtctl->sheets[i]->height);
								sheet_refresh(shtctl->sheets[i]->x_start, shtctl->sheets[i]->y_start, shtctl->sheets[i]->x_start + shtctl->sheets[i]->x_size, shtctl->sheets[i]->y_start + shtctl->sheets[i]->y_size, shtctl->sheets[i]->height, shtctl->sheets[i]->height);
							}
							if(mouse_btn_tmp == 0)
								now_win = i;
							mouse_btn_tmp = 1;
						}
					}
				}
				
				if((mouse.btn & 0x01) == 0)
					mouse_btn_tmp = 0;
				
				sheet_refresh(m_x_old, m_y_old, m_x_old + 8, m_y_old + 12, 0, sheet_mouse->height - 1);
				sheet_refresh(m_x, m_y, m_x + 8, m_y + 12, sheet_mouse->height, sheet_mouse->height);
			}
		}
		
		if(mykey.num != 0)
		{
			key_tmp = mykey.buf[mykey.r++];
			if(mykey.r == 128)
				mykey.r = 0;
			key_tmp = get_key(key_tmp);
			win_load_character(sheet_background, 0, 0, 0, 15, key_tmp);
			sheet_refresh(0*8, 1*16, 1*8, 2*16, 0, 0);
			
			if(key_tmp == 'P')	/* 这个if/else用来测试任务休眠和唤醒功能 */
			{
				if(task_c->flags == 2)
					task_sleep(task_c);
			}
			else if(key_tmp == 'N')
			{
				if(task_c->flags == 1)
					task_run(task_c);
			}
			
			mykey.num--;
		}
		
		io_sti();
	}
}

void task_c_main(void)
{
	put_character(1, 10, 0, 'C');
	while(1)
	{
		put_character(1, 10, 0, 'C');
	}
}

void task_more_0(void)
{
	while(1)
	{
		put_character(1, 14, 0, '0');
	}
}

void task_more_1(void)
{
	while(1)
	{
		put_character(1, 15, 0, '1');
	}
}

void task_more_2(void)
{
	while(1)
	{
		put_character(1, 16, 0, '2');
	}
}


int mouse_in_sheet(struct SHEET *sheet)
{
	int sht_x_0, sht_y_0, sht_x_1, sht_y_1;
	sht_x_0 = sheet->x_start;
	sht_y_0 = sheet->y_start;
	sht_x_1 = sheet->x_start + sheet->x_size;
	sht_y_1 = sheet->y_start + sheet->y_size;
	if((m_x > sht_x_0) && (m_x < sht_x_1) && (m_y > sht_y_0) && (m_y < sht_y_0 + 16))
		return 1;	/* 在图层的标题栏处 */
	else if((m_x > sht_x_0) && (m_x < sht_x_1) && (m_y > sht_y_0 + 16) && (m_y < sht_y_1))
		return 2;	/* 在图层的出去标题栏的地方 */
	else
		return 0;	/* 不在图层里面 */
}


/*******************************************************************************************/
void vector32_handler_c(void)
{
	int i, j;
	struct TIMER *t;
	io_out8(PIC0_OCW2, 0x60);
	timerctl->count++;
	if(timerctl->next > timerctl->count)	/* 没有定时器超时 */
		return ;
	
	t = timerctl->t0;
	for(i = 0; i < timerctl->timer_top; i++)	/* 遍历所有使用的定时器 */	/* 这个for循环是对已经超时的定时器进行处理 */
	{
		if(t->timeout > timerctl->count)	/* 一旦有发现定时器没超时，则该定时器后面的都没超时，则不用处理 */
			break;
		
		/* 有定时器超时 */
		t->flags = TIMER_ALLOC;
		fifo8_put(t->fifo, t->data);
		t = t->next;
	}
	
	timerctl->timer_top -= i;	/* 前i个已经超时，定时时间到，从定时器列表中去掉前i个定时器 */
	
	timerctl->t0 = t;
	
	if(timerctl->timer_top > 0)
		timerctl->next = timerctl->t0->timeout;
	else
		timerctl->next = 0xffffffff;
	
	if(task_timer->fifo->free != task_timer->fifo->size)	/* task_timer超时 */
	{
		put_character(0, 12, 0, 'B');
	//	timer_settime(timerctl, task_timer, 2);
		task_switch();
	}
}

void vector33_handler_c(void)
{
	unsigned char data;
	if(mykey.num != 128)
	{
		data = io_in8(PORT_KEYDAT);
		if((data & 0x80) != 0)
			return ;
		mykey.buf[mykey.w++] = data;
		if(mykey.w == 128)
			mykey.w = 0;
		mykey.num++;
	}
}

void vector44_handler_c(void)
{
	unsigned char data;
	put_character(0, 0, 0, 'M');
	if(mymouse.num != 1024)
	{
		data = io_in8(PORT_KEYDAT);
		mymouse.buf[mymouse.w++] = data;
		if(mymouse.w == 1024)
			mymouse.w = 0;
		mymouse.num++;
	}
}







