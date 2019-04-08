%include "pm.inc"
%include "fat12head.inc"


SETUP_START:
	mov ax, cs
	mov ds, ax
	mov ss, ax
	mov es, ax
	mov sp, BaseLoadAddr
	mov ax, 0xB800
	mov gs, ax
	
	mov di, (80 * 5 + 1) * 2
	mov ah, 0x0C
	mov al, 'L'
	mov [gs:di], ax
	
	call READ_ELF
	
	jmp LABEL_SEG_REAL_MODE

;----------------------------------------------------------------------------------------	
READ_ELF:
	mov di, (80 * 5 + 2) * 2
	mov ah, 0x0C
	mov al, 'G'
	mov [gs:di], ax
	
	mov di, (80 * 6 + 1) * 2
	mov si, TestStr
	call DisplayStr

	;复位软驱，用BIOS调用int 13h
	mov ah, 0
	mov dl, 0
	int 0x13
	
	;读取一个根目录区的扇区用于查找setup.bin的文件名条目
	mov word [ReadSectorNum], 19
LABEL_READ_ROOT_DIR_FOR_FILENAME:
	cmp word [RootDirSize], 0	;根目录区一共有14个扇区，判断是否这14个扇区都找过了
	jz LABEL_NO_SETUP
	dec word [RootDirSize]
	
	;读取一个扇区到内存中，用来查找文件名SETUPBIN
	mov ax, BaseElfAddr
	mov es, ax
	mov bx, OffsetElfAddr
	mov cl, 1
	mov ax, [ReadSectorNum]
	
	call ReadSector		;读一个扇区到[es:bx]的内存位置
	
	;在刚读取的扇区中寻找文件名为SETUP.BIN的条目
	mov si, ElfName
	mov di, OffsetElfAddr
	cld
	mov dx, 16	;判断一个扇区中的16个条目，一个扇区512个字节，一个条目32个字节，一共16个条目
LABEL_FIND_FILENAME:
	cmp dx, 0
	jz LABEL_NEXT_SECTOR
	dec dx
	mov cx, 11	;文件名为11个字节
LABEL_CMP_FILENAME:
	cmp cx, 0
	jz LABEL_FILE_FOUND
	dec cx
	lodsb
	cmp al, byte [es:di]
	jz LABEL_GO_ON
	jmp LABEL_DIFFERENT
	
	
LABEL_DIFFERENT:
	and di, 0xFFE0
	add di, 0x20
	mov si, ElfName
	jmp LABEL_FIND_FILENAME
	
LABEL_GO_ON:
	inc di
	jmp LABEL_CMP_FILENAME
	
LABEL_NEXT_SECTOR:
	add word [ReadSectorNum], 1	;为下一次到扇区做准备
	jmp LABEL_READ_ROOT_DIR_FOR_FILENAME
	
LABEL_NO_SETUP:
	mov di, (80 * 7 + 1) * 2
	mov si, NoElfStr
	call DisplayStr
	
	ret	
	
LABEL_FILE_FOUND:
	push di
	mov di, (80 * 7 + 1) * 2
	mov si, ElfFoundStr
	call DisplayStr
	pop di

	and di, 0xFFE0
	add di, 0x1A
	
	mov ax, word [es:di]	;把数据区的簇号存在ax中
	push ax
	
	add ax, RootDirSectors
	add ax, DeltaSectorNo
	mov dx, BaseElfAddr
	mov es, dx
	mov bx, OffsetElfAddr
	
LABEL_GOON_LOADING_FILE:
	mov cl, 1
	call ReadSector
	pop ax
	call GetFATEntry
	cmp ax, 0xFFF
	jz LABEL_FILE_LOADED
	push ax
	add ax, RootDirSectors
	add ax, DeltaSectorNo
	add bx, [BPB_BytesPerSec]
	jmp LABEL_GOON_LOADING_FILE
	
	
LABEL_FILE_LOADED:
	mov di, (80 * 8 + 1) * 2
	mov si, ElfLoaded
	call DisplayStr
	
ret
	
