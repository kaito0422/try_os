org 0x7C00

jmp LABEL_START
nop

%include "fat12head.inc"

LABEL_START:
	mov ax, cs
	mov ds, ax
	mov ss, ax
	mov es, ax
	mov sp, 0x7C00		;降栈，把sp指针指向栈顶，也就是说0x0000~0x7C00都是堆栈
	
	mov di, (80 * 1 + 1) * 2
	mov si, kaitoStr
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
	mov ax, BaseLoadAddr
	mov es, ax
	mov bx, OffsetLoadAddr
	mov cl, 1
	mov ax, [ReadSectorNum]
	
	call ReadSector		;读一个扇区到[es:bx]的内存位置
	
	;在刚读取的扇区中寻找文件名为SETUP.BIN的条目
	mov si, FileName
	mov di, OffsetLoadAddr
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
	mov si, FileName
	jmp LABEL_FIND_FILENAME
	
LABEL_GO_ON:
	inc di
	jmp LABEL_CMP_FILENAME
	
LABEL_NEXT_SECTOR:
	add word [ReadSectorNum], 1	;为下一次到扇区做准备
	jmp LABEL_READ_ROOT_DIR_FOR_FILENAME
	
LABEL_NO_SETUP:
	mov di, (80 * 2 + 1) * 2
	mov si, NoSetupStr
	call DisplayStr
	
	jmp $	
	
LABEL_FILE_FOUND:
	push di
	mov di, (80 * 2 + 1) * 2
	mov si, FileFoundStr
	call DisplayStr
	pop di

	and di, 0xFFE0
	add di, 0x1A
	
	mov ax, word [es:di]	;把数据区的簇号存在ax中
	push ax
	
	add ax, RootDirSectors
	add ax, DeltaSectorNo
	mov dx, BaseLoadAddr
	mov es, dx
	mov bx, OffsetLoadAddr
	
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
	mov di, (80 * 3 + 1) * 2
	mov si, FileLoaded
	call DisplayStr

;	jmp $
	jmp BaseLoadAddr:OffsetLoadAddr
	
FileFoundStr	db	'Setup found!'
NoSetupStr	db	'There is no setup!'
kaitoStr	db	'Hello, kaito!'
FileLoaded	db	'setup has been loaded!'
;========================================================================================
;读取cl个扇区到内存里，读取的扇区号存在ax里，存到内存的[es:bx]位置
;使用BIOS调用int 13h来读取扇区
;
;                                 
;         扇区号ax          商al   柱面号 = al >> 1  ch
;     ----------------  =>         磁头号 = al & 1   dh
;      每个磁道扇区数       
;                           余ah   起始扇区号 = ah + 1   cl
;
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
	mov ax, BaseLoadAddr
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


times 510 - ($ - $$)	db	0
dw	0xAA55