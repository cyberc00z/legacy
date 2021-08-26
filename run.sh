#! /usr/bin/env bash

CYAN="\e[36m"
GREEN="\e[32m"
ENDCOLOR="\e[0m"
FILE=Makefile

cmd(){
   cmake CMakeLists.txt
   if [ -f "$FILE" ] ; then
       make
   else
     cmake CMakeLists.txt
     make
   fi
}

cmd
echo -e "${GREEN}running cmake from shell"
