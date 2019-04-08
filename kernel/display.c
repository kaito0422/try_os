#include "../include/display.h"
#include "../include/font.h"

void io_out8(int port, int data);

void init_palette(void)
{
	static unsigned char table_rgb[16 * 3] = {
		0x00, 0x00, 0x00,	/* 0 黑 */
		0xff, 0x00, 0x00,	/* 1 亮红 */
		0x00, 0xff, 0x00,	/* 2 亮绿 */
		0xff, 0xff, 0x00,	/* 3 亮黄 */
		0x00, 0x00, 0xff,	/* 4 亮蓝 */
		0xff, 0x00, 0xff,	/* 5 亮紫 */
		0x00, 0xff, 0xff,	/* 6 咖啡 */
		0xff, 0xff, 0xff,	/* 7 亮灰 */
		0xc6, 0xc6, 0xc6,	/* 8 暗灰 */
		0x84, 0x00, 0x00,	/* 9 浅暗蓝 */
		0x00, 0x84, 0x00,	/* 10 暗绿 */
		0x84, 0x84, 0x00,	/* 11 浅亮蓝 */
		0x00, 0x00, 0x84,	/* 12 暗红 */
		0x84, 0x00, 0x84,	/* 13 暗紫 */
		0x00, 0x84, 0x84,	/* 14 暗黄 */
		0x84, 0x84, 0x84	/* 15 白 */
	};
	set_palette(0, 16, table_rgb);
}

void set_palette(int start, int end, unsigned char *rgb)
{
	int i;
	io_out8(0x03c8, start);
	for(i = start; i < end; i = i + 3)
	{
		io_out8(0x03c9, rgb[i]/4);
		io_out8(0x03c9, rgb[i + 1]/4);
		io_out8(0x03c9, rgb[i + 2]/4);
	}
}

void draw_rectangle(struct ScreenInfo *k)
{
	char *p;
	p = (char *)0xA0000;
	p = p + k->x0 + k->y0 * 320;	/* the start position of resctangle */
	
	int i, j;
	for(j = 0; j < k->y1 - k->y0; j++)
		for(i = 0; i < k->x1 - k->x0; i++)
		{
			*(p + i + j * 320) = k->color;
		}
}

void putfont(struct FontInfo *k)
{
	char *p;
	p = (char *)0xA0000;
	
	p = p + k->x + k->y * 320;
	int i, j;
	
	for(i = 0; i < 16; i++)
	{
		if((k->font[i] & 0x80) != 0)
			*(p + i * 320 + 0) = k->color;
		if((k->font[i] & 0x40) != 0)
			*(p + i * 320 + 1) = k->color;
		if((k->font[i] & 0x20) != 0)
			*(p + i * 320 + 2) = k->color;
		if((k->font[i] & 0x10) != 0)
			*(p + i * 320 + 3) = k->color;
		if((k->font[i] & 0x08) != 0)
			*(p + i * 320 + 4) = k->color;
		if((k->font[i] & 0x04) != 0)
			*(p + i * 320 + 5) = k->color;
		if((k->font[i] & 0x02) != 0)
			*(p + i * 320 + 6) = k->color;
		if((k->font[i] & 0x01) != 0)
			*(p + i * 320 + 7) = k->color;
	}
	
}

void put_character(int row, int column, int color, char k)
{
	int x, y;
	char *p;
	p = (char *)0xA0000;
	
	p = p + column * 8 + row * 16 *320;
	int i;
	
	for(i = 0; i < 16; i++)
	{
		if((font[(int)k][i] & 0x80) != 0)
			*(p + i*320 + 0) = color;
		if((font[(int)k][i] & 0x40) != 0)
			*(p + i*320 + 1) = color;
		if((font[(int)k][i] & 0x20) != 0)
			*(p + i*320 + 2) = color;
		if((font[(int)k][i] & 0x10) != 0)
			*(p + i*320 + 3) = color;
		if((font[(int)k][i] & 0x08) != 0)
			*(p + i*320 + 4) = color;
		if((font[(int)k][i] & 0x04) != 0)
			*(p + i*320 + 5) = color;
		if((font[(int)k][i] & 0x02) != 0)
			*(p + i*320 + 6) = color;
		if((font[(int)k][i] & 0x01) != 0)
			*(p + i*320 + 7) = color;
	}
}

