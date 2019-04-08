#define TIMER_MAX	500

#define TIMER_ALLOC		1
#define TIMER_USING		2

struct TIMER {
	struct TIMER *next;
	unsigned int timeout, flags;
	struct FIFO8 *fifo;
	unsigned char data;
};

struct TIMER_CONTROLLER {
	unsigned int count, next, timer_top;
	struct TIMER *t0;	/* 用来记录定时器列表中最前面的那个定时器 */
	struct TIMER timer[TIMER_MAX];
};

/* 初始化定时器控制器 */
void timer_controller_init(struct TIMER_CONTROLLER *timerctl);

/* 分配一个定时器 */
struct TIMER* timer_alloc(struct TIMER_CONTROLLER *timerctl);

/* 释放一个定时器 */
void timer_free(struct TIMER *timer);

/* 初始化一个定时器 */
void timer_init(struct TIMER *timer, struct FIFO8 *fifo, unsigned char data);

/* 设置定时器计时时间 */
void timer_settime(struct TIMER_CONTROLLER *timerctl, struct TIMER *timer, unsigned int timeout);