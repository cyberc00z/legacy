#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include "color.h"
#include "gotoxy.h"

void input();
void writefile();
void search();
void output();


struct date {
   int month;
   int day;
   int year;
};

struct account {
      int number;
      char name[100];
      int acc_no;
      float mobile_no;
      char street[100];
      char city[100];
      char acct_type;
      float oldbalance;
      float newbalance;
      float payment;
      struct date lastpayment;

} customer;


int tl, sl, ts;
int main(){
    int i, n;
    char ch;    
    system("clear");

    
    printf(" CUSTOMER BILLING SYSTEM:\n\n");
    printf("===================================\n\n");
    printf("\n1 :  to add account on list\n ");
    printf("\n2 :  to search customer account\n");
    printf("\n3 :  exit\n");

    do {
         printf("\n Select any one option ?");
	 ch = getchar();

      }while(ch<='0' || ch>'3');
     switch(ch){
          case '1':
		  system("clear");
		  printf("\n how many customer accounts?");
		  scanf("%d", &n);
		  for (i=0; i<n;i++){
		     input();
		     if (customer.payment>0)
			     customer.acct_type=(customer.payment*customer.oldbalance)? '0':'0';
		     else
			     customer.acct_type=(customer.oldbalance>0)?'D':'C';
                    
	             customer.newbalance=customer.oldbalance - customer.payment;
	             writefile();    	     
		        
		  }
		  main();

         case '2':
		  system("clear");
		  printf("Search by What?\n");
		  printf("\n1 --- Search by Customer number\n");
		  printf("\n2 --- Search by Customer name \n");
		  search();
		  ch=getchar();
		  main();

         case '3':
		  system("clear");
		  sleep(2);
		  
     
     } 

}
   

void input ()
{
   FILE *fp = fopen("input.txt", "r");
   fseek(fp,0,SEEK_END);
   tl=ftell(fp);
   sl = sizeof(customer);
   ts = tl/sl;
   fseek(fp,(ts-1)*sl, SEEK_SET);
   fread(&customer, sizeof(customer), 1, fp);
   printf("\ncustomer no: %d\n", ++customer.number);

   fclose(fp);
   printf("      Account Number : ");
   scanf("%d", &customer.acc_no);
   printf("\n , Name : ");
   scanf("%s", customer.name);
   printf("\n,  mobile_no :  ");
   scanf("%f", &customer.mobile_no);
   printf("   Street : ");
   scanf("%s", customer.street);
   printf("    City :   ");
   scanf("%s", customer.city);
   printf("   Previous balance :  ");
   scanf("%f",&customer.oldbalance);
   printf( "  Current Payment : ");
   scanf("%f", &customer.payment);
   printf("    Payment date(mm/dd/yyyy) : ");
   scanf("%d/%d/%d", &customer.lastpayment.month, &customer.lastpayment.day, &customer.lastpayment.day,&customer.lastpayment.year);
   return;

}


void writefile(){

   FILE *fp;
   fp= fopen("input.txt", "w");
   fwrite(&customer,sizeof(customer), 1, fp);
   fclose(fp);
   return;
}

void search(){
  
	char ch;
	char nam[100];
	int n, i, m=1;
	FILE *fp;
	fp=fopen("input.txt", "r");
	do{
	   printf("\nEnter your choice : ");
	   ch = getchar();

	}while(ch!='1' && ch!='2');
	switch(ch){
	     case '1':
		     fseek(fp,0, SEEK_END);
		     tl= ftell(fp);
		     sl=  sizeof(customer);
		     ts = tl/sl;
		     do {
		        printf("\nChoose customer number");
			scanf("%d", &n);

			if (n<=0||n>ts)
				printf("\nEnter choice\n");
			else{
			    fseek(fp, (n-1)*sl, SEEK_SET);
			    fread(&customer, sl, 1, fp);
			    output();

			}
			printf("\n\nagain?(y/n)");
			ch = getchar();

		     }while(ch=='y');
		       fclose(fp);
		       break;

              case '2':
		       fseek(fp,0, SEEK_END);
		       tl=ftell(fp);
		       sl=sizeof(customer);
		       ts=tl/sl;
		       fseek(fp,(ts-1)*sl, SEEK_SET);
		       fread(&customer, sizeof(customer), 1, fp);
		       n= customer.number;
		       do{
		          printf("\n Enter the name : ");
			  scanf("%s", nam);
			  fseek(fp, 0, SEEK_SET);
			  for (i=0; i<=n; i++){
			     fread(&customer, sizeof(customer), 1, fp);
			     if (strcmp(customer.name, nam) == 0){
			         output();
				 m=0;
				 break;
			     }
			  
			  }
			  if(m!=0)
		              printf("\n\ndoesn't exist\n");
			  printf("\n\nanother?(y/n)");
			  ch = getchar();
		       } while(ch=='y');
		       fclose(fp);

		     }
		     return;
	}

void output(){
   
	printf("\n\n Customer no : %d\n", customer.number);
	printf("\n\n Name :%s\n", customer.name);
	printf("\n\n Mobile no : %.f\n", customer.mobile_no);
	printf("\n\n Account Number", customer.acc_no);
	printf("\n\n Street :%s\n", customer.street);
	printf("\n\n City : %s\n", customer.city);
	printf("\n\n Old balance : %.2f\n", customer.oldbalance);
	printf("\n\n Current Balance : %.2f\n", customer.payment);
	printf("\n\n New Balance : %.2f\n", customer.newbalance);
	printf("\n\n Payment date : %d/%d/%d\n\n", customer.lastpayment.month,customer.lastpayment.day,customer.lastpayment.year);
	printf("   Account Status : ");
	KBLU;
	switch(customer.acct_type)
	{
	   case 'C':
		   printf("CURRENT\n\n");
		   break;
	   case 'O':
		   printf("OVERDUE\n\n");
		   break;

           case 'D':
		   printf("DELINQUENT\n\n");
		   break;

	   default:
		   printf("ERROR\\n\n");
	}
	
	return;


}   









