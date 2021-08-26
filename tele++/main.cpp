#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define KRED "\x1B[31m"
#define KG  "\x1B[32m"
#define KY  "\x1B[33m"
#define KB  "\x1B[34m"
#define KM  "\x1B[35m"

FILE *fp;

class tele{
  public:
      char Name[20];
      char Address[20];
      unsigned int ph_no;
      void  get(void);
      void show(void);
      void modify(void);
      void delete_user(void);
     
};

void tele::get(void){
   std::cout << " Enter name : " << std::endl;
   std::cin >> Name;
   std::cout << " Enter Address : " << std::endl ;
   std::cin >> Address;
   std::cout << "\n Enter Phone Number : " << std::endl;
   std::cin >> ph_no;
  
}

void tele::show(void) {
   std::cout << "\n ";
   std::cout << "Name :   " << "\t" << Name << std::endl;
   std::cout << "Address : " << "\t" << Address << std::endl;
   std::cout << "Phone Number : " << "\t" << ph_no << std::endl;

}

int main(){
     char ch;
     system("clear");
     std::cout << KRED << "Welcome to Phone Book in C++ " << std::endl;
     std::cout << KM  <<  "1. Add New User  " << std::endl; 
     std::cout << KG  <<  "2. Show User : " << std::endl;
     std::cout << KB  <<  "3. Edit User  " << std::endl;
     std::cout << KM  <<  "4. Delete User  " << std::endl;
     std::cout << "Choose One Option : ";
     std::cin >> ch;
     return 0;
}
