ElfPhyAddr		equ		0x90000
ELFBASE			equ		0x90000     ;BaseElfAddr * 0x10 + OffsetElfAddr

e_entry			equ		24 		; ������ڵ�ַ 
e_phoff			equ		28 		; Program header table���ļ��е�ƫ���������ֽ�λ��λ��  
e_phentsize		equ		42 		; Program header table��ÿһ��Ŀ�Ĵ�С 
e_phnum			equ		44 		; Program header table���ж��ٸ���Ŀ 

p_type			equ		0		; ��ǰProgram header�������Ķε����� 
p_offset		equ		4		; �εĵ�һ���ֽ����ļ��е�ƫ�� 
p_vaddr			equ		8		; �εĵ�һ���ֽ����ڴ��е������ַ 
p_filesz		equ		16		; �����ļ��еĳ��� 
p_memsz			equ		20		; �����ڴ��еĳ��� 
p_align			equ		28		; ���ö����ļ������ڴ��еĶ��䷽ʽ 


PHT_BASE		dd		0		;program header table�Ļ�ַ
PH_SIZE			dd		0		;program header��Ŀ�Ĵ�С 
PH_NUM			dd		0		;program header���� 
PH_ADD			dd		0		;��ǰprogram header��λ��
SEG_BASE		dd		0		;��ǰ�����ļ��е�ƫ��
SEG_SRC			dd		0		;�ε�Դ��ַ 
SEG_DET			dd		0		;�ε�Ŀ�ĵ�ַ 
SEG_LEN			dd		0		;�����ļ��еĳ���  
SEG_MOVED		dd		0		;��ǰ�ڼ�����
