struct Segment_Descriptor {
	short limit_low, base_low;
	char base_mid, access_right;
	char limit_high, base_high;
};

struct Gate_Desciptor {
	short offset_low, selector;
	char resv_part, access_right;
	short offset_high;
};

struct Tss_Table {
	unsigned int PreTaskLink;
	unsigned int esp0;
	unsigned int SelectorRing0;
	unsigned int esp1;
	unsigned int SelectorRing1;
	unsigned int esp2;
	unsigned int SelectorRing2;
	unsigned int cr3;
	unsigned int eip;
	unsigned int eflags;
	unsigned int eax;
	unsigned int ecx;
	unsigned int edx;
	unsigned int ebx;
	unsigned int esp;
	unsigned int ebp;
	unsigned int esi;
	unsigned int edi;
	unsigned int es;
	unsigned int cs;
	unsigned int ss;
	unsigned int ds;
	unsigned int fs;
	unsigned int gs;
	unsigned int LDTSelector;
	unsigned short t;
	unsigned short IOMapBaseAddr;
};

#define		DA_32	0x4000		/* 32位段 */

/* 描述符优先级 */
#define		DA_DPL0		0x0000	
#define		DA_DPL1		0x0020
#define		DA_DPL2		0x0040
#define		DA_DPL3		0x0060

/* 存储段描述符类型 */
#define		DA_DR		0x0090		/* 存在的只读数据段类型值 */
#define		DA_DRW		0x0092		/* 存在的可写可读数据段属性值 */
#define		DA_DRWA		0x0093		/* 存在的已访问可读写数据段类型值 */
#define		DA_C		0x0098		/* 存在的只执行代码段属性值 */
#define		DA_CR		0x009A		/* 存在的可执行可读代码段属性值 */
#define		DA_CCO		0x009C		/* 存在的只执行一致代码段属性值 */
#define		DA_CCCOR	0x009E		/* 存在的可执行可读一致代码段属性值 */

/* 系统段描述符类型 */
#define		DA_LDT			0x0082		/* 局部描述符表段类型值 */
#define		DA_TASKGAT		0x0085		/* 任务门类型值 */
#define		DA_386TSS		0x0089		/* 可用386任务状态段类型值 */
#define		DA_386CGATE		0x008C		/* 386调用门类型值 */
#define		DA_386IGATE		0x008E		/* 386中断门类型值 */
#define		DA_386TGATE		0x008F		/* 386陷阱门类型值 */

/* 选择子类型 */
#define		SA_RPL0		0x0000
#define		SA_RPL1		0x0001
#define		SA_RPL2		0x0002
#define		SA_RPL3		0x0003

#define		SA_TIG		0x0000
#define		SA_TIL		0x0004

#define		DA_LIMIT_4K		0x8000




#define PIC0_ICW1		0x0020
#define PIC0_OCW2		0x0020
#define PIC0_IMR		0x0021
#define PIC0_ICW2		0x0021
#define PIC0_ICW3		0x0021
#define PIC0_ICW4		0x0021
#define PIC1_ICW1		0x00a0
#define PIC1_OCW2		0x00a0
#define PIC1_IMR		0x00a1
#define PIC1_ICW2		0x00a1
#define PIC1_ICW3		0x00a1
#define PIC1_ICW4		0x00a1

struct FIFO8 {
	unsigned char *buf;
	int p, q, size, free, flags;
};

void fifo8_init(struct FIFO8 *fifo, int size, unsigned char *buf);
int fifo8_put(struct FIFO8 *fifo, unsigned char data);
int fifo8_get(struct FIFO8 *fifo);

void set_segdesc(struct Segment_Descriptor *sd, unsigned int limit, int base, int arg);
void set_gatedesc(struct Gate_Desciptor *gd, int offset, int selector, int arg);