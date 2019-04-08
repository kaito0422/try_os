org 0x7C00

jmp LABEL_START
nop

%include "fat12head.inc"

LABEL_START:
	mov ax, cs
	mov ds, ax
	mov ss, ax
	mov es, ax
	mov sp, 0x7C00		;��ջ����spָ��ָ��ջ����Ҳ����˵0x0000~0x7C00���Ƕ�ջ
	
	mov di, (80 * 1 + 1) * 2
	mov si, kaitoStr
	call DisplayStr
	
	;��λ��������BIOS����int 13h
	mov ah, 0
	mov dl, 0
	int 0x13
	
	;��ȡһ����Ŀ¼�����������ڲ���setup.bin���ļ�����Ŀ
	mov word [ReadSectorNum], 19
LABEL_READ_ROOT_DIR_FOR_FILENAME:
	cmp word [RootDirSize], 0	;��Ŀ¼��һ����14���������ж��Ƿ���14���������ҹ���
	jz LABEL_NO_SETUP
	dec word [RootDirSize]
	
	;��ȡһ���������ڴ��У����������ļ���SETUPBIN
	mov ax, BaseLoadAddr
	mov es, ax
	mov bx, OffsetLoadAddr
	mov cl, 1
	mov ax, [ReadSectorNum]
	
	call ReadSector		;��һ��������[es:bx]���ڴ�λ��
	
	;�ڸն�ȡ��������Ѱ���ļ���ΪSETUP.BIN����Ŀ
	mov si, FileName
	mov di, OffsetLoadAddr
	cld
	mov dx, 16	;�ж�һ�������е�16����Ŀ��һ������512���ֽڣ�һ����Ŀ32���ֽڣ�һ��16����Ŀ
LABEL_FIND_FILENAME:
	cmp dx, 0
	jz LABEL_NEXT_SECTOR
	dec dx
	mov cx, 11	;�ļ���Ϊ11���ֽ�
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
	add word [ReadSectorNum], 1	;Ϊ��һ�ε�������׼��
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
	
	mov ax, word [es:di]	;���������ĴغŴ���ax��
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
;��ȡcl���������ڴ����ȡ�������Ŵ���ax��浽�ڴ��[es:bx]λ��
;ʹ��BIOS����int 13h����ȡ����
;
;                                 
;         ������ax          ��al   ����� = al >> 1  ch
;     ----------------  =>         ��ͷ�� = al & 1   dh
;      ÿ���ŵ�������       
;                           ��ah   ��ʼ������ = ah + 1   cl
;
ReadSector:
	mov dl, [BPB_SecPerTrk]
	div dl
	mov ch, al
	mov dh, al
	shr ch, 1	;�����ch׼����
	and dh, 1	;��ͷ��dh׼����
	mov al, cl	;Ҫ����������al׼����
	mov cl, ah
	add cl, 1	;��ʼ������cl׼����
	mov ah, 0x02	;ah׼����
	mov dl, 0
.GoOnReading:
	int 0x13
	jc .GoOnReading
	
	ret
	
;�ҵ�FAT���
GetFATEntry:
	push es
	push bx
	push ax
	
	;���ڴ�������1K�ռ䣬���FAT
	mov ax, BaseLoadAddr
	sub ax, 0x100
	mov es, ax
	pop ax
	
	mov byte [OdEven], 0
	
	;��ȡ�غŶ�Ӧ��FAT�е���ţ�ax=�ֽ����� dx��ʾ��ż
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
	div bx		;ax=FAT�е���������dx=�������е��ֽ�ƫ��
	push dx
	mov bx, 0
	add ax, 1
	mov cl, 2	;һ�ζ�ȡ��������������FAT��պÿ��������ֽ�֮��
	call ReadSector
	
	pop dx
	add bx, dx
	mov ax, [es:bx]
	cmp byte [OdEven], 1
	jnz LABEL_EVEN_2	;�����ż��
	shr ax, 4
LABEL_EVEN_2:
	and ax, 0xFFF
	
	pop bx
	pop es
	ret
	
;��ʾ�ַ���
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