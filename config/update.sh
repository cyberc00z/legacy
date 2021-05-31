#! /usr/bin/env bash
#automating update and upgrade process for my machine
 
#let's add some colors!
 
CYAN="\e[36m"
GREEN="\e[32m"
ENDCOLOR="\e[0m"

echo -e "${CYAN}running update and upgrde commands"
 
update()
{
   sudo apt update
   sudo apt upgrade
}
 
update
echo -e  "${GREEN}your system in up-to-date now ${ENDCOLOR}"
 
~                                              
