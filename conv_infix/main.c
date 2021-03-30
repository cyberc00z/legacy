/*
 * Infix -----> (A+B/C*D)
 * POSTFIX ----> ABC*D/+
 *
 * int r(char z)
 * void pushout(char a)
 * void pushs(char b)
 * void pop()
 *
 **/

#include <stdio.h>
#include <stdlib.h>

int i,top1=-1,j,h,f,g;
char n1[20],n[10];
int r(char z)
{

	if ((z=='-') || (z=='+'))
		return 1;
	if ((z=='/'))
		return 2;
	if ((z=='*'))
		return 3;
};

void pushout(char a)
{

	top1=top1+1;
	n[top1]=a;
}
void pushs(char b)
{

	top1=top1+1;
	n1[top1]=b;
}


void pop(){

	for (i=0;i<top1;i++)
		printf("%c",n[i]);
}

void main(){

	char c[10];
	printf("Enter the expression\n");
	scanf("%s", &c);
	for (j=0;c[j]!='\0';j++)
	{
	
		h=0;
		if(c[j]=='('){
		
			pushs(c[j]);
			f=0;
			continue;
		}

		if (((c[j]>='a')&&(c[j]<='z')) || ((c[j]>='A')&&(c[j]<='Z'))){
		
			f=f+1;
			if(f>1){
			
				while(h!=1){
				
					if((r(c[j])>r(n1[top1])) || (n1[top1]=='C'))
					{
					
						h=h+1;
						pushs(c[j]);
						continue;
					}
					for(g=0;(r(c[j]) <=r(n1[top1]));g++)
					{
					
						pushout(n1[top1]);
						top1=top1-1;
						if(n1[top1]=='(')
							break;
					}
				}
			}
		}
	}
}




