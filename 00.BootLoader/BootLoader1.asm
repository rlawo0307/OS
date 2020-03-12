[ORG 0x00] ;코드?�� ?��?�� ?��?��?��?���? 0x00?���? ?��?��
[BITS 16]   ; ?��?�� 코드?�� 16비트�? ?��?��

SECTION .text
  
jmp 0x07C0:START
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   MINT64 OS?�� �??��?�� ?��경설?�� �?
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   코드 ?��?�� 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
START:
    mov ax, 0x07C0
    mov ds, ax      ;ds?��그먼?�� ?���??��?��?�� �??��로더 ?��?��?��?��?��?�� ?��?��
    mov ax, 0xB800
    mov es, ax      ;es?��그먼?�� ?���??��?��?�� 비디?��메모�? ?��?�� ?��?��?��?�� ?��?��

    ;?��?�� 초기?�� 코드
    mov ax, 0x0000
	mov ss, ax
	mov sp, 0xFFFF
	mov bp, 0xFFFF
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; ?��면�???���? ?��?��값을 ?��?��?���? ?��?��
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    mov si,    0

.SCREENCLEARLOOP:
	mov byte [ es: si ], 0
    mov byte [ es: si + 1 ], 0x0A	

    add si, 2
    cmp si, 80 * 25 * 2		

    jl .SCREENCLEARLOOP

;?��/?��/?��?�� �? ?��?��?�� �??��?��?�� ?��?��
GETDAY:
    mov ah, 0x04
    int 0x1a
    ;TODO : error 메세�? 추�??(kch)
    ;?��?�� ?��?�� �? ?���?
    xor bx, bx
    mov al, dl
    call .CALCDAY
    ;?��?�� ?��?�� �? ?���?
    mov al, dh
    call .CALCDAY
    xor ax, ax
    ;?��기에 ?��?�� �? ?���?
    mov al, ch
    call .CALCDAY
    ;?��?��?�� ?��?�� �? ?���?
    mov al, cl
    sub bx, 0x1
    call .CALCDAY
    xor ax, ax
    ;?��기�?? ?��?��?�� ?��?��?��?�� 값을 BCD값에?�� ?��?���? �??��?��?�� �?�?
    ;?��?�� 
    mov bl, cl
    mov bh, cl
    and bx, 0xf00f
    shr bh, 4
    mov ax, 0xa
    mul bh
    and bh, 0x0
    add bx, ax
    push bx
    ;?���?
    mov bl, ch
    mov bh, ch
    and bx, 0xf00f
    shr bh, 4
    mov ax, 0xa
    mul bh
    and bh, 0x0
    add bx, ax
    mov ax, 0x64
    mul bx
    pop bx
    add bx, ax                  
    ;?��?��?�� ?���? �??��?��?�� ?��?�� ?���? ?��?�� ?��?��??? ?��?�� ?��?�� -1�? ?��?���? �??��(?��?�� ?��?���? ?��?��?���? 비교?���? ?��?��?��)
    call .CHECKLEAPYEAR     
    mov word[LEAPYEAR1], cx
	sub bx, 1		
    call .CHECKLEAPYEAR
    mov word[LEAPYEAR2], cx
    ;1900?��?�� �??�� ?��?�� ?��?�� 까�???�� ?��?��?�� ?���? 계산?��?��.
    sub cx, 0x1c8   ;456
    sub bx, 0x76b   ;1900-2018?��?�� ?��(365 mod 7 == 1?��기때문에)
    add bx, cx   ;1900-2018(?��?��?��?��?�� ?��?��?�� ?��?���?)
	;?��?�� ?��?��?��?�� 값을 �??��?��?�� �?�?
	mov al, byte[DAY + 3]
	sub al, 0x30
	mov cx, 10
	mul cx
	mov cl, byte[DAY + 4]
	sub cl, 0x30
	add cl, al 
    sub cl, 0x1
    mov di, cx  ;di = 8month
    and ecx, 0xf
    ;미리 ?��?��?�� 배열?��?�� ?��?�� ?��?�� ?��?��?��?�� 값을 �??��?��?��(?��-1)까�??
    mov ax, [MONTH+ecx*2]
    add bx, ax  ;year + month
    ;?��?�� ?��?��?��?�� 값을 �??��?��?�� �?�?
    mov al, byte[DAY + 0]
	sub al, 0x30
	mov cl, 0xa
	mul cl
	mov cl, byte[DAY + 1]
	sub cl, 0x30
	add cl, al
    add bx, cx ;year + month + day
    ;?��?��?���? ?��?��?��?�� �?�?
    mov cx, word[LEAPYEAR1]
    mov ax, word[LEAPYEAR2]
    ;?��?��?���? ?��?��
    sub ax, cx        
    test ax, ax
    jz .PRINTWEEK
    ;2?��?��?�� ?��?��?���? ?��?��
    cmp di, 2
    js .PRINTWEEK
    add bx, 0x1
    jmp .PRINTWEEK
;BCD�? ?��?���? 바꾸?�� ?��?��
.CALCDAY:
    mov ah, al
    and ax, 0xf00f
    shr ah, 4
    add ax, 0x3030
    mov byte[DAY + bx], ah
    add bx, 1
    mov byte[DAY + bx], al
    add bx, 2
    ret
;?��?��?�� ?���? ?��리는 ?��?��
.CHECKLEAPYEAR:
    mov cx, bx
    shr cx, 2
    mov ax, bx
    mov si, 0x64
    xor dx, dx
    div si
    sub cx, ax
    mov ax, bx
    mov si, 0x190
    xor dx, dx
    div si
    add cx, ax
    ret
;미리 ?��?��?�� 배열?��?�� ?��?�� 값을 찾아?�� 출력
.PRINTWEEK:
    add bx, 0x3
    xor dx, dx
    mov ax, bx 
    mov cx, 0x7
    div cx    
    mov ax, dx
    mov bx, 0x3
    mul bx
    mov di, ax
    mov cl, byte[DAYS + di]
    mov byte [ es: 210 ], cl
    mov cl, byte[DAYS + di + 1]
    mov byte [ es: 212 ], cl
    mov cl, byte[DAYS + di + 2]
    mov byte [ es: 214 ], cl
	xor si, si
	mov di, 188
PRINTDAY:
	mov bl, byte [DAY + si]
	test bl, bl
	je RESETDISK

	mov byte [ es: di ], bl
	add si, 1
	add di, 2

	jmp PRINTDAY

RESETDISK:
	mov ax, 0
	mov dl, 0
	int 0x13
	jc HANDLEDISKERROR
;kch stack test
	; mov si, 0x1000  ;물리?�� 주소(0x10000)?�� ?��그먼?��?���??��?�� 값으�? �??��
    mov si, 0x07E0
	mov es, si      ;es?��그먼?�� ?���??��?��?�� �? ?��?��
	mov bx, 0x0000  ;bx?���??��?��?�� 0x0000?�� ?��?��?��?�� 복사?�� ?��?��?��?���? 0x10000?���? ?��?��
READDATA:
    mov ah, 0x02                    ; BIOS ?��비스 번호 2(?��?�� ?���?)
	mov al, 0x1                     ; ?��?�� ?��?�� ?��?�� 1
	mov ch, byte [ TRACKNUMBER ]	;?��?�� ?��?�� 번호 ?��?��
	mov cl, byte [ SECTORNUMBER ]	;?��?�� ?��?�� 번호 ?��?��
	mov dh, byte [ HEADNUMBER ]     ;?��?�� ?��?�� 번호 ?��?��
	mov dl, 0x00                    ;?��?�� ?��?��?���? 번호(?��로피?��?��?��?�� 0)
	int 0x13                        ;?��?��?�� io?��비스�? ?��?��?���? ?��?�� ?��?��?��?��발생
                                    ; (0x13 : ?��?�� 기반 ?��?��/?��로피 ?��?��?�� ?��기·쓰�? ?��비스 ?���?)
	jc HANDLEDISKERROR
READEND:
;kch stack test
    ; jmp 0x1000:0x0000
    jmp 0x07E0:0x0000
HANDLEDISKERROR:
	mov si, 0
	mov di, 360
	
.MESSAGELOOP:
	mov cl, byte [ DISKERRORMESSAGE + si ]
	cmp cl, 0
	jmp .MESSAGEEND

	mov byte [ es: di ], cl
	add si, 1
	add di, 2

	jmp .MESSAGELOOP
.MESSAGEEND:
	jmp $

;?��?��?�� ?��?��--------------------------
DAY:		db '00/00/0000', 0	
DISKERRORMESSAGE:	db 'DISK Err', 0

DAYS:	db 'SUNMONTUEWEDTHUFRISAT', 0
LEAPYEAR1:    dw  0x00
LEAPYEAR2:    dw  0x00
MONTH: dw 0,3,3,6,1,4,6,2,5,0,3,5   ;?��?��?�� ?��?�� 7�? ?��?�� �?

SECTORNUMBER:           db  0x02    
HEADNUMBER:             db  0x00   
TRACKNUMBER:            db  0x00 
times 510 - ( $ - $$ ) db 0x00  ;?��?���??�� 510 address 까�?? 1바이?���? ?��?��
                                ; ?��?�� ?��치에?�� 510 address 까�?? 반복?��?��?��?�� 0x00?���? 채�??

db 0x55
db 0xAA ; 마�??�? 바이?�� 값이 0xAA?���? �??�� 
        ;�??�� ?��?�� 512 바이?��?��?�� 마�??�? 2바이?���? 0x55?�� 0xAA�? ?��?��?���? �??��로더�? ?��?�� ?���? ?��?�� 
