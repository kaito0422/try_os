struct SHEET {
	unsigned char *buf;		/* 用来存储当前图层的显示信息 */
	int x_start, y_start, x_size, y_size, height, flags, transparent;
};

#define SHEET_MAX	256		/* 最多允许有256个图层 */

struct SHEET_CONTROLLER {
	unsigned char *vram, *map;	/* 用来保存现存的起始地址（一般为0xA0000） */
	int x_scrn, y_scrn;	/* 屏幕的x大小和y大小，和最顶层图层的图层号 */
	int top;	/* 一共有几个要显示的图层 */
	struct SHEET *now;
	struct SHEET *sheets[SHEET_MAX];	/* 图层显示列表 */
	struct SHEET sheets0[SHEET_MAX];	/* 用来分配图层 */
};

extern struct MEMMAN *memctl;
extern struct SHEET_CONTROLLER *shtctl;
extern struct TIMER_CONTROLLER *timerctl;
extern struct TASK_CONTROLLER *taskctl;

/* 初始化图层控制器 */
void sheet_controller_init(unsigned char *vram, int x_scrn, int y_scrn);

/* 从图层控制器里面分配一个图层来用 */
struct SHEET* sheet_alloc();

/* 设置图层信息 */
void sheet_init(struct SHEET *sheet, unsigned char *buf, int x_size, int y_size, int transparent);

/* 移动图层位置 */
void sheet_location(struct SHEET *sheet, int x_start, int y_start);

/* 设置图层高度 */
void sheet_height(struct SHEET *sheet, int height);

/* 释放一个使用完的图层 */
void sheet_free(struct SHEET* sheet);

/* 刷新一个区域 */
void sheet_refresh(int x0, int y0, int x1, int y1, int h0, int h1);

/* 刷新map */	/* 移动图层位置 或者调整图层高度时需要使用 */
void refresh_map(int x0, int y0, int x1, int y1, int height);

/* 刷新一个窗口对应的图图层 */
void win_refresh(struct SHEET *sheet);
