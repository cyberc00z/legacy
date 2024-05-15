// Linear Data Structure

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#define MAXSIZE 10

struct Stack {
    int data[MAXSIZE];
    int top;
}stk;

void push();
int pop();
void display();
int num;


int main(){
    stk.top = -1;
    printf("Enter the number of elements that you want to insert \n");
    printf("1 --> PUSH \n");
    printf("2 --> POP \n");
    printf("3 --> DISPLAY \n");
    printf("4 --> EXIT \n");

    int ch; 
    scanf("%d", &ch);
    switch (ch) {
        case 1: push();
        case 2: pop();
        case 3: display();
        case 4:
          printf("Exiting....");
          exit(0);
          break; 
    }
    
}

void push(){
    if (stk.top == (MAXSIZE - 1))
    {
       printf("Stack is full \n"); 
    }
    else {
        stk.top++;
        printf("Enter the element to be inserted \n");
        scanf("%d",  &num);
        stk.data[stk.top] = num;
    }
} 

int pop (){
    if (stk.top == -1) {
        printf("Stack is empty"); 
        return -1;
    }
    else {
        
        printf("Popped element is =%d\n", stk.data[stk.top]);
        stk.top--;
        return stk.data[stk.top];
    }
}

void display(){
    int i;
    if (stk.top == -1){
        printf("Stack is empty");
        return;
    } else {
        printf("\n The status of the stack \n"); 
        for (int i=stk.top; i>=0; i--){
            printf("%d", stk.data[i]);
        }
    }
    printf("\n");
}
