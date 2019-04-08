;����������
DA_32	equ	0x4000	;32λ��

DA_DPL0	equ	0x00	;DPL = 0
DA_DPL1	equ	0x20	;DPL = 1
DA_DPL2	equ	0x40	;DPL = 2
DA_DPL3	equ	0x60	;DPL = 3

;�洢������������
DA_DR		equ	0x90	;���ڵ�ֻ�����ݶ�����ֵ
DA_DRW		equ	0x92	;���ڵĿ�д�ɶ����ݶ�����ֵ
DA_DRWA		equ	0x93	;���ڵ��ѷ��ʿɶ�д���ݶ�����ֵ
DA_C		equ	0x98	;���ڵ�ִֻ�д��������ֵ
DA_CR		equ	0x9A	;���ڵĿ�ִ�пɶ����������ֵ
DA_CCO		equ	0x9C	;���ڵ�ִֻ��һ�´��������ֵ
DA_CCCOR	equ	0x9E	;���ڵĿ�ִ�пɶ�һ�´��������ֵ

;ϵͳ������������
DA_LDT		equ	0x82	;�ֲ���������������ֵ
DA_TaskGate	equ	0x85	;����������ֵ
DA_386TSS	equ	0x89	;����386����״̬������ֵ
DA_386CGate	equ	0x8C	;386����������ֵ
DA_386IGate	equ	0x8E	;386�ж�������ֵ
DA_386Tgate	equ	0x8F	;386����������ֵ

;ѡ��������ֵ
SA_RPL0		equ 0	;RPL
SA_RPL1		equ 1	;RPL
SA_RPL2		equ 2	;RPL
SA_RPL3		equ 3	;RPL

SA_TIG		equ 0	;TI
SA_TIL		equ 4	;TI

;��
;�������ṹ
;���е�һ��������%1���ǸöεĻ���ַBase���ڶ��������Ƕν��ޣ����ȣ��������������Ƕ�����
%macro Descriptor 3
	dw	%2 & 0FFFFh				;�ν���1
	dw	%1 & 0FFFFh				;�λ�ַ1
	db	(%1 >> 16) & 0FFh			;�λ�ַ2
	dw	((%2 >> 8) & 0F00h) | (%3 & 0F0FFh)	;����1 + �ν���2 + ����2
	db	(%1 >> 24) & 0FFh			;�λ�ַ3
%endmacro	;��8���ֽ�

;�Žṹ
;��һ����������ѡ���ӣ��ڶ���������ƫ�ƣ�������������DCount�����ĸ�������Attr
%macro Gate 4
	dw	(%2 & 0FFFFh)				;ƫ��1
	dw	%1					;ѡ����
	dw	(%3 & 1Fh) | ((%4 << 8) & 0FF00h)	;����
	dw	((%2 >> 16) & 0FFFFh)			;ƫ��2
%endmacro 	;��8�ֽ�


DA_LIMIT_4K	equ	0x8000	;�ν�������Ϊ4K�ֽ�

PG_P		EQU	1	;ҳ��������λ
PG_RWR		EQU	0	;R/W����λֵ����/ִ��
PG_RWW		EQU	2	;R/W����λֵ����/д/ִ��
PG_USS		EQU	0	;U/S����λֵ��ϵͳ��
PG_USU		EQU	4	;U/S����λֵ���û���