TestStr			db	'for test!'
ElfFoundStr		db	'ELF found!'
NoElfStr		db	'There is no ELF!'
ElfLoaded		db	'ELF has been loaded!'
;----------------------------------------------------------------------------------------
ReadSector:
	mov dl, [BPB_SecPerTrk]
	div dl
	mov ch, al
	mov dh, al
	shr ch, 1	;柱面号ch准备好
	and dh, 1	;磁头号dh准备好
	mov al, cl	;要读的扇区数al准备好
	mov cl, ah
	add cl, 1	;起始扇区号cl准备好
	mov ah, 0x02	;ah准备好
	mov dl, 0
.GoOnReading:
	int 0x13
	jc .GoOnReading
	
	ret
	
;找到FAT入口
GetFATEntry:
	push es
	push bx
	push ax
	
	;在内存中留出1K空间，存放FAT
	mov ax, BaseElfAddr
	sub ax, 0x100
	mov es, ax
	pop ax
	
	mov byte [OdEven], 0
	
	;获取簇号对应在FAT中的项号，ax=字节数， dx表示奇偶
	mov bx, 3
	mul bx
	mov bx, 2
	div bx
	cmp dx, 0
	jz LABEL_EVEN
	mov byte [OdEven], 1
	
LABEL_EVEN:
	xor dx, dx
	mov bx, [BPB_BytesPerSec]
	div bx		;ax=FAT中的扇区数，dx=在扇区中的字节偏移
	push dx
	mov bx, 0
	add ax, 1
	mov cl, 2	;一次读取两个扇区，避免FAT项刚好卡在两个字节之间
	call ReadSector
	
	pop dx
	add bx, dx
	mov ax, [es:bx]
	cmp byte [OdEven], 1
	jnz LABEL_EVEN_2	;如果是偶数
	shr ax, 4
LABEL_EVEN_2:
	and ax, 0xFFF
	
	pop bx
	pop es
	ret
	
;显示字符串
DisplayStr:
	mov ax, 0xB800
	mov gs, ax
	mov ah, 0x0F
	cld
.load:
	lodsb
	mov [gs:di], ax
	add di, 2
	sub al, '!'
	jnz .load
	
	ret

;========================================================================================	
;========================================================================================	
;========================================================================================	
;========================================================================================	
;========================================================================================	
;========================================================================================	
	
	
	
;========================================================================================
%include "elf.inc"

GDT_ADDR	equ	LABEL_GDT + BaseLoadAddr * 0x10 + OffsetLoadAddr
PageDirBase		equ		0x100000
PageTblBase		equ		0x101000

[section .gdt]
LABEL_GDT:			Descriptor		0,				0,				0
LABEL_DESC_DATA:	Descriptor		0,              DATA_LEN,       DA_32 | DA_DRW
LABEL_DESC_STACK0:	Descriptor		0,              STACK0_TOP,     DA_32 | DA_DRW | DA_DPL0
LABEL_DESC_STACK1:	Descriptor		0,              STACK1_TOP,     DA_32 | DA_DRW | DA_DPL1
LABEL_DESC_STACK2:	Descriptor		0,              STACK2_TOP,     DA_32 | DA_DRW | DA_DPL2
LABEL_DESC_STACK3:	Descriptor		0,              STACK3_TOP,     DA_32 | DA_DRW | DA_DPL3
LABEL_DESC_PROTECT:	Descriptor		0,              PROTECT_LEN,    DA_32 | DA_CR
LABEL_DESC_VIDEO:	Descriptor		0xB8000,		0xFFFF,			DA_DRW | DA_DPL3
LABEL_DESC_PAGE:	Descriptor		0,              0xFFFFF,        DA_DRW | DA_LIMIT_4K
LABEL_DESC_PHY:		Descriptor		0,              0xFFFFFFFF,     DA_32 | DA_DRW | DA_LIMIT_4K
LABEL_DESC_ELF:		Descriptor		ELFBASE,        0xFFFFF,        DA_32 | DA_DRW
LABEL_DESC_EXE:		Descriptor      0,              0xFFFFFFFF,     DA_32 | DA_CR | DA_LIMIT_4K
LABEL_DESC_NS:		Descriptor		0xFFFFF800,		0x800,			DA_32 | DA_DRW

GDT_LEN		equ	$ - LABEL_GDT
GDT_PTR		dw	GDT_LEN - 1
			dd	GDT_ADDR

