[ORG 0x00]
[BITS 16]

SECTION .text

START:
    mov ax, 0x1000 
    mov ds, ax     
	mov es, ax     

	xor ebx, ebx
	mov di, 0
	mov ecx, 18h
	xor ebp, ebp

CALRAM:
	mov eax, 0xe820
	mov edx, 534d4150h
	int 15h

	test ecx, ecx
	jz NEXTDES

	mov edx, [es:di+16]	;type
	cmp dx, 1
	jne NEXTDES

	mov eax, [es:di+8]	;length
	add ebp, eax

NEXTDES:
	test ebx, ebx
	jnz CALRAM

	shr ebp, 20
	mov eax, ebp
	mov dl, 10
	div dl

	mov bl, al
	and bx, 0xff
	mov cl, BYTE[HEX+bx]
	mov [SIZESTR+11], cl

	shr ax, 8
	mov bx, ax
	mov cl, BYTE[HEX+bx]
	mov [SIZESTR+12], cl

    mov ax, 0x2401 
    int 0x15       

    jc .A20GATEERROR
    jmp .A20GATESUCCESS

.A20GATEERROR:
    in al, 0x92     
    or al, 0x02     
    and al, 0xFE    
    out 0x92, al    
	    
.A20GATESUCCESS:
    cli     
    lgdt [ GDTR ]   

    mov eax, 0x4000003B ; PG=0, CD=1, NW=0, AM=0, WP=0, NE=1, ET=1, TS=1, EM=0, MP=1, PE=1
    mov cr0, eax        
    jmp dword 0x18: ( PROTECTEDMODE - $$ + 0x10000 )

[BITS 32]     
PROTECTEDMODE:
    mov ax, 0x20   
    mov ds, ax   
    mov es, ax          ; ES ���׸�Ʈ �����Ϳ� ����
    mov fs, ax          ; FS ���׸�Ʈ �����Ϳ� ����
    mov gs, ax          ; GS ���׸�Ʈ �����Ϳ� ����
    
    mov ss, ax          ; SS ���׸�Ʈ �����Ϳ� ����
    mov esp, 0xFFFE     ; ESP ���������� ��巹���� 0xFFFE�� ����
    mov ebp, 0xFFFE     ; EBP ���������� ��巹���� 0xFFFE�� ����
    
	push ( SIZESTR - $$ + 0x10000 )
	push 3
	push 0
	call PRINTMESSAGE
	add esp, 12

	push ( SWITCHSUCCESSMESSAGE - $$ + 0x10000 )   
	push 4                                          ; ȭ�� Y ��ǥ(3)�� ���ÿ� ����
    push 0                                          ; ȭ�� X ��ǥ(0)�� ���ÿ� ����
    call PRINTMESSAGE                               ; PRINTMESSAGE �Լ� ȣ��
    add esp, 12                                     ; ������ �Ķ���� ����

    jmp dword 0x18: 0x10200 

PRINTMESSAGE:
    push ebp        ; ���̽� ������ ��������(BP)�� ���ÿ� ����
    mov ebp, esp    ; ���̽� ������ ��������(BP)�� ���� ������ ��������(SP)�� ���� ����
    
	push esi        ; �Լ����� �ӽ÷� ����ϴ� �������ͷ� �Լ��� ������ �κп���
    push edi        ; ���ÿ� ���Ե� ���� ���� ���� ������ ����
    push eax
    push ecx
    push edx
    
    mov eax, dword [ ebp + 12 ] ; �Ķ���� 2(ȭ�� ��ǥ Y)�� EAX �������Ϳ� ����
    mov esi, 160                ; �� ������ ����Ʈ ��(2 * 80 �÷�)�� ESI �������Ϳ� ����
    mul esi                     ; EAX �������Ϳ� ESI �������͸� ���Ͽ� ȭ�� Y ��巹�� ���
    mov edi, eax                ; ���� ȭ�� Y ��巹���� EDI �������Ϳ� ����
    
    mov eax, dword [ ebp + 8 ]  ; �Ķ���� 1(ȭ�� ��ǥ X)�� EAX �������Ϳ� ����
    mov esi, 2                  ; �� ���ڸ� ��Ÿ���� ����Ʈ ��(2)�� ESI �������Ϳ� ����
    mul esi                     ; EAX �������Ϳ� ESI �������͸� ���Ͽ� ȭ�� X ��巹���� ���
    add edi, eax                ; ȭ�� Y ��巹���� ���� X ��巹���� ���ؼ�
                                
    mov esi, dword [ ebp + 16 ] ; �Ķ���� 3(����� ���ڿ��� ��巹��)

