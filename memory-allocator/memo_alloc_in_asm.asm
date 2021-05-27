; We have to use sys_brk() call provided from the kernal
;; sys_brk() system call is provided by the kernal , to allocate memory without the need of moving it later. This call allocates memory right behind the application image in the memory. This system function allows you to set the highest available address in the data section.
; in any case of error, sys_brk() returns -1
; a following program will allocate 16kb of memory using sys_brk() system call




section .data
msg  db "Allocated 4kb of memory", 10  
len  equ $-msg

section .data
    global _start

_start:
     mov eax, 45           ;call number 45 for sys_brk()
     xor ebx, eax          ;; cleaning the eax register
     int 0x80              ; kernal call

     add eax, 4096   ;let's allocate 4kb of memory
     mov ebx, eax     
     mov eax, 45
     int 0x80
  
     cmp eax, 0
     jl  exit    ;exit if something went wrong
     mov edi, eax   ;edi - highest ava.. addr
     sub edi, 4
     mov ecx, 1024    ;numbers of DWORDS allocated
     xor eax, eax
     std
     rep stosd
     cld

     mov eax, 4
     mov ebx, 1        
     mov ecx, msg
     mov edx, len
     int 0x80

exit:
     mov eax, 1
     xor ebx, ebx
     int 0x80

  





