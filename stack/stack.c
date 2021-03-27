/*                                    Stack Data Structure
 *
 *   Linear Data Structure
 *   LAST IN FIRST OUT (LIFO)   or FIRST IN LAST OUT  (FILO) 
 *   push()                          
 *   pop()
 *
 *   peek() ---> get the top of data element of stack, without removing it
 *   isFull() --> check if stack is full
 *   isEmpty()  --> check if stack is empty
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define MAXSIZE 10

struct stack{
   
        int stk[MAXSIZE];	
	int top;

	
} s;




void push();
int pop();
void display();
int num;
int main(){
              
       printf("             Welcome to Stack operation in C\n");
       printf("1 --> PUSH \n");
       printf("2 --> POP \n");
       printf("3 --> DISPLAY\n");
       printf("4 --> EXIT\n");

       s.top = -1;

       printf("Enter your choice \n");
       int ch;
       scanf("%d", &ch);
       system("clear");
       

       switch (ch){
          case '1':
		  push();
	  break;
	  case '2':
	          pop();
	  break;
	  case '3':
	         display();
          break; 

       }
       return 0;
 
}

void push(){
   
   if (s.top == (MAXSIZE - 1))
   {
      printf("Stack is full\n");
      return;
   }
   else 
   {
      printf("Enter the element to be pushed\n");
      scanf("%d", &num);
      s.top = s.top+1;
      s.stk[s.top] = num;
   }

}

int pop(){
     
     if (s.top == -1){
        printf("Stack is Empty\n");
	return (s.top);
     }
     else {
        num = s.stk[s.top];
	printf("poped element is = %d\n", s.stk[s.top]);
	s.top  = s.top -1;
     }
     return (num);
   
}


void display(){
  int i;
 
  if (s.top == -1){
     printf("Stack is Empty!\n");
     return;
  }
  else {
     printf("\n The status of the stack is\n");
     for (i=s.top;i>=0;i--)
     {
        printf("%d\n", s.stk[i]);
     }
  }
  printf("\n");
}