.MESSAGELOOP:               ; �޽����� ����ϴ� ����
    mov cl, byte [ esi ]    ; ESI �������Ͱ� ����Ű�� ���ڿ� ��ġ���� �� ���ڸ� 
                            ; CL �������Ϳ� ����
    cmp cl, 0               ; ����� ���ڿ� 0�� ��
    je .MESSAGEEND          ; ������ ������ ���� 0�̸� ���ڿ��� ����Ǿ�����

    mov byte [ edi + 0xB8000 ], cl  ; 0�� �ƴ϶�� ���� �޸� ��巹�� 
    
    add esi, 1              ; ESI �������Ϳ� 1�� ���Ͽ� ���� ���ڿ��� �̵�
    add edi, 2       

    jmp .MESSAGELOOP        ; �޽��� ��� ������ �̵��Ͽ� ���� ���ڸ� ���

.MESSAGEEND:
    pop edx     ; �Լ����� ����� ���� EDX �������ͺ��� EBP �������ͱ����� ���ÿ�
    pop ecx     ; ���Ե� ���� �̿��ؼ� ����
    pop eax     ; ������ ���� �������� �� �����Ͱ� ���� ���� ������
    pop edi     ; �ڷᱸ��(Last-In, First-Out)�̹Ƿ� ����(push)�� ��������
    pop esi     ; ����(pop) �ؾ� ��
    pop ebp     ; ���̽� ������ ��������(BP) ����
    ret         ; �Լ��� ȣ���� ���� �ڵ��� ��ġ�� ����

align 8, db 0

dw 0x0000

GDTR:
    dw GDTEND - GDT - 1         ; �Ʒ��� ��ġ�ϴ� GDT ���̺��� ��ü ũ��
    dd ( GDT - $$ + 0x10000 )   ; �Ʒ��� ��ġ�ϴ� GDT ���̺��� ���� ��巹��

GDT:
    NULLDescriptor:
        dw 0x0000
        dw 0x0000
        db 0x00
        db 0x00
        db 0x00
        db 0x00

    IA_32eCODEDESCRIPTOR:     
        dw 0xFFFF       ; Limit [15:0]
        dw 0x0000       ; Base [15:0]
        db 0x00         ; Base [23:16]
        db 0x9A         ; P=1, DPL=0, Code Segment, Execute/Read
        db 0xAF         ; G=1, D=0, L=1, Limit[19:16]
        db 0x00         ; Base [31:24]  
        
    IA_32eDATADESCRIPTOR:
        dw 0xFFFF       ; Limit [15:0]
        dw 0x0000       ; Base [15:0]
        db 0x00         ; Base [23:16]
        db 0x92         ; P=1, DPL=0, Data Segment, Read/Write
        db 0xAF         ; G=1, D=0, L=1, Limit[19:16]
        db 0x00         ; Base [31:24]
        
    CODEDESCRIPTOR:     
        dw 0xFFFF       ; Limit [15:0]
        dw 0x0000       ; Base [15:0]
        db 0x00         ; Base [23:16]
        db 0x9A         ; P=1, DPL=0, Code Segment, Execute/Read
        db 0xCF         ; G=1, D=1, L=0, Limit[19:16]
        db 0x00         ; Base [31:24]  
        
    DATADESCRIPTOR:
        dw 0xFFFF       ; Limit [15:0]
        dw 0x0000       ; Base [15:0]
        db 0x00         ; Base [23:16]
        db 0x92         ; P=1, DPL=0, Data Segment, Read/Write
        db 0xCF         ; G=1, D=1, L=0, Limit[19:16]
        db 0x00         ; Base [31:24]
GDTEND:

SWITCHSUCCESSMESSAGE:	db 'Switch To Protected Mode Success~!!', 0
SIZESTR:				db 'RAM SIZE : ** MB', 0
HEX:					db '0123456789abcdef'

times 512 - ( $ - $$ )  db  0x00    ; 512����Ʈ�� ���߱� ���� ���� �κ��� 0���� ä��
