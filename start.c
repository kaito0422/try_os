#include "./include/types.h"

void stack_ring0(void);
void stack_ring1(void);
void stack_ring2(void);
void stack_ring3(void);

void init_gdt_idt(void);
void set_segdesc(struct Segment_Descriptor *sd, unsigned int limit, int base, int arg);
void set_gatedesc(struct Gate_Desciptor *gd, int offset, int selector, int arg);
void load_gdt(int addr, int limit);
void load_idt(int addr, int limit);
void init_pic(void);
void io_out8(int port, int data);
char io_in8(int port);
void io_cli(void);
void io_sti(void);
void write_vedio(void);
void interrupt_handler(void);
void vector13_handler(void);
void vector32_handler(void);
void vector33_handler(void);
void vector44_handler(void);
void vector_others(void);
void LABEL_TSS(void);

void enable_keyboard(void);
void enable_mouse(void);

void wait_kbc_sendready(void)
{
	while(1)
	{
		if((io_in8(0x64)&0x02) == 0)
			break;
	}
}

void kstart(void)
{
	int i = 1;
	unsigned short *p = (unsigned short *)0xB8000;
	
	*(p + 80 * 11 + 4) = (0x0A00) | ('A');
	*(p + 80 * 11 + 5) = (0x0B00) | ('l');
	*(p + 80 * 11 + 6) = (0x0C00) | ('l');
	*(p + 80 * 11 + 7) = (0x0D00) | ('e');
	*(p + 80 * 11 + 8) = (0x0E00) | ('n');
	
	io_cli();
	init_gdt_idt();
	init_pic();
	
	io_out8(0x0021, 0xf9);
	io_out8(0x00A1, 0xef);
	
	io_sti();
}

void init_gdt_idt(void)
{
	struct Segment_Descriptor *gdt = (struct Segment_Descriptor *)0x00000800;
	struct Gate_Desciptor     *idt = (struct Gate_Desciptor     *)0x00000000;
	int i;
	
	for(i = 0; i < 8192; i++)
		set_segdesc(gdt + i, 0, 0, 0);
	for(i = 0; i < 256; i++)
		set_gatedesc(idt + i, (int)(vector_others), 3*8, 0x8E);
	
	set_segdesc((struct Segment_Descriptor *)(0x00000800 + 1*8), 0xFFFFFFFF, 0x00000000, (DA_32 | DA_CR));	/* 范围是整块内存的代码段 */
	set_segdesc((struct Segment_Descriptor *)(0x00000800 + 2*8), 0xFFFFFFFF, 0x00000000, (DA_32 | DA_DRW));	/* 范围是整块内存的数据段 */
	set_segdesc((struct Segment_Descriptor *)(0x00000800 + 3*8), 0x7FF, (int)stack_ring0, (DA_32 | DA_DRW | DA_DPL0));	/* ring0的堆栈段 */
	set_segdesc((struct Segment_Descriptor *)(0x00000800 + 4*8), 0x7FF, (int)stack_ring1, (DA_32 | DA_DRW | DA_DPL1));	/* ring1的堆栈段 */
	set_segdesc((struct Segment_Descriptor *)(0x00000800 + 5*8), 0x7FF, (int)stack_ring2, (DA_32 | DA_DRW | DA_DPL2));	/* ring2的堆栈段 */
	set_segdesc((struct Segment_Descriptor *)(0x00000800 + 6*8), 0x7FF, (int)stack_ring3, (DA_32 | DA_DRW | DA_DPL3));	/* ring3的堆栈段 */
	set_segdesc((struct Segment_Descriptor *)(0x00000800 + 7*8), 0xFFFF, 0xB8000, (DA_DRW | DA_DPL3));	/* 显存的段 */
	set_segdesc((struct Segment_Descriptor *)(0x00000800 + 8*8), 0xFFFFF, 0x28000, (DA_32 | DA_CR));	/* handler的段 */
	set_segdesc((struct Segment_Descriptor *)(0x00000800 + 9*8), 0x68, (int)LABEL_TSS, DA_386TSS);	/* 用来存放TSS */
	
	
	set_gatedesc((struct Gate_Desciptor     *)(0x00000000 + 13*8), (int)(vector13_handler), 1*8, 0x8E);
	set_gatedesc((struct Gate_Desciptor     *)(0x00000000 + 32*8), (int)(vector32_handler), 1*8, 0x8E);
	set_gatedesc((struct Gate_Desciptor     *)(0x00000000 + 33*8), (int)(vector33_handler), 1*8, 0x8E);
	set_gatedesc((struct Gate_Desciptor     *)(0x00000000 + 44*8), (int)(vector44_handler), 1*8, 0x8E);
	
	load_gdt(8191*8, 0x00000800);
	load_idt(255*8, 0x00000000);
}

