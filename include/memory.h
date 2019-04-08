struct FreeInfo {
	unsigned int addr, size;
};

struct MEMMAN {
	int frees;
	struct FreeInfo free[4090];
};

void mem_man_init(struct MEMMAN *mem);
unsigned int mem_alloc(struct MEMMAN *mem, unsigned int size);
int mem_free(struct MEMMAN *mem, unsigned int addr, unsigned int size);
int mem_total(struct MEMMAN *mem);
unsigned int mem_alloc_4k(struct MEMMAN *man, unsigned int size);
int mem_free_4k(struct MEMMAN *man, unsigned int addr, unsigned int size);