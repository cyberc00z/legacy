#include <iostream>
#include <fstream>
#include <cstdlib>
#include <graphics.h>
#include <string.h>
#include <stdlib.h>
#include <conio.h>
#include <stdio.h>
#include <iomanip.h>
#include <dos.h>

using std::cout;
using std::cin;
using std:endl;

class tele{
  public:
      char Name[20];
      char Address[20];
      unsigned int ph_no;
      void  get(void);
      void show(void);
      void modify(void);
};

void tele::get(void){
   cout << "\n Enter name : ";
   cin >> Name;
   cout << "\n Enter Address : ";
   cin >> Address;
   cout << "\n Enter Phone Number : ";
   cin >> ph_no;
  
}

void tele::show(void) {
   cout << "\n ";
   cout << "Name :   " << "\t" << Name << endl;
   cout << "Address : " << "\t" << Address << endl;
   cout << "Phone Number : " << "\t" << ph_no << endl;

}
