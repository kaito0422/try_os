BS_OEMName		db	'kaito-OS'
BPB_BytesPerSec		dw	512		;ÿ������512���ֽ�
BPB_SecPerClus		db	1		;ÿ����ռһ������
BPB_RsvdSecCnt		dw	1		;boot����������ռ��1������
BPB_NumFATs		db	2		;������FAT��
BPB_RootEntCnt		dw	224		;��Ŀ¼���ɷ�224���ļ��Ķ�Ӧ��Ŀ
BPB_TotSec16		dw	2880		;һ����2880������
BPB_Media		db	0xF0		;ý��������
BPB_FATSz16		dw	9		;ÿ��FAT����9������
BPB_SecPerTrk		dw	18		;ÿ���ŵ���18������
BPB_NumHeads		dw	2		;��������ͷ�������棩
BPB_HiddSec		dd	0		;��������0��
BPB_TotSec32		dd	0		;wTotalSectorCountΪ0ʱ���ֵ��¼������
BS_DrvNum		db	0		;�ж�13����������
BS_Reserved1		db	0		;����
BS_BootSig		db	0x29		;��չ������ǣ�29h��
BS_VolID		dd	0		;�����к�
BS_VolLab		db	'kOS        '	;���꣬����11�ֽ�
BS_FileSysType		db	'FAT12   '	;�ļ�ϵͳ���ͣ�����8�ֽ�

BaseLoadAddr		equ	0x8000			;���ص��ڴ��ַ�Ļ���ַ
OffsetLoadAddr		equ	0x0			;���ص��ڴ��ַ��ƫ�Ƶ�ַ
RootDirSectors		equ	14	;��Ŀ¼ռ�õ�������14
DeltaSectorNo		equ	17	;�������Ĵغż������ֵ����14���Ƕ��ڵ�������

ReadSectorNum	dw	0		;Ҫ��ȡ��������
FileName	db	'SETUP   BIN'	;Ҫ��ȡ���ļ������ļ���ռ8���ֽڣ���չ��ռ3���ֽ�
RootDirSize	dw	RootDirSectors	;��Ŀ¼��ռ��������
OdEven		db	0		;��ż��0=ż���� 1=����
DisRow		db	0

BaseKernelAddr		equ	0x9000		;�����̼����ں˵��ڴ����λ��--����ַ
OffserKernelAddr	equ	0x0		;�����̼����ں˵��ڴ����λ��--ƫ�Ƶ�ַ

KernelName	db	'KERNEL  BIN'

;----------------------------------------------------------------------------------------
;ELF file info
BaseElfAddr		equ		0x9000		;��elf�ļ����ص����λ��
OffsetElfAddr	equ		0x0

ElfName		db	'ELF     BIN'