SelectorData	equ		LABEL_DESC_DATA - LABEL_GDT
SelectorStack0	equ		LABEL_DESC_STACK0 - LABEL_GDT + SA_RPL0
SelectorStack1	equ		LABEL_DESC_STACK1 - LABEL_GDT + SA_RPL1
SelectorStack2	equ		LABEL_DESC_STACK2 - LABEL_GDT + SA_RPL2
SelectorStack3	equ		LABEL_DESC_STACK3 - LABEL_GDT + SA_RPL3
SelectorProtect	equ		LABEL_DESC_PROTECT - LABEL_GDT
SelectorVideo	equ		LABEL_DESC_VIDEO - LABEL_GDT
SelectorPage	equ		LABEL_DESC_PAGE - LABEL_GDT
SelectorPhy		equ		LABEL_DESC_PHY - LABEL_GDT
SelectorElf		equ		LABEL_DESC_ELF - LABEL_GDT
SelectorExe		equ		LABEL_DESC_EXE - LABEL_GDT
SelectorNs		equ		LABEL_DESC_NS - LABEL_GDT

[section .data]
align 32
[bits 32]
SEG_DATA:
	Mystr	db	"I am kaito!", 0
	Mystroffset	equ	Mystr - $$
DATA_LEN	equ	$ - SEG_DATA - 1

[section .stack]
align 32
[bits 32]
SEG_STACK0:
	times 1024	db	0
STACK0_TOP	equ	$ - SEG_STACK0 - 1

[section .stack]
align 32
[bits 32]
SEG_STACK1:
	times 1024	db	0
STACK1_TOP	equ	$ - SEG_STACK1 - 1

[section .stack]
align 32
[bits 32]
SEG_STACK2:
	times 1024	db	0
STACK2_TOP	equ	$ - SEG_STACK2 - 1

[section .stack]
align 32
[bits 32]
SEG_STACK3:
	times 1024	db	0
STACK3_TOP	equ	$ - SEG_STACK3 - 1


[section .real_mode]
[bits 16]
LABEL_SEG_REAL_MODE:
	mov ax, cs
	mov ds, ax
	mov ss, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov sp, BaseLoadAddr
	
	xor eax, eax
	mov ax, ds
	shl eax, 4
	add eax, SEG_DATA
	mov word [LABEL_DESC_DATA + 2], ax
	shr eax, 16
	mov byte [LABEL_DESC_DATA + 4], al
	mov byte [LABEL_DESC_DATA + 7], ah
	
	xor eax, eax
	mov ax, ss
	shl eax, 4
	add eax, SEG_STACK0
	mov word [LABEL_DESC_STACK0 + 2], ax
	shr eax, 16
	mov byte [LABEL_DESC_STACK0 + 4], al
	mov byte [LABEL_DESC_STACK0 + 7], ah
	
	xor eax, eax
	mov ax, ss
	shl eax, 4
	add eax, SEG_STACK1
	mov word [LABEL_DESC_STACK1 + 2], ax
	shr eax, 16
	mov byte [LABEL_DESC_STACK1 + 4], al
	mov byte [LABEL_DESC_STACK1 + 7], ah
	
	xor eax, eax
	mov ax, ss
	shl eax, 4
	add eax, SEG_STACK2
	mov word [LABEL_DESC_STACK2 + 2], ax
	shr eax, 16
	mov byte [LABEL_DESC_STACK2 + 4], al
	mov byte [LABEL_DESC_STACK2 + 7], ah
	
	xor eax, eax
	mov ax, ss
	shl eax, 4
	add eax, SEG_STACK3
	mov word [LABEL_DESC_STACK3 + 2], ax
	shr eax, 16
	mov byte [LABEL_DESC_STACK3 + 4], al
	mov byte [LABEL_DESC_STACK3 + 7], ah
	
	xor eax, eax
	mov ax, ss
	shl eax, 4
	add eax, SEG_PROTECT
	mov word [LABEL_DESC_PROTECT + 2], ax
	shr eax, 16
	mov byte [LABEL_DESC_PROTECT + 4], al
	mov byte [LABEL_DESC_PROTECT + 7], ah
	
	xor eax, eax
	mov ax, ds
	shl eax, 4
	add eax, LABEL_GDT
	mov dword [GDT_PTR + 2], eax
	
	lgdt [GDT_PTR]
	
	mov ah, 0x00
	mov al, 0x13
	int 0x10
	
