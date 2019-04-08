#include "../include/sheet.h"
#include "../include/window.h"
#include "../include/font.h"

void new_win(struct SHEET *sht, int x_size, int y_size, unsigned char *title, int act)
{
	unsigned char *buf = sht->buf;
	int i, j;
	char back_color;	/* 用来显示区别当前窗口和不处在状态的窗口 */
	if(act == 0)
		back_color = 7;
	else
		back_color = 8;
	
	for(i = 0; i < x_size; i++)
		for(j = 0; j < 16; j++)
			*(buf + i + j*x_size) = 12;
	for(i = 0; i < x_size; i++)
		for(j = 16; j < y_size + 16; j++)
			*(buf + i + j*x_size) = back_color;
	for(i = x_size - 16; i < x_size; i++)
		for(j = 0; j < 16; j++)
		{
			if((((i - (x_size - 16)) == j) || (j == (x_size - i - 1))) && (j > 0 && j < 15))
				*(buf + i + j*x_size) = 15;
			else
				*(buf + i + j*x_size) = 4;
		}
	
	i = 0;
	while(1)
	{
		if(title[i] != 0)
		{
			win_load_character(sht, 0 + i, -1, 15, 12, title[i]);
			i++;
		}
		else
			break;
	}
}

void reload_win(struct SHEET *sheet, int act)
{
	int i, j;
	unsigned char *buf = sheet->buf;
	
	if(act == 0)	/* 主色 */
	{
		for(i = 0; i < sheet->x_size; i++)
			for(j = 16; j < sheet->y_size; j++)
				if(*(buf + i + j*sheet->x_size) == 8)
					*(buf + i + j*sheet->x_size) = 7;
	}
	else
	{
		for(i = 0; i < sheet->x_size; i++)
			for(j = 16; j < sheet->y_size; j++)
				if(*(buf + i + j*sheet->x_size) == 7)
					*(buf + i + j*sheet->x_size) = 8;
	}
}

void win_load_character(struct SHEET *sht, int x, int y, char color, char back_color, unsigned char k)
{
	int i;
	unsigned char *p;
	y++;
	p = sht->buf + x*8 + y*16*sht->x_size;
	for(i = 0; i < 16; i++)
	{
			*(p + i*sht->x_size + 0) = back_color;
			*(p + i*sht->x_size + 1) = back_color;
			*(p + i*sht->x_size + 2) = back_color;
			*(p + i*sht->x_size + 3) = back_color;
			*(p + i*sht->x_size + 4) = back_color;
			*(p + i*sht->x_size + 5) = back_color;
			*(p + i*sht->x_size + 6) = back_color;
			*(p + i*sht->x_size + 7) = back_color;
	}	
	for(i = 0; i < 16; i++)
	{
		if((font[(int)k][i] & 0x80) != 0)
			*(p + i*sht->x_size + 0) = color;
		if((font[(int)k][i] & 0x40) != 0)
			*(p + i*sht->x_size + 1) = color;
		if((font[(int)k][i] & 0x20) != 0)
			*(p + i*sht->x_size + 2) = color;
		if((font[(int)k][i] & 0x10) != 0)
			*(p + i*sht->x_size + 3) = color;
		if((font[(int)k][i] & 0x08) != 0)
			*(p + i*sht->x_size + 4) = color;
		if((font[(int)k][i] & 0x04) != 0)
			*(p + i*sht->x_size + 5) = color;
		if((font[(int)k][i] & 0x02) != 0)
			*(p + i*sht->x_size + 6) = color;
		if((font[(int)k][i] & 0x01) != 0)
			*(p + i*sht->x_size + 7) = color;
	}	
}

void win_load_str(struct SHEET *sht, int x, int y, char color, char back_color, unsigned char *str)
{
	int i = 0;
	while(1)
	{
		if(str[i] != 0)
		{
			win_load_character(sht, x + i, y, color, back_color, str[i]);
			i++;
		}
		else 
			break;
	}
}

void win_load_int(struct SHEET *sht, int x, int y, char color, char back_color, int k)
{
	int i;
	char tmp[10];
	tmp[0] = k / 1000000000 + '0';
	k = k - (tmp[0] - '0')*1000000000;
	tmp[1] = k / 100000000 + '0';
	k = k - (tmp[1] - '0')*100000000;
	tmp[2] = k / 10000000 + '0';
	k = k - (tmp[2] - '0')*10000000;
	tmp[3] = k / 1000000 + '0';
	k = k - (tmp[3] - '0')*1000000;
	tmp[4] = k / 100000 + '0';
	k = k - (tmp[4] - '0')*100000;
	tmp[5] = k / 10000 + '0';
	k = k - (tmp[5] - '0')*10000;
	tmp[6] = k / 1000 + '0';
	k = k - (tmp[6] - '0')*1000;
	tmp[7] = k / 100 + '0';
	k = k - (tmp[7] - '0')*100;
	tmp[8] = k / 10 + '0';
	k = k - (tmp[8] - '0')*10;
	tmp[9] = k % 10 + '0';
	for(i = 0; i < 10; i++)
		win_load_character(sht, x + i, y, color, back_color, tmp[i]);
}