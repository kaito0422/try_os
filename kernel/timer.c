#include "../include/kernel.h"
#include "../include/types.h"
#include "../include/timer.h"

#define PIT_CTRL	0x0043
#define PIT_CNT0	0x0040

extern struct timer_ctl mytimer;

void timer_controller_init(struct TIMER_CONTROLLER *timerctl)
{
	int i;
	io_out8(PIT_CTRL, 0x34);
	io_out8(PIT_CNT0, 0x9c);
	io_out8(PIT_CNT0, 0x2e);
	timerctl->count = 0;	/* 定时器计数器初值为0 */
	timerctl->next = 0xffffffff;	/* 没有设定下一个要超时的时间 */
	timerctl->timer_top = 0;	/* 还没有计时器在计时 */
	for(i = 0; i < TIMER_MAX; i++)
		timerctl->timer[i].flags = 0;	/* 标记所有定时器都未使用 */
}

struct TIMER* timer_alloc(struct TIMER_CONTROLLER *timerctl)
{
	int i;
	for(i = 0; i < TIMER_MAX; i++)
	{
		if(timerctl->timer[i].flags == 0)
		{
			timerctl->timer[i].flags = TIMER_ALLOC;
			return &(timerctl->timer[i]);
		}
	}
	return 0;
}

void timer_free(struct TIMER *timer)
{
	timer->flags = 0;
}

void timer_init(struct TIMER *timer, struct FIFO8 *fifo, unsigned char data)
{
	timer->fifo = fifo;
	timer->data = data;
}

void timer_settime(struct TIMER_CONTROLLER *timerctl, struct TIMER *timer, unsigned int timeout)
{
	int i, j;
	struct TIMER *t, *p;
	timer->timeout = timeout + timerctl->count;
	timer->flags = TIMER_USING;
	timerctl->timer_top++;	/* 定时器列表个数加1 */
	if(timerctl->timer_top == 1)	/* 定时器列表中只有一个元素，即刚刚加入的那个定时器 */
	{
		timerctl->t0 = timer;
		timer->next = 0;		/* 没有下一个 */
		timerctl->next = timer->timeout;	/* 唯一一个超时时间，记录到下一个超时时刻中 */
		return ;
	}
	
	/* 如果定时器列表中不止一个元素 */
	if(timer->timeout <= timerctl->t0->timeout)
	{
		timer->next = timerctl->t0;
		timerctl->t0 = timer;
		timerctl->next = timer->timeout;
		return ;
	}
	
	/* 如果当前定时器要插入到定时器列表中间 */
	t = timerctl->t0;
	while(1)
	{
		if(t->next == 0)
			break;
		p = t;
		t = t->next;	/* 指向定时器链表的下一个元素 */
		if(timer->timeout <= t->timeout)
		{
			p->next = timer;
			timer->next = t;
			return;
		}
	}
	
	/* 插入到最后 */
	t->next = timer;
	timer->next = 0;
	return ;
}




