;	mov bx, 0x4101	;VBE的640*480*8bi
;	mov ax, 0x4f02
;	int 10
	
	;关闭所有中断
	cli
	
	;打开20根地址线
	in al, 0x92
	or al, 00000010b
	out 0x92, al
	
	;进入保护模式
	mov eax, cr0
	or eax, 1
	mov cr0, eax
	
	jmp dword SelectorProtect:0

[section .s32]
align 32
[bits 32]
SEG_PROTECT:
	mov ax, SelectorVideo
	mov gs, ax
	mov ax, SelectorData
	mov ds, ax
	mov ax, SelectorStack0
	mov ss, ax
	mov esp, STACK0_TOP
	
	mov edi, (80 * 10 + 1) * 2
	mov ah, 0x0B
	mov al, 'M'
	mov [gs:edi], ax
	
	mov esi, Mystroffset
	mov edi, (80 * 10 + 5) * 2
	mov ah, 0x0B
	call Strdisplay
	
	call Paging
	
	call Move_elf
	mov ax, SelectorPhy
	mov ds, ax
	mov es, ax
	mov ss, ax
	mov esp, STACK0_TOP
	
	jmp SelectorExe:0x50000
	
	jmp $

;------------------------------------------------------------------------------
Strdisplay:
push ax
	mov ax, SelectorVideo
	mov gs, ax
pop ax
	cld
	.str_disp_loop:
		lodsb
		cmp al, 0
		jz .str_disp_loop_end
		mov [gs:edi], ax
		add edi, 2
		jmp .str_disp_loop
	.str_disp_loop_end:
ret	

Paging:
	mov eax, SelectorPage
	mov es, eax
	mov edi, PageDirBase		;让线性地址等于物理地址
	xor eax, eax
	mov eax, PageTblBase | PG_P | PG_USS | PG_RWW
	mov ecx, 1024
.1:
	stosd
	add eax, 4096
	loop .1
	
	mov edi, PageTblBase
	mov ecx, 1048567
	xor eax, eax
	mov eax, 0x0 | PG_P | PG_USS | PG_RWW
.2:
	stosd
	add eax, 4096
	loop .2
	
	;告诉cr3，这个映射关系表在内存的什么地方
	mov eax, PageDirBase
	mov cr3, eax
	
	mov eax, cr0
	or eax, 0x80000000
	mov cr0, eax
ret

Move_elf:
	; ELFBASE		equ		0x9000
	; [es:edi] <- [ds:esi]
	mov ax, ds
	push ax
	mov ax, es
	push ax
	push esi
	push edi
	
		; I decide to use SelectorPhy started with 0 address to load the segment register
		; And then all the offset addresses equal to linear addresses
		
		; note: in paging mode, we must load selector into segment registers
		; instead of read-address
		mov ax, SelectorPhy		; start with 0x0
		mov es, ax				
		mov ax, SelectorElf		; start with 0x90000
		mov ds, ax
		mov ax, SelectorVideo	; start with 0xB8000(video memory)
		mov gs, ax
		
		; start to move ELF file
		; to get some info in the ELF table [PHT_BASE], [PH_NUM], [PH_SIZE], [SEG_MOVED]
		
		mov ax, [ds:e_phnum]	; get the program header number
		mov cx, ax
		mov eax, [ds:e_phoff]	; point to the start of program header table
		mov esi, eax

	.check_every_program_header:
		cmp cx, 0
		jz .move_elf_end
		dec cx
		
		mov eax, [ds:esi]
		cmp eax, 0
		jz .next_program_header
		
		push ecx
			mov ecx, [ds:esi + p_filesz]
			mov edi, [ds:esi + p_vaddr]
			push esi
			mov esi, [ds:esi + p_offset]
			
			.move_loop:
				cmp ecx, 0
				jz .move_loop_end
				dec ecx
				
				mov al, byte [ds:esi]
				mov byte [es:edi], al
				inc edi
				inc esi
				
				jmp .move_loop
			.move_loop_end:
			pop esi
		
		pop ecx
		
	.next_program_header:
		add esi, 0x20
		jmp .check_every_program_header
	
.move_elf_end:
	pop edi
	pop esi
	pop ax
	mov es, ax
	pop ax
	mov ds, ax
ret
	
PROTECT_LEN		equ	$ - SEG_PROTECT - 1














	