void put_hex(int row, int column, char hex)
{
	char ch_h, ch_l;
	ch_h = (hex >> 4);
	ch_l = (hex & 0xF);
	
	if((ch_h >= 0) && (ch_h <= 9))
		ch_h += '0';
	else
		ch_h = (ch_h - 10) + 'A';
	put_character(row, column, 0, ch_h);
	if((ch_l >= 0) && (ch_l <= 9))
		ch_l += '0';
	else
		ch_l = (ch_l - 10) + 'A';
	put_character(row, column + 1, 0, ch_l);
}

void put_int(int row, int column, int integer)
{
	char tmp;
	tmp = (char)(integer / 1000);
	put_character(row, column, 8, tmp + '0');
	tmp = (char)((integer - (integer / 1000) * 1000) / 100);
	put_character(row, column + 1, 8, tmp + '0');
	tmp = (char)(integer - (integer / 1000) * 1000 - ((integer - (integer / 1000) * 1000) / 100) * 100) / 10;
	put_character(row, column + 2, 8, tmp + '0');
	tmp = (char)(integer % 10);
	put_character(row, column + 3, 8, tmp + '0');
}

void clean_character(int row, int column)
{
	int x, y;
	char *p;
	p = (char *)0xA0000;
	p = p + column * 8 + row * 16 *320;
	int i;
	for(i = 0; i < 16; i++)
	{
			*(p + i*320 + 0) = 15;
			*(p + i*320 + 1) = 15;
			*(p + i*320 + 2) = 15;
			*(p + i*320 + 3) = 15;
			*(p + i*320 + 4) = 15;
			*(p + i*320 + 5) = 15;
			*(p + i*320 + 6) = 15;
			*(p + i*320 + 7) = 15;
	}
}

void putstr(int row, int column, int color, char *p)
{
	int i, j;
	i = 0;
	while(1)
	{
		if(p[i] != 0)
		{
			put_character(row, column, color, p[i++]);
			column += 1;
			if(column == 40)
			{
				column = 0;
				row ++;
			}
		}
		else
			break;
	}
}

void clean_mouse(int x, int y)
{
	char *p;
	p = (char *)0xA0000;
	
	p = p + x + y * 320;
	int i, j;
	
	for(i = 0; i < 12; i++)
		for(j = 0; j < 8; j++)
		{
			 *(p + i * 320 + j) = mouse_save_data[i][j];
		}
}

void save_mouse(int x, int y)
{
	char *p;
	p = (char *)0xA0000;
	
	p = p + x + y * 320;
	int i, j;
	for(i = 0; i < 12; i++)
		for(j = 0; j < 8; j++)
		{
			mouse_save_data[i][j] = *(p + i * 320 + j);
		}
}

void display_mouse(int x, int y)
{
	char *p;
	p = (char *)0xA0000;
	
	p = p + x + y * 320;
	int i, j;
	
	for(i = 0; i < 12; i++)
		for(j = 0; j < 8; j++)
		{
			if(mouse_icon[i][j] == '0')
				*(p + i * 320 + j) = 15;
			else if(mouse_icon[i][j] == '1')
				*(p + i * 320 + j) = 0;
		}
}

void load_back(unsigned char *p)
{
	int i, j;
	for(i = 0; i < 320; i++)
		for(j = 0; j < 185; j++)
			*(p + j*320 + i) = 15;
	for(i = 0; i < 320; i++)
		for(j = 185; j < 200; j++)
			*(p + j*320 + i) = 12;
}

void load_int(unsigned char *p, int x, int y, int integer)
{
	char tmp;
	tmp = (char)(integer / 1000);
	load_character(p, x, y, 8, tmp + '0');
	tmp = (char)((integer - (integer / 1000) * 1000) / 100);
	load_character(p, x + 1, y, 8, tmp + '0');
	tmp = (char)(integer - (integer / 1000) * 1000 - ((integer - (integer / 1000) * 1000) / 100) * 100) / 10;
	load_character(p, x + 2, y, 8, tmp + '0');
	tmp = (char)(integer % 10);
	load_character(p, x + 3, y, 8, tmp + '0');
}

