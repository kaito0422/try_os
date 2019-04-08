BS_OEMName		db	'kaito-OS'
BPB_BytesPerSec		dw	512		;每个扇区512个字节
BPB_SecPerClus		db	1		;每个簇占一个扇区
BPB_RsvdSecCnt		dw	1		;boot（引导程序）占用1个扇区
BPB_NumFATs		db	2		;有两个FAT区
BPB_RootEntCnt		dw	224		;根目录最多可放224个文件的对应条目
BPB_TotSec16		dw	2880		;一共有2880个扇区
BPB_Media		db	0xF0		;媒体描述符
BPB_FATSz16		dw	9		;每个FAT区有9个扇区
BPB_SecPerTrk		dw	18		;每个磁道有18个扇区
BPB_NumHeads		dw	2		;有两个磁头（有两面）
BPB_HiddSec		dd	0		;隐藏扇区0个
BPB_TotSec32		dd	0		;wTotalSectorCount为0时这个值记录扇区数
BS_DrvNum		db	0		;中断13的驱动器号
BS_Reserved1		db	0		;保留
BS_BootSig		db	0x29		;扩展引导标记（29h）
BS_VolID		dd	0		;卷序列号
BS_VolLab		db	'kOS        '	;卷标，必须11字节
BS_FileSysType		db	'FAT12   '	;文件系统类型，必须8字节

BaseLoadAddr		equ	0x8000			;加载到内存地址的基地址
OffsetLoadAddr		equ	0x0			;加载到内存地址的偏移地址
RootDirSectors		equ	14	;根目录占用的扇区数14
DeltaSectorNo		equ	17	;数据区的簇号加上这个值加上14就是对于的扇区号

ReadSectorNum	dw	0		;要读取的扇区号
FileName	db	'SETUP   BIN'	;要读取的文件名，文件名占8个字节，扩展名占3个字节
RootDirSize	dw	RootDirSectors	;根目录区占用扇区数
OdEven		db	0		;奇偶，0=偶数， 1=奇数
DisRow		db	0

BaseKernelAddr		equ	0x9000		;从软盘加载内核到内存这个位置--基地址
OffserKernelAddr	equ	0x0		;从软盘加载内核到内存这个位置--偏移地址

KernelName	db	'KERNEL  BIN'

;----------------------------------------------------------------------------------------
;ELF file info
BaseElfAddr		equ		0x9000		;把elf文件加载到这个位置
OffsetElfAddr	equ		0x0

ElfName		db	'ELF     BIN'

