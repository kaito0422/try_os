; 在这个位置，cs = 0x0，ds = 0x0，es = 0x0，ss = 0x0
; cs所在段为可读可执行非一致代码段
; es，ds，ss所在段是可读可写数据段

global stack_ring0
global stack_ring1
global stack_ring2
global stack_ring3
global LABEL_TSS

[section .bss]
stack_ring0:	resb 3*1024
Stack0Top:	

[section .bss]
stack_ring1:	resb 2*1024
Stack1Top:	

[section .bss]
stack_ring2:	resb 2*1024
Stack2Top:	

[section .bss]
stack_ring3:	resb 2*1024
Stack3Top:	

[section .tss]
align 32
[bits 32]
LABEL_TSS:
	dd	0
	dd	Stack0Top
	dd	3*8
	dd	Stack1Top
	dd	4*8
	dd	Stack2Top
	dd	5*8
	dd	0
	dd	0
	dd	0
	dd	0
	dd	0
	dd	0
	dd	0
	dd	0
	dd	0
	dd	0
	dd	0
	dd	0
	dd	0
	dd	0
	dd	0
	dd	0
	dd	0
	dd	0
	dw	0
	dw	$ - LABEL_TSS + 2
	db	0x0FF
TSS_LEN		equ	$ - LABEL_TSS - 1

[section .text]
global kernel_start
global load_gdt
global load_idt
global io_out8
global io_in8
global io_cli
global io_sti
global write_vedio
global interrupt_handler
global vector13_handler
global vector32_handler
global vector33_handler
global vector44_handler
global vector_others
global mem_test
global load_tr
global farjmp

extern kstart
extern kmain
extern get_key
extern vector33_handler_c
extern vector44_handler_c
extern vector32_handler_c

kernel_start:
	mov edi, (80 * 11 + 1) * 2
	mov ah, 0x0F
	mov al, 'L'
	mov [gs:edi], ax
	
	mov esi, interrupt_handler
	mov edi, 0x28000
	mov ecx, handler_length
	handler_cp:
		cmp ecx, 0
		jz handler_cp_end
		dec ecx
		mov al, byte [cs:esi]
		mov byte [es:edi], al
		inc esi
		inc edi
		jmp handler_cp
	handler_cp_end:
	
	mov esp, Stack0Top
	
	call kstart
	
;------------------------------------------------------------------------------
	jmp 8:now
now:
	call kmain
	
	mov edi, (80 * 21 + 12) * 2
	mov ah, 0x0F
	mov al, '!'
	mov [gs:edi], ax
	
	mov ax, 9*8
	ltr ax
	
	mov edi, (80 * 12 + 4) * 2
	
	jmp $
	
; void load_gdt(int limit, int addr)
load_gdt:
	mov ax, [esp + 4]
	mov [esp + 6], ax
	lgdt [esp + 6]
ret

; void load_idt(int limit, int addr)
load_idt:
	mov ax, [esp + 4]
	mov [esp + 6], ax
	lidt [esp + 6]
ret

io_out8:
	mov edx, [esp + 4]
	mov al, [esp + 8]
	out dx, al
	nop
	nop
ret

io_in8:
	mov edx, [esp + 4]
	mov eax, 0
	in al, dx
ret

io_cli:
	cli
	nop
ret

io_sti:
	sti
	nop
ret

write_vedio:
	mov ax, 7*8
	mov gs, ax
	mov edi, (80 * 12 + 1) * 2
	mov ah, 0x0B
	mov al, 'X'
	mov [gs:edi], ax
ret

mem_test:	;unsigned int mem_test(unsigned int start, unsigned int end)
push edi
push esi
push ebx
	mov esi, 0xaa55aa55
	mov edi, 0x55aa55aa
	mov eax, [esp + 12 + 4]	; i = start
	mts_loop:
		mov ebx, eax
		add ebx, 0xffc		; p = i + 0xffc
		mov edx, [ebx]		; old = *p
		mov [ebx], esi		; *p = pat0
		xor dword [ebx], 0xffffffff		; *p ^= 0xffffffff;
		cmp edi, [ebx]
		jne mts_fin
		xor dword [ebx], 0xffffffff
		cmp esi, [ebx]
		jne mts_fin
		mov [ebx], edx		; *p = old
		add eax, 0x1000
		cmp eax, [esp + 12 + 8]
		
		jbe mts_loop
		pop ebx
		pop esi
		pop edi
		ret
	mts_fin:
	mov [ebx], edx
pop ebx
pop esi
pop edi	
ret
	
load_tr:	;void load_tr(short tr);
	ltr [esp + 4]
ret

farjmp:	;void farjmp(int eip, int cs);
	jmp far [esp + 4]
ret

[section .handler]
align 32
[bits 32]
interrupt_handler:
vector13_handler:
pushad
	mov ah, 0x0A
	mov al, 'G'
	mov edi, (80 * 12 + 10) * 2
	mov [gs:edi], ax
	pop ax
popad
iretd
nop

vector32_handler:
pushad
	cli
	call vector32_handler_c
	sti
popad
iretd
nop

vector33_handler:
pushad
	cli
	call vector33_handler_c
	mov al, 0x61
	out 0x0020, al
	sti
popad
iretd
nop

vector44_handler:
pushad
	cli
	call vector44_handler_c
	mov al, 0x64
	out 0x00A0, al
	mov al, 0x62
	out 0x0020, al
	sti
popad
iretd
nop

vector_others:
pushad
	mov ah, 0x0F
	mov al, 'O'
	mov edi, (80 * 14 + 1) * 2
	mov [gs:edi], ax
popad
iretd

handler_length	equ	$ - interrupt_handler