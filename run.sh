#! /usr/bin/env bash

CYAN="\e[36m"
GREEN="\e[32m"
ENDCOLOR="\e[0m"

cmd(){
   cmake CMakeLists.txt
}

cmd
echo -e "${GREEN}running cmake from shell"
