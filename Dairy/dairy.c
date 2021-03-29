/* THINK FAST WHEN YOU TYPE SLOW, DON'T STRUGGLE USE THE FORCE AND LET IT GO
 *
 **/
#include <stdio.h>
#include <stlib.h>
#include <string.h>
#include <unistd.h>

void password();
void dairy();
void add_record();
void show_record();
void delete_record();
void edit_record();

struct record{


	char time[6];
	char name[30];
	char place[25];
	char duration[10];
	char note[500];
};



int main(){
      
	dairy();
	return 0;
}


void dairy(){

	int ch;
	printf("\nWelcome to your Dairy!!\n");
	printf("\n ENter the Password for access!\n");

	printf("*******************************************\n");

	while(1){
	
		printf("\n\n\tMAIN MENU : ");
		printf("\n\n\tADD RECORD\t[1]");
		printf("\n\tVIEW RECORD\t[2]");
		printf("\n\tEDIT RECORD\t[3]");
		printf("\n\tDELETE RECORD\t[4]");
		printf("\n\tEDIT PASSWORD\n\t[5]");
		printf("\n\tEXIT \n\t[6]");
		printf("\n\n\tENTER YOUR CHOICE : ");
		scanf("%d", &ch);
		while (ch)
		{
		
			case 1:
				add_record();
				break;
			case 2:
				view_record();
				break;
			case 3:
				edit_record();
				break;
			case 4:
				delete_record();
				break;
			case 5:
				edit_password();
				break;

			case 6:
			   printf("\n\n\t\tThank You for Using My Dairy!!1");
			   getchar();
			   exit(0);

			default:
			   printf("\n You entered invalid option...");
			   printf("\n\n Please Sir Try Again!");
			   getchar();
			   break;
		}
		system("clear");



	}

	return 0;

}


void add_record(){


	system("clear");
	FILE *fp;
	char another = 'Y', time[10];
	struct record edit;
	char filename[15];
	int choice;

	printf("\n\n\t\t****************************************\n");
	printf("\n\n\t ENTER DATE OF YOUR RECORD:[yyyy-mm-dd]:");
	fflush(stdin);
	fgets(filename, 15, stdin);
	fp = fopen(filename, "a");
	if (fp==NULL)
	{
	
		fp = fopen(filename, "w");
		if (fp==NULL){
		
			printf("\nSYSTEM ERROR...");
			printf("\nPRESS ANY KEY TO EXIT");
			getchar();
			return;
		}
	}
	while (another =='Y' || another=='y'){
	
		choice=0;
		fflush(stdin);
		printf("\n\t ENTER TIME :[hh:mm]:");
		scanf("%s", time);
		rewind(fp);
		while(fread(&e, sizeof(e), 1, fp)==1)
		{
		
			if(strcmp(e.time,time)==0)
			{
			
				printf("\n\t The Record Already exists\n");

				choice = 1;
			}
		}
		if (choice==0){
		
			strcpy(e.time,time):
				printf("\tEnter Name: ");
				fflush(stdin);
			        fgets(e.name,30,stdin);
				fflush(stdin);
				printf("\tENTER PLACE :");
				fgets(e.place,25 ,stdin);
				fflush(stdin);
				printf("\tENTER DURATION:");
				fgets(e.place, 6, stdin);
				fflush(stdin);
				printf("\tNOTE:");
				fgets(e.note,500,stdin);
				fwrite(&e,sizeof(e),1,fp);
				printf("\n Your Record is added...\n");

		}
		printf("\n\tADD ANOTHER RECORD(Y/N) ");
		fflush(stdin);
		another = getchar();

	}
	fclose(fp);
	printf("\n\n\tPress any key to Exit...");
	getchar();
	exit(0);
}


void view_record(){

	FILE *type;
	system("clear");
	struct record customer;
	char time[6], choice, filename[14];
	int ch;

	printf("\n\n\t\t***************************************************\n");
	printf("\t\t HERE IS THE VIEWING MENU  *");
	printf("\n\t ***********************\n\n");
	choice=password();

	if (choice!=0){
	
		return;
	}
	do{
	
		printf("\n\t ENTER THE DATE OF RECORD TO BE VIEWD:[yyyy-mm-dd]");
		fflush(stdin);
		fgets(filename, 14, stdin);
		type = fopen(filename, "r");
		if (type==NULL){
		
			puts("\nThe Record does not exist.....\n");
			printf("Press any key to exit....");
			getchar();
			return;
		}
		system("clear");
		printf("\n\tHow Would You like to View:\n");
		printf("\n\t1. Whole Record of the Day");
                printf("\n\t2. RECORD OF FIC TIME.");
		printf("\n\t3. ENTER YOR CHOICE:");
		scanf("%d", &ch);
		switch(ch)
		{
		
			case 1:
				printf("\n The Whole Record For %sis:", filename);
				while(fread(&customer,sizeof(customer),1,type)==1){
				
					printf("\n");
					printf("\nTIME : %s", customer.time);
					printf("\nMEETING WITH: %s", customer.name);
					printf("\nMEETING AT: %s", customer.place);
					printf("\nDURATION: %s", customer.duration);
					printf("\nNote: %s", customer.note);
					printf("\n");
				}
				break;
			case 2:
				fflush(stdin);
				printf("\nEnter the Time[hh:mm] :");
				fgets(time,6 , stdin);
				
				while (fread(&customer, sizeof(customer), 1, type)==1){
				
					if (strcmp(customer.time, time)==0)
					{
					
						printf("\nYour Record is :");
						printf("\nTime : %s", customer.time);
						printf("\nMeeting With : %s", customer.name);
						printf("\nMeeting At  : %s", customer.palce);
						printf("\nNote: %s", customer.note);
					}
				}
				break;

			default:
				printf("\nYou type something else....\n");
				break;
		}
		printf("\n\nWould you like to continue viewing (y/n) ? ");
		fflush(stdin);
		scanf("%c",&choice);

	}
	while(choice=='Y' || choice=='y');
	fclose(type);
	return;
}


void edit_record(){

	system("clear");
	FILE *type;
	struct record customer;
	char time[
}




