struct ScreenInfo {
	int x0, y0, x1, y1;
	int color;
};

struct FontInfo {
	int x, y, color;
	char *font;
};

void init_palette(void);
void set_palette(int start, int end, unsigned char *rgb);
void draw_rectangle(struct ScreenInfo *k);
void putfont(struct FontInfo *k);
void put_character(int row, int column, int color, char k);
void put_hex(int row, int column, char hex);
void put_int(int row, int column, int integer);
void clean_character(int row, int column);
void putstr(int row, int column, int color, char *p);
void init(void);
void clean_mouse(int x, int y);
void save_mouse(int x, int y);
void display_mouse(int x, int y);
void load_back(unsigned char *p);
void load_mouse(unsigned char *p);
void load_character(unsigned char *p, int x, int y, char color, unsigned char k);
void load_str(unsigned char *p, int x, int y, char color, unsigned char *str);
void load_int(unsigned char *p, int x, int y, int integer);
void init_display(void);
char get_key(unsigned char key);	
char io_in8(int port);
	
static char mouse_icon[12][8] = {
	"11......",
	"1011....",
	"10001...",
	"1000011.",
	"10001...",
	"10101...",
	"11.101..",
	"1..101..",
	"....101.",
	"....101.",
	".....11.",
	"........",
};

static int mouse_save_data[12][8] = {
	{15, 15, 15, 15, 15, 15, 15, 15},
	{15, 15, 15, 15, 15, 15, 15, 15},
	{15, 15, 15, 15, 15, 15, 15, 15},
	{15, 15, 15, 15, 15, 15, 15, 15},
	{15, 15, 15, 15, 15, 15, 15, 15},
	{15, 15, 15, 15, 15, 15, 15, 15},
	{15, 15, 15, 15, 15, 15, 15, 15},
	{15, 15, 15, 15, 15, 15, 15, 15},
	{15, 15, 15, 15, 15, 15, 15, 15},
	{15, 15, 15, 15, 15, 15, 15, 15},
	{15, 15, 15, 15, 15, 15, 15, 15},
	{15, 15, 15, 15, 15, 15, 15, 15},
};

	

	
	
	
	
	
	

