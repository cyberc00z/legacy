#include <stdio.h>
#include <termios.h>


static struct termios old, new;


void initTermios(int echo)
{

	tcgetattr(0, &old); //grab old terminal I/O setting
	new = old;
	new.c_iflag &= ~ICANON; //disable buffered i/o
	new.c_iflag &= echo ? ECHO : ~ECHO;  // set echo mode
	tcsetattr(0,TCSANOW, &new); //apply terminal io

}


void resetTermios(void)
{
   tcsetattr(0, TCSANOW, &old);
}


char getch_(int echo)
{
  char ch;
  initTermios(echo);
  ch = getchar();
  resetTermios();
  return ch;
}

char getch(void){
   return getch_(0);
}

char getche(void)
{

  return getch_(1);
}


int main(){
   char c;
   printf("(getche example) Please enter a character");
   c = getche();
   printf("\nYou entered : %c\n", c);
   printf("(getch example) Likho Kuch : ");
   c = getch();
   printf("\nYou entered: %c\n", c);
   return 0;

}
