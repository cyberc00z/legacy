cmake_minimum_required(VERSION 3.10)

project(legacy)

add_executable(legacy  sys/creat.c  sys/lseek.c sys/open.c  sys/truncate.c sys/read.c  sys/write.c memory-allocator/alloc.cc Protection/main.c ticketing_sys/main.c banker_simulation/main.c  monte/main.c management/main.c  banker_simulation/implementation.c banker_simulation/main.c  bank/term.c  tele++/main.cpp  stack/stack.c)
