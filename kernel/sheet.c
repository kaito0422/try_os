#include "../include/memory.h"
#include "../include/sheet.h"

void sheet_controller_init(unsigned char *vram, int x_scrn, int y_scrn)
{
	int i;
	shtctl = (struct SHEET_CONTROLLER *)mem_alloc_4k(memctl, sizeof(struct SHEET_CONTROLLER *));
	shtctl->vram = vram;
	shtctl->x_scrn = x_scrn;	/* 设置屏幕宽度 */
	shtctl->y_scrn = y_scrn;	/* 设置屏幕高度 */
	shtctl->top = -1;	/* 最顶层的图层 */
	shtctl->now = 0;	/* 当前图层 */
	shtctl->map = (unsigned char *)mem_alloc_4k(memctl, x_scrn*y_scrn);
	for(i = 0; i < SHEET_MAX; i++)
		shtctl->sheets0[i].flags = 0;
}

struct SHEET* sheet_alloc()
{
	int i;
	struct SHEET *sheet;
	for(i = 0; i < SHEET_MAX; i++)
		if(shtctl->sheets0[i].flags == 0)
		{
			sheet = &(shtctl->sheets0[i]);
			sheet->flags = 1;	/* 表示这个图层为已经使用 */
			return sheet;
		}
	return 0;
}

void sheet_init(struct SHEET *sheet, unsigned char *buf, int x_size, int y_size, int transparent)
{
	sheet->buf = buf;
	sheet->x_size = x_size;
	sheet->y_size = y_size;
	sheet->height = -1;	/* -1表示这个图层不显示 */
	sheet->transparent = transparent;
}

void sheet_location(struct SHEET *sheet, int x_start, int y_start)
{
	int x_old, y_old;
	x_old = sheet->x_start;
	y_old = sheet->y_start;
	sheet->x_start = x_start;
	sheet->y_start = y_start;
	refresh_map(x_old, y_old, x_old + sheet->x_size, y_old + sheet->y_size, 0);
	refresh_map(x_start, y_start, x_start + sheet->x_size, y_start + sheet->y_size, sheet->height);
}

void sheet_height(struct SHEET *sheet, int height)
{
	int i, old = sheet->height;
	if(height > shtctl->top + 1)
		height = shtctl->top + 1;
	if(height < -1)
		height = -1;
	sheet->height = height;
	
	if(old > height)	/* 图层下降 */
	{
		if(height == -1)	/* 要减少一个图层的显示 */
		{
			for(i = old; i < shtctl->top; i++)
			{
				shtctl->sheets[i] = shtctl->sheets[i + 1];
				shtctl->sheets[i]->height = i;
			}
			shtctl->top--;
		}
		else	/* 显示的图层数不变 */
		{
			for(i = old; i > height; i--)
			{
				shtctl->sheets[i] = shtctl->sheets[i - 1];
				shtctl->sheets[i]->height = i;
			}
			shtctl->sheets[height] = sheet;
		}
	}
	else	/* 图层上升 */
	{
		if(old == -1)	/* 要添加一个要显示的图层 */
		{
			for(i = shtctl->top + 1; i > height; i--)
			{
				shtctl->sheets[i] = shtctl->sheets[i - 1];
				shtctl->sheets[i]->height = i;
			}
			shtctl->sheets[height] = sheet;
			shtctl->top++;
		}
		else	/* 要显示的图层数量不变 */
		{
			for(i = old; i < height; i++)
			{
				shtctl->sheets[i] = shtctl->sheets[i + 1];
				shtctl->sheets[i]->height = i;
			}
			shtctl->sheets[height] = sheet;
		}
	}
	refresh_map(sheet->x_start, sheet->y_start, sheet->x_start + sheet->x_size, sheet->y_start + sheet->y_size, 0);
}

void sheet_free(struct SHEET* sheet)
{
	if(sheet->height >= 0)
		sheet_height(sheet, -1);
	sheet->flags = 0;
}

