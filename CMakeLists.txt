cmake_minimum_required(VERSION 3.10)

project(legacy)

add_executable(legacy  sys/read.c management/main.c Protection/main.c  ticketing_sys/main.c Dairy/dairy.c Cyber-Management/client.c Cyber-Management/server.c Cyber-Management/cyber.c Cyber-Management/multithreading.c stack/stack.c tele++/main.cpp
               
)
