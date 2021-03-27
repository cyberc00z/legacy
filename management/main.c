#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

void menu();
void new_acc();
void view_list();
void edit();
void transcat();
void erase();
void see();
void write_file();

struct date {
   int day;
   int month;
   int year;
};

struct account {
     char name[60];
     struct date date;
     struct date dob;
     float mobile_no;
} info;

int tl, sl , ts;
void main(){
        menu();
        	
}


void menu(){
	int n;
	char ch;
	system("clear");
         
	  printf("               CUSTOMER ACCOUNT BANKING MANAGMENT SYSTEM\n\n");

       	printf("             WELCOME TO MAIN MENU OF SYNC BANK since 2243 \n\n ");      printf("\n1. Create new Account \n\n");
	printf("\n2. Check details of Existing Account\n\n");
        printf("\n3. Update information of existing account\n\n");
        printf("\n4. For Transactions\n\n");
        printf("\n5. Renewing existing account\n\n");
        printf("\n6. View Customer's list\n\n");
        printf("\n7. Exit\n\n");	

	do {
	    printf("\n Select any one option ? ");
	    ch = getchar();
	}while(ch<='0' || ch>'7');
        switch(ch){
	      case '1':
		      for(int i=0; i<n;i++){
		         system("clear");
			 sleep(1);
			 new_acc();
			 write_file();
			 break;
		      }
		      break;
		      
             case '2' :
	            	      
	}
       	return;
      
}

void new_acc(){
        FILE *fp = fopen("Acc.txt", "r");
	fseek(fp, 0, SEEK_END);
	tl=ftell(fp);
	sl = sizeof(info);
	ts = tl/sl;
        fseek(fp, (ts-1)*sl, SEEK_SET);
        fread(&info, sizeof(info), 1, fp);
        fclose(fp);
	
	printf("         Enter following details of Account Holder\n\n") ;
        printf("  Name :  ");
	scanf("%s", info.name);
	printf("\n  Date of Today(mm/dd/yyyy) : ");
	scanf("%d/%d/%d", &info.date.month, &info.date.day, &info.date.year);
        printf("\n Date of Birth(mm/dd/yyyy) : ");
	scanf("%d/%d/%d", &info.dob.month, &info.dob.day, &info.dob.year);
        printf("          All Right you new Account is up now!        ") 
	return;

}

void write_file(){

	FILE *fp;
	fp = fopen("Acc.txt", "w");
	fwrite(&info, sizeof(info), 1, fp);
	fclose(fp);
	return;
 
}

void erase(){
   
}
