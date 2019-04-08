struct TSS32 {
	unsigned int pre_link, esp0, ss0, esp1, ss1, esp2, ss2, cr3;
	unsigned int eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
	unsigned int es, cs, ss, ds, fs, gs;
	unsigned int ldtr, iomap;
};

struct TASK {
	int selector, flags;
	struct TSS32 tss;
	struct TASK *next;
};

#define TASK_MAX	1000
#define TASK_DGT	11

struct TASK_CONTROLLER {
	int num;	/* 注册的任务数 */
	int running;	/* 当前正在运行的任务数量 */
	int now_running;
	struct TASK *now;	/* 当前正在运行的是哪一个任务 */
	struct TASK *task;	/* 任务链表中的第一个任务 */
	struct TASK tasks0[TASK_MAX];
};


extern struct MEMMAN *memctl;
extern struct SHEET_CONTROLLER *shtctl;
extern struct TIMER_CONTROLLER *timerctl;
extern struct TASK_CONTROLLER *taskctl;
extern struct TIMER *task_timer;

/* 初始化任务控制器 */
void task_ctl_init(void);

/* 注册一个TASK + 默认初始化 */
struct TASK *task_alloc(void);

/* 初始化一个任务的函数 */
void task_init(struct TASK *task, int task_eip);

/* 设置一个任务为运行状态，并加入到运行任务队列 */
void task_run(struct TASK *task);

/* 任务切换 */
void task_switch(void);

/* 让一个任务休眠 */
void task_sleep(struct TASK *task);