void set_segdesc(struct Segment_Descriptor *sd, unsigned int limit, int base, int arg)
{
	if(limit > 0xFFFFF)
	{
		arg |= 0x8000;
		limit /= 0x1000;
	}
	
	sd->limit_low = limit & 0xFFFF;
	sd->base_low = base & 0xFFFF;
	sd->base_mid = (base >> 16) & 0xFF;
	sd->access_right = arg & 0xff;
	sd->limit_high = ((limit >> 16) & 0x0F) | ((arg >> 8) & 0xF0); 
	sd->base_high = (base >> 24) & 0xFF;
}

void set_gatedesc(struct Gate_Desciptor *gd, int offset, int selector, int arg)
{
	gd->offset_low = offset & 0xFFFF;
	gd->selector = selector &0xFFFF;
	gd->resv_part = (arg >> 8) & 0xFF;
	gd->access_right = arg & 0xFF;
	gd->offset_high = (offset >> 16) & 0xFFFF;
}

void init_pic(void)
{	
	io_out8(0x0021, 0xff);
	io_out8(0x00A1, 0xff);
	
	io_out8(0x0020, 0x11);
	io_out8(0x0021, 0x20);
	io_out8(0x0021, 1 << 2);
	io_out8(0x0021, 0x01);
	
	io_out8(0x00A0, 0x11);
	io_out8(0x00A1, 0x28);
	io_out8(0x00A1, 2);
	io_out8(0x00A1, 0x01);
	
	io_out8(0x0021, 0xfd);
	io_out8(0x00A1, 0xff);
}

void enable_keyboard(void)
{
//	wait_kbc_sendready();
//	io_out8(0x64, 0x60);
//	wait_kbc_sendready();
//	io_out8(0x60, 0x47);
}

void enable_mouse(void)
{
//	wait_kbc_sendready();
//	io_out8(0x64, 0xA8);
	wait_kbc_sendready();
	io_out8(0x64, 0xD4);
	wait_kbc_sendready();
	io_out8(0x60, 0xF4);
}

void fifo8_init(struct FIFO8 *fifo, int size, unsigned char *buf)
{
	fifo->size = size;
	fifo->buf = buf;
	fifo->free = size;
	fifo->flags = 0;
	fifo->p = 0;
	fifo->q = 0;
}

int fifo8_put(struct FIFO8 *fifo, unsigned char data)
{
	if(fifo->free == 0)
	{
		fifo->flags |= 0x0001;
		return -1;
	}
	fifo->buf[fifo->p] = data;
	fifo->p++;
	if(fifo->p == fifo->size)
		fifo->p = 0;
	fifo->free--;
	return 0;
}

int fifo8_get(struct FIFO8 *fifo)
{
	int data;
	if(fifo->free == fifo->size)
		return -1;
	data = fifo->buf[fifo->q];
	fifo->q++;
	if(fifo->q == fifo->size)
		fifo->q = 0;
	fifo->free++;
	return data;
}






