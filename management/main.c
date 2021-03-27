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
int main_exit;

struct date {
   int day;
   int month;
   int year;
};

struct account {
     int acc_no;
     char name[60];
     struct date deposit;
     struct date today;
     struct date dob;
     struct date withdraw;
     float mobile_no;
     float amt;
} info, add,upd, check ,rem, transaction;




int tl, sl , ts;
void main(){
        menu();
        	
}

float interset(float t, float amount, int rate){
   float SI;
   SI=(rate*t*amount)/100.0;
   return (SI);

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
		      
             case '2':view_list();
		      break;
	     case '3':edit();
		      break;
	     case '4':transcat();
		      break;
             case '5':erase();
		      break;
	     case '6':see();
		      break;
	     case '7': close();
		      break;

	     default:
		    printf("Done!");
		    break;   

	     
	            	      
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
	scanf("%d/%d/%d", &info.today.month, &info.today.day, &info.today.year);
        printf("\n  Mobile No : ");
	scanf("%f", &info.mobile_no);
	printf("\n Date of Birth(mm/dd/yyyy) : ");
	scanf("%d/%d/%d", &info.dob.month, &info.dob.day, &info.dob.year);
	printf("\n  Amount :   ");
	scanf("%f",&info.amt);

	printf("\n  Desposit Date : ");
	scanf("%d/%d/%d", &info.deposit.month,&info.deposit.day, &info.deposit.year);
	printf("\n Withdraw! :  ");
	scanf("%d/%d/%d", &info.withdraw.month, &info.withdraw.day, &info.withdraw.year);
        printf("          All Right you new Account is up now!        ") ;
        printf("\n\n\n\t Enter 1 to go to the main menu and 0 to exit ");
	scanf("%d", &main_exit);
       	system("clear");
	if (main_exit==1)
		menu();
	else if(main_exit==0)
		close();
	else{
	   printf("\nInvalid\n");
	   close();
	}

}

void write_file(){

	FILE *fp;
	fp = fopen("Acc.txt", "w");
	fwrite(&info, sizeof(info), 1, fp);
	fclose(fp);
	return;
 
}

void erase(){
   FILE *fp, *new_rec;
   int test = 0;
   fp = fopen("Acc.txt", "r");
   new_rec = fopen("NewAcc.txt", "w");
   printf("Enter the Account no. of customer you want to delete : ");
   scanf("%d", &rem.acc_no);
   while(fscanf(fp, "%d  %s  %f   %d/%d/%d %d/%d/%d  %f %d/%d/%d   %d/%d/%d  ", &add.acc_no,add.name, &add.mobile_no , &add.today.month,&add.today.day, &add.today.year,&add.dob.month, &add.dob.day,&add.amt, &add.dob.year ,&add.deposit.month, &add.deposit.day, &add.deposit.year, &add.withdraw.month, &add.withdraw.day, &add.withdraw.year)!=EOF)
   {
   
	   if (add.acc_no!=rem.acc_no)
		   printf(new_rec, "%d  %s %f  %d/%d/%d %d/%d/%d  %f %d/%d/%d   %d/%d/%d  ",&add.acc_no,add.name,&add.mobile_no ,&add.today.month,&add.today.day, &add.today.year,&add.dob.month, &add.dob.day,&add.amt, &add.dob.year ,&add.deposit.month, &add.deposit.day, &add.deposit.year, &add.withdraw.month, &add.withdraw.day, &add.withdraw.year );
	   else {
	       test++;
	       printf("\n Record Deleted Successfully!\n");

	   }
	   fclose(fp);
           fclose(new_rec);
	   remove("Acc.txt");
	   rename("NewAcc.txt", "Acc.txt");
	   if (test ==0){
	   	   printf("\nRecord not found!\a\a\a");
	           erase_invalid:
	               printf("\nEnter 0 to try again, 1 to return to main menu and 2 to exit");
		       scanf("%d", &main_exit);
		       if(main_exit==1)
			       menu();
		       else if(main_exit==2)
			       close();
		       else if (main_exit==0)
			       erase();
		       else
			       printf("\nInvalid!\a");
		         
            	       goto erase_invalid;
	   } 
             else
	     {  
		   printf("\nEnter 1 to go to the main meun and 0 to exit:");
	           scanf("%d", &main_exit);
		   sleep(1);
		   system("clear");
		   if(main_exit==1)
			   menu();
		   else
	         	   close();
      
	     }   
   }


}

void close(void){

	printf("\n\n\n This is Bank Management System in C .");

}