/* 这里面的x0, y0, x1, y1是相对于320*200像素屏幕的绝对坐标 */
void sheet_refresh(int x0, int y0, int x1, int y1, int h0, int h1)
{
	int i, display_x_0, display_y_0, display_x_1, display_y_1;
	int i_x, i_y, x_v, y_v;
	char color;
	unsigned char *buf, *vram, *map;
	vram = shtctl->vram;
	map = shtctl->map;
	struct SHEET *sheet;
	
	/* 一下4个if用来调整不正确的刷新范围，使刷新在屏幕范围内 */
	if(x0 < 0)
		x0 = 0;
	if(y0 < 0)
		y0 = 0;
	if(x1 > shtctl->x_scrn)
		x1 = shtctl->x_scrn;
	if(y1 > shtctl->y_scrn)
		y1 = shtctl->y_scrn;
	
	for(i = h0; i <= h1; i++)	/* 把高度从h0到h1的图层遍历	*/
	{
		display_x_0 = x0;
		display_y_0 = y0;
		display_x_1 = x1;
		display_y_1 = y1;
		sheet = shtctl->sheets[i];
		buf = sheet->buf;
		
		/* 一下4个if用来使刷新在该图层范围内 */
		if(display_x_0 < sheet->x_start)
			display_x_0 = sheet->x_start;
		if(display_y_0 < sheet->y_start)
			display_y_0 = sheet->y_start;
		if(display_x_1 > sheet->x_start + sheet->x_size)
			display_x_1 = sheet->x_start + sheet->x_size;
		if(display_y_1 > sheet->y_start + sheet->y_size)
			display_y_1 = sheet->y_start + sheet->y_size;
		
		for(i_y = display_y_0; i_y < display_y_1; i_y++)
			for(i_x = display_x_0; i_x < display_x_1; i_x++)
			{
				x_v = i_x - sheet->x_start;
				y_v = i_y - sheet->y_start;
				color = *(buf + x_v + y_v*sheet->x_size);	/* 取来颜色 */
				if(*(map + i_x + i_y*shtctl->x_scrn) == sheet->height)
					*(vram + i_x + i_y*shtctl->x_scrn) = color;
			}
	}
}

void refresh_map(int x0, int y0, int x1, int y1, int height)
{
	int i, map_x_0, map_y_0, map_x_1, map_y_1;
	int i_x, i_y, x_v, y_v;
	struct SHEET *sheet;
	unsigned char *buf, *map;
	char color;
	
	map = shtctl->map;
	
	if(x0 < 0)
		x0 = 0;
	if(y0 < 0)
		y0 = 0;
	if(x1 > shtctl->x_scrn)
		x1 = shtctl->x_scrn;
	if(y1 > shtctl->y_scrn)
		y1 = shtctl->y_scrn;
	
	for(i = height; i <= shtctl->top; i++)
	{
		sheet = shtctl->sheets[i];
		buf = sheet->buf;
		
		map_x_0 = x0;
		map_y_0 = y0;
		map_x_1 = x1;
		map_y_1 = y1;
		
		if(map_x_0 < sheet->x_start)
			map_x_0 = sheet->x_start;
		if(map_y_0 < sheet->y_start)
			map_y_0 = sheet->y_start;
		if(map_x_1 > sheet->x_start + sheet->x_size)
			map_x_1 = sheet->x_start + sheet->x_size;
		if(map_y_1 > sheet->y_start + sheet->y_size)
			map_y_1 = sheet->y_start + sheet->y_size;
		
		for(i_x = map_x_0; i_x < map_x_1; i_x++)
			for(i_y = map_y_0; i_y < map_y_1; i_y++)
			{
				x_v = i_x - sheet->x_start;
				y_v = i_y - sheet->y_start;
				color = *(buf + x_v + y_v*sheet->x_size);
				if(color != sheet->transparent)
					*(map + i_x + i_y*shtctl->x_scrn) = sheet->height;
			}
	}
}

void win_refresh(struct SHEET *sheet)
{
	sheet_refresh(sheet->x_start, sheet->y_start, sheet->x_start + sheet->x_size, sheet->y_start + sheet->y_size, sheet->height, sheet->height);
}










