;描述符类型
DA_32	equ	0x4000	;32位段

DA_DPL0	equ	0x00	;DPL = 0
DA_DPL1	equ	0x20	;DPL = 1
DA_DPL2	equ	0x40	;DPL = 2
DA_DPL3	equ	0x60	;DPL = 3

;存储段描述符类型
DA_DR		equ	0x90	;存在的只读数据段类型值
DA_DRW		equ	0x92	;存在的可写可读数据段属性值
DA_DRWA		equ	0x93	;存在的已访问可读写数据段类型值
DA_C		equ	0x98	;存在的只执行代码段属性值
DA_CR		equ	0x9A	;存在的可执行可读代码段属性值
DA_CCO		equ	0x9C	;存在的只执行一致代码段属性值
DA_CCCOR	equ	0x9E	;存在的可执行可读一致代码段属性值

;系统段描述符类型
DA_LDT		equ	0x82	;局部描述符表段类型值
DA_TaskGate	equ	0x85	;任务门类型值
DA_386TSS	equ	0x89	;可用386任务状态段类型值
DA_386CGate	equ	0x8C	;386调用门类型值
DA_386IGate	equ	0x8E	;386中断门类型值
DA_386Tgate	equ	0x8F	;386陷阱门类型值

;选择子类型值
SA_RPL0		equ 0	;RPL
SA_RPL1		equ 1	;RPL
SA_RPL2		equ 2	;RPL
SA_RPL3		equ 3	;RPL

SA_TIG		equ 0	;TI
SA_TIL		equ 4	;TI

;宏
;描述符结构
;其中第一个参数（%1）是该段的基地址Base，第二个参数是段界限（长度），第三个参数是段属性
%macro Descriptor 3
	dw	%2 & 0FFFFh				;段界限1
	dw	%1 & 0FFFFh				;段基址1
	db	(%1 >> 16) & 0FFh			;段基址2
	dw	((%2 >> 8) & 0F00h) | (%3 & 0F0FFh)	;属性1 + 段界限2 + 属性2
	db	(%1 >> 24) & 0FFh			;段基址3
%endmacro	;共8个字节

;门结构
;第一个参数是门选择子，第二个参数是偏移，第三个参数是DCount，第四个参数是Attr
%macro Gate 4
	dw	(%2 & 0FFFFh)				;偏移1
	dw	%1					;选择子
	dw	(%3 & 1Fh) | ((%4 << 8) & 0FF00h)	;属性
	dw	((%2 >> 16) & 0FFFFh)			;偏移2
%endmacro 	;共8字节


DA_LIMIT_4K	equ	0x8000	;段界限粒度为4K字节

PG_P		EQU	1	;页存在属性位
PG_RWR		EQU	0	;R/W属性位值，读/执行
PG_RWW		EQU	2	;R/W属性位值，读/写/执行
PG_USS		EQU	0	;U/S属性位值，系统级
PG_USU		EQU	4	;U/S属性位值，用户级