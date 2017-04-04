; serial line routines
; -snp
; init serial port (8n1 115200)
; 
;
serial_sync_read:
	_inb SERIAL_PORT + 5
	and al, 1
	jz serial_sync_read
	ret

serial_sync:
	_inb SERIAL_PORT + 5
	and al, 0x20
	jz serial_sync
	ret

serial_write:
	mov si, buffer.data
serial_write_ex:
	mov word [buffer.count], 0
.cont:
	mov cx, word [buffer.total] 
	cmp word [buffer.count], cx 
	jz .done				
    ; speed up performance in qemu since it doesnt require syncing 
	;call serial_sync
	lodsb					
	_outb SERIAL_PORT, al
	inc word [buffer.count]
	jmp .cont
.done:
	ret

serial_read
	mov word [buffer.total], 0
.cont:
	call serial_sync_read
	_inb SERIAL_PORT
	mov byte [bx], al
	inc bx
	inc word [buffer.total]
	mov cx, word [buffer.total]
	cmp word [buffer.count], cx
	jnz .cont
.done:
	ret
