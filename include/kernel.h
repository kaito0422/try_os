void io_out8(int port, int data);
char io_in8(int port);
void io_cli(void);
void io_sti(void);
int mem_test(unsigned int start, unsigned int end);
void load_tr(short tr);
void farjmp(int eip, int cs);