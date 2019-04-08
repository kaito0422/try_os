ElfPhyAddr		equ		0x90000
ELFBASE			equ		0x90000     ;BaseElfAddr * 0x10 + OffsetElfAddr

e_entry			equ		24 		; 程序入口地址 
e_phoff			equ		28 		; Program header table在文件中的偏移量（以字节位单位）  
e_phentsize		equ		42 		; Program header table中每一条目的大小 
e_phnum			equ		44 		; Program header table中有多少个条目 

p_type			equ		0		; 当前Program header所描述的段的类型 
p_offset		equ		4		; 段的第一个字节在文件中的偏移 
p_vaddr			equ		8		; 段的第一个字节在内存中的虚拟地址 
p_filesz		equ		16		; 段在文件中的长度 
p_memsz			equ		20		; 段在内存中的长度 
p_align			equ		28		; 定该段在文件或者内存中的对其方式 


PHT_BASE		dd		0		;program header table的基址
PH_SIZE			dd		0		;program header条目的大小 
PH_NUM			dd		0		;program header个数 
PH_ADD			dd		0		;当前program header的位置
SEG_BASE		dd		0		;当前段在文件中的偏移
SEG_SRC			dd		0		;段的源地址 
SEG_DET			dd		0		;段的目的地址 
SEG_LEN			dd		0		;段在文件中的长度  
SEG_MOVED		dd		0		;当前第几个段

