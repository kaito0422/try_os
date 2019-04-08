#include "../include/memory.h"

void mem_man_init(struct MEMMAN *mem)
{
	mem->frees = 0;
}


unsigned int mem_alloc(struct MEMMAN *mem, unsigned int size)
{
	int i, a;
	for(i = 0; i < mem->frees; i++)
	{
		if(mem->free[i].size >= size)	/* 找到一块内存大小满足要求 */
		{
			a = mem->free[i].addr;
			mem->free[i].addr += size;
			mem->free[i].size -= size;
			if(mem->free[i].size == 0)
			{
				mem->frees--;
				for( ; i < mem->frees; i++)
					mem->free[i] = mem->free[i + 1];
			}
			return a;	/* 返回可用空间的起始地址 */
		}
	}
	return 0;		/* 没有可用空间满足要求 */
}


int mem_free(struct MEMMAN *mem, unsigned int addr, unsigned int size)
{
	int i, j;
	for(i = 0; i < mem->frees; i++)
	{
		if(mem->free[i].addr > addr)
			break;
	}
	
	/* free[i - 1].addr < addr < free[i].addr */
	if(i > 0)		/* 如果能与前面合并的话，i必须大于0 */
	{
		if((mem->free[i - 1].addr + mem->free[i - 1].size) == addr)	/* 可与前面合并 */
		{
			mem->free[i - 1].size += size;
			if(addr + size == mem->free[i].addr)	/* 又能和后面合并 */
			{
				mem->free[i - 1].size += mem->free[i].size;
				mem->frees--;
				for( ; i < mem->frees; i++)
					mem->free[i] = mem->free[i + 1];	/* 后面的所有free都往前移 */
			}
			return 0;	/* 释放成功 */
		}
	}
	
	if(i < mem->frees)	/* 如果运行到这个if，则表示不能和前面合并 */	/* 若要和后面合并，则必须有后面 */
	{
		if((addr + size) == mem->free[i].addr)	/* 可以和后面合并 */
		{
			mem->free[i].addr = addr;
			mem->free[i].size += size;
			return 0;	/* 释放成功 */
		}
	}
	
	if(mem->frees < 4090)		/* 4090是最多可保存的可用空间的块数，要添加一块可用空间，则frees必须小于4090 */
	{
		for(j = mem->frees; j > i; j--)
			mem->free[j] = mem->free[j - 1];	/* 后面的所有free都往后移，因为中间要添加一个 */
		mem->frees++;
		mem->free[i].addr = addr;
		mem->free[i].size = size;
		return 0;
	}
	
	return -1;	/* 释放失败 */
}

int mem_total(struct MEMMAN *mem)
{
	int i, t = 0;
	for(i = 0; i < mem->frees; i++)
		t += mem->free[i].size;
	return t;
}

unsigned int mem_alloc_4k(struct MEMMAN *man, unsigned int size)
{
	unsigned int a;
	size = (size + 0xfff) & 0xfffff000;
	a = mem_alloc(man, size);
	return a;
}

int mem_free_4k(struct MEMMAN *man, unsigned int addr, unsigned int size)
{
	int i;
	size = (size + 0xfff) & 0xfffff000;
	i = mem_free(man, addr, size);
	return i;
}





















