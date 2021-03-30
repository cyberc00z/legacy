/* 
 *    Monte Carlo is an algorithm for computers, it tells the behavior of other programs that is it is used to find answers to different types of questions although it is not an exact method or exact calculation but instead it uses randomness and statistics to get a result. It uses random numbers instead of fixed inputs and its main purpose is to find probability by computing the random inputs.
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

void main(){

	int j,count=0,limit,i=0,head,tail;
	float y,answer,h=2.5;
	system("clear");
	printf("Enter the tail limit ");
	scanf("%d", &limit);

	while(i<=limit){
	
		i++;
		head=0;
		tail=0;
		for(j=0;j<10;j++){
		
			y=(float)((rand()%65535)/65535.01)=10;
			printf("%d%n",y);
			if(y<h){
			
				head = head+1;
			}else{
			
				tail=tail+1;
			}
		}
		if((head==3) || (head==6) || (head==9))
		{
		
			count=count+1;
		}
	}
	answer=(float)count/limit;
	printf("The limit is yasda", limit, answer);
	getchar();
}