void load_character(unsigned char *p, int x, int y, char color, unsigned char k)
{
	int i, j;
	p = p + y*16*320 + x*8;
	for(i = 0; i < 16; i++)
	{
			*(p + i*320 + 0) = 15;
			*(p + i*320 + 1) = 15;
			*(p + i*320 + 2) = 15;
			*(p + i*320 + 3) = 15;
			*(p + i*320 + 4) = 15;
			*(p + i*320 + 5) = 15;
			*(p + i*320 + 6) = 15;
			*(p + i*320 + 7) = 15;
	}	
	for(i = 0; i < 16; i++)
	{
		if((font[(int)k][i] & 0x80) != 0)
			*(p + i*320 + 0) = color;
		if((font[(int)k][i] & 0x40) != 0)
			*(p + i*320 + 1) = color;
		if((font[(int)k][i] & 0x20) != 0)
			*(p + i*320 + 2) = color;
		if((font[(int)k][i] & 0x10) != 0)
			*(p + i*320 + 3) = color;
		if((font[(int)k][i] & 0x08) != 0)
			*(p + i*320 + 4) = color;
		if((font[(int)k][i] & 0x04) != 0)
			*(p + i*320 + 5) = color;
		if((font[(int)k][i] & 0x02) != 0)
			*(p + i*320 + 6) = color;
		if((font[(int)k][i] & 0x01) != 0)
			*(p + i*320 + 7) = color;
	}	
}

void load_str(unsigned char *p, int x, int y, char color, unsigned char *str)
{
	int i = 0, j;
	while(1)
	{
		if(str[i] != 0)
		{
			load_character(p, x, y, color, str[i]);
			i++;
			x++;
			if(x == 40)
			{
				x = 0;
				y++;
			}
		}
		else
			break;
	}
}

void load_mouse(unsigned char *p)
{
	int i, j;
	for(i = 0; i < 12; i++)
		for(j = 0; j < 8; j++)
		{
			if(mouse_icon[i][j] == '0')
				*(p + i * 8 + j) = 15;
			else if(mouse_icon[i][j] == '1')
				*(p + i * 8 + j) = 0;
			else
				*(p + i * 8 + j) = 99;
		}
}

void init_display(void)
{
	struct ScreenInfo *scrn_mod;
	struct ScreenInfo *inter;
	
	init_palette();
	
	scrn_mod->x0 = 0;
	scrn_mod->y0 = 0;
	scrn_mod->x1 = 320;
	scrn_mod->y1 = 200;
	scrn_mod->color = 15;
	draw_rectangle(scrn_mod);
	scrn_mod->x0 = 0;
	scrn_mod->y0 = 185;
	scrn_mod->x1 = 320;
	scrn_mod->y1 = 200;
	scrn_mod->color = 12;
	draw_rectangle(scrn_mod);
	scrn_mod->x0 = 1;
	scrn_mod->y0 = 186;
	scrn_mod->x1 = 17;
	scrn_mod->y1 = 199;
	scrn_mod->color = 8;
	draw_rectangle(scrn_mod);

	/* 40 * 12 */
	put_character(0, 0, 0, 'I');
	put_character(0, 1, 0, ' ');
	put_character(0, 2, 1, 'l');
	put_character(0, 3, 2, 'o');
	put_character(0, 4, 3, 'v');
	put_character(0, 5, 4, 'e');
	put_character(0, 6, 0, ' ');
	put_character(0, 7, 5, 'k');
	put_character(0, 8, 6, 'a');
	put_character(0, 9, 7, 'i');
	put_character(0, 10, 8, 't');
	put_character(0, 11, 9, 'o');
	put_character(0, 12, 10, '!');
	
	putstr(1, 0, 13, "This is a test program");
	putstr(7, 30, 10, "And you're a total asshole!");
	
	display_mouse(100, 100);
}


