bits 16 ; 16 bit mode
org 0x7c00  ;get output at offset 0x7c00

boot:
   mov si, hello  ;point si register to hello label memory location
   mov ah, 0x0e   ;0x0e means 'Write Character in TTY mode';

.loop:
    lodsb
    or al, al ; al == 0 ? 
    jz halt  ; if al==0 jump halt label
    int 0x10  ;runs BIOS interrupt 0x10 - Video Services
    jmp .loop

halt: 
   cli  ;clear interrupt flag
   hlt  ;halt execution

hello: db "First Write a Simple Bootloader!",0

times 510 - ($-$$) db 0 ; pad remiaing 510 bytes with zeros
dw 0xaa55 ;mark this 512 bytes sector
