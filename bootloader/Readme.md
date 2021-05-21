## Simple Bootloader Program Written in Assembly

#### require libs

1. nasm : sudo apt install nasm

2. qemu : sudo apt install qemu (www.qemu.org)

#### MacOs

brew install nasm

#### Windows

you can use Windows Subsystyem for Linux and you'll aslo need to install an X Server which allows QEMU to open a window from linux subsystem.

# Compilation : 

```
cd boot1

and run following commands 

1. nasm -f bin boot1.asm 

2. qemu-system-x86_64 -fda boot1.bin

```


```
cd boot2

and run following commands 

1. nasm -f bin boot2.asm 

2. qemu-system-x86_64 -fda boot2.bin

```

```
cd boot3

and run following commands

1. nasm -f elf32 boot3.asm

2. i386-elf-_g++ x86_64-elf-g++ -m64 kmain.cpp boot4.o -o kernel.bin -nostdlib -ffreestanding -std=c++11 -mno-red-zone -fno-exceptions -nostdlib -fno-rtti -Wall -Wextra -Werror -T linker.ld  #respective to arch...
```