char get_key(unsigned char key)
{
	if(((key&0x7F)>=0x10) && ((key&0x7F)<=0x1B))
	{
		switch(key&0x7F)
		{
			case 0x10: return 'Q'; break;
			case 0x11: return 'W'; break;
			case 0x12: return 'E'; break;
			case 0x13: return 'R'; break;
			case 0x14: return 'T'; break;
			case 0x15: return 'Y'; break;
			case 0x16: return 'U'; break;
			case 0x17: return 'I'; break;
			case 0x18: return 'O'; break;
			case 0x19: return 'P'; break;
			case 0x1A: return '['; break;
			case 0x1B: return ']'; break;
			default : break;
		}
	}
	else if(((key&0x7F)>=0x1E) && ((key&0x7F)<=0x29))
	{
		switch(key&0x7F)
		{
			case 0x1E: return 'A'; break;
			case 0x1F: return 'S'; break;
			case 0x20: return 'D'; break;
			case 0x21: return 'F'; break;
			case 0x22: return 'G'; break;
			case 0x23: return 'H'; break;
			case 0x24: return 'J'; break;
			case 0x25: return 'K'; break;
			case 0x26: return 'L'; break;
			case 0x27: return ';'; break;
			case 0x28: return 0x22; break;
			case 0x29: return '*'; break;
			default : break;
		}
	}
	else if(((key&0x7F)>=0x2B) && ((key&0x7F)<=0x35))
	{
		switch(key&0x7F)
		{
			case 0x2B: return 0x5C; break;
			case 0x2C: return 'Z'; break;
			case 0x2D: return 'X'; break;
			case 0x2E: return 'C'; break;
			case 0x2F: return 'V'; break;
			case 0x30: return 'B'; break;
			case 0x31: return 'N'; break;
			case 0x32: return 'M'; break;
			case 0x33: return '<'; break;
			case 0x34: return '>'; break;
			case 0x35: return '?'; break;
			default : break;
		}
	}
	else if(((key&0x7F)>=0x02) && ((key&0x7F)<=0x0D))
	{
		switch(key&0x7F)
		{
			case 0x02: return '1'; break;
			case 0x03: return '2'; break;
			case 0x04: return '3'; break;
			case 0x05: return '4'; break;
			case 0x06: return '5'; break;
			case 0x07: return '6'; break;
			case 0x08: return '7'; break;
			case 0x09: return '8'; break;
			case 0x0A: return '9'; break;
			case 0x0B: return '0'; break;
			case 0x0C: return '-'; break;
			case 0x0D: return '+'; break;
			default : break;
		}
	}
	else if(((key&0x7F)>=0x02) && ((key&0x7F)<=0x0D))
	{
		switch(key&0x7F)
		{
			case 0x02: return '1'; break;
			case 0x03: return '2'; break;
			case 0x04: return '3'; break;
			case 0x05: return '4'; break;
			case 0x06: return '5'; break;
			case 0x07: return '6'; break;
			case 0x08: return '7'; break;
			case 0x09: return '8'; break;
			case 0x0A: return '9'; break;
			case 0x0B: return '0'; break;
			case 0x0C: return '-'; break;
			case 0x0D: return '+'; break;
			default : break;
		}
	}
	else if(((key&0x7F)>=0x35) && ((key&0x7F)<=0x53))
	{
		switch(key&0x7F)
		{
			case 0x35: return '/'; break;
			case 0x37: return '*'; break;
			case 0x4A: return '-'; break;
			case 0x47: return '7'; break;
			case 0x48: return '8'; break;
			case 0x49: return '9'; break;
			case 0x4B: return '4'; break;
			case 0x4C: return '5'; break;
			case 0x4D: return '6'; break;
			case 0x4E: return '+'; break;
			case 0x4F: return '1'; break;
			case 0x50: return '2'; break;
			case 0x51: return '3'; break;
			case 0x52: return '0'; break;
			case 0x53: return '.'; break;
			default : break;
		}
	}
	else if((key&0x7F) == 0x0E)
		return 8;
	else
		put_character(2, 0, 15, ' ');
}












