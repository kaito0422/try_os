#include "../include/multitask.h"
#include "../include/types.h"
#include "../include/memory.h"
#include "../include/timer.h"
#include "../include/kernel.h"

void task_ctl_init(void)
{
	int i;
	taskctl->num = 0;	/* 当前注册了0个任务 */
	taskctl->running = 0;
	taskctl->now = 0;	/* 表示当前正在运行的任务 */
	taskctl->task = 0;	/* 任务指针指向空 */
	taskctl->now_running = 0;
	for(i = 0; i < TASK_MAX; i++)
		taskctl->tasks0[i].flags = 0;	/* 表示未使用 */
}

struct TASK *task_alloc(void)
{
	int i, esp_top;
	struct TASK *task, *tmp;
	for(i = 0; i < TASK_MAX; i++)
		if(taskctl->tasks0[i].flags == 0)	/* 找到一个任务变量未使用 */
		{
			task = &(taskctl->tasks0[i]);
			task->flags = 1;
			task->tss.cr3 = 0x100000;
			task->tss.eflags = 0x00000202;
			task->tss.eax = 0;
			task->tss.ecx = 0;
			task->tss.edx = 0;
			task->tss.ebx = 0;
			task->tss.ebp = 0;
			task->tss.esi = 0;
			task->tss.edi = 0;
			task->tss.es = 2*8;
			task->tss.cs = 1*8;
			task->tss.ss = 2*8;
			task->tss.ds = 2*8;
			task->tss.fs = 2*8;
			task->tss.gs = 2*8;
			task->tss.ldtr = 0;
			task->tss.iomap = 0x40000000;
			task->next = 0;
			taskctl->num++;
			
			set_segdesc((struct Segment_Descriptor *)(0x00000800 + (i + TASK_DGT)*8), 103, (int)(&(task->tss)), DA_386TSS);
			task->selector = (i + TASK_DGT)*8;
			esp_top = mem_alloc_4k(memctl, 2*1024) + 2*1024;
			task->tss.esp = esp_top;
			return task;
		}
	return 0;
}

void task_init(struct TASK *task, int task_eip)
{
	task->tss.eip = task_eip;
}

void task_run(struct TASK *task)
{
	struct TASK *tmp;
	task->flags = 2;	/* 表示该任务正在运行 */
	if(taskctl->now != 0)	/* 表示有任务在运行 */
	{
		tmp = taskctl->task;
		while(1)
		{
			if(tmp->next == 0)
			{
				tmp->next = task;
				task->next = 0;
				break;
			}
			else
				tmp = tmp->next;
		}
	}
	else	/* 当前没有任务在运行 */
	{
		taskctl->now = task;
		taskctl->task = task;
		task->next = 0;
	}
	taskctl->running++;
}

void task_switch(void)
{
	timer_settime(timerctl, task_timer, 2);
	if(taskctl->running == 0)
		return ;
	else if(taskctl->running == 1)
	{
		if(taskctl->now_running == 0)
		{
			taskctl->now_running = 1;
			timer_settime(timerctl, task_timer, 4);
			farjmp(0, taskctl->task->selector);
		}
		else
			return ;
	}
	else
	{
		if(taskctl->now->next != 0)
		{
			taskctl->now = taskctl->now->next;
			timer_settime(timerctl, task_timer, 1);
			farjmp(0, taskctl->now->selector);
		}
		else
		{
			taskctl->now = taskctl->task;
			timer_settime(timerctl, task_timer, 4);
			farjmp(0, taskctl->now->selector);
		}
	}
}

void task_sleep(struct TASK *task)
{
	struct TASK *tmp;
	task->flags = 1;	/* 任务存在，但是不运行（休眠） */
	
	if(task == taskctl->now)	/* 要休眠的是当前正在运行的任务 */
	{
		tmp = taskctl->task;
		while(1)
		{
			if(tmp->next == task)
			{
				tmp->next = task->next;
				taskctl->running--;
				farjmp(0, task->next->selector);
				break;	/* 只有在该任务在此唤醒的时候会运行到这个位置 */
			}
			else
				tmp = tmp->next;
		}
	}
	
	else	/* 要休眠的不是当前在运行的任务 */
	{
		if(taskctl->running >= 2)
		{
			if(task == taskctl->task)	/* 第一个任务 */
			{
				taskctl->task = task->next;
				taskctl->running--;
			}
			else if(task->next != 0)	/* 中间的任务 */
			{
				tmp = taskctl->task;
				while(1)
				{
					if(tmp->next == task)
					{
						tmp->next = task->next;
						taskctl->running--;
						break;
					}
					else
						tmp = tmp->next;
				}
			}
			else	/* 最后的任务 */
			{
				tmp = taskctl->task;
				while(1)
				{
					if(tmp->next == task)
					{
						tmp->next = 0;
						taskctl->running--;
						break;
					}
					else
						tmp = tmp->next;
				}
			}
		}
		
		else	/* 当前只有一个任务在运行 */	/* 正常情况下不允许出现这种情况，因为出现则最初始的任务都被终止了 */
		{
			taskctl->task = 0;
			taskctl->now = 0;
			taskctl->running--;
		}
	}
}


























