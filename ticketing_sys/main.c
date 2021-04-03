/*  Date : 04/04/2021
 *  Author : Adhrit (https://github.com/xadhrit)
 *  Description: "Terminal Based Ticket Reservation Application"
 */



#include <stdio.h>    //I/O operations
#include <unistd.h>   // for standard symbolic constants and types
#include <stdlib.h>    // standard library fpr conversions and control purpose here
#include "textcolor.c"  // text coloring in terminal
#include <string.h>    // for string related operations

// Declare the main reservation file database

FILE *bt;

//date structure

struct date {
   int day;
   int month;
   int year;
};

//ticket and traveler infomation structure

struct ticket{
  char firstName[10];
  char lastName[10];
  struct date dot;
  struct date dob;
  int age;
  char gender[10];
  char boarding_station[10];
  char destination_station[10];
   
}info;

//decalring global variables and functions

int tl, ts, sl;
void mainmenu();
void feedback();
void book_ticket();
void cancel_ticket();
void check_train();
void write_res();
void seatNo();

// function for main menu

void mainmenu(){

	printf("     %s     MAIN MENU               \n   ", KWHT);
	
	printf("\n1. %sCheck Train\n", KWHT);
	printf("\n2. %sBook Ticket\n", KWHT);
	printf("\n3. %sCancel Ticket\n", KWHT);
	printf("\n4. %sFeedback\n", KWHT);
	printf("\n5. %sExit\n", KWHT);
    
	printf("\n>>>> %sEnter Choice : ", KYEL);
        char ch;
	ch = getchar();
	switch(ch){
		case '1':
			check_train();
                        break;

		case '2':
			system("clear");
			printf("%s::::::::               Book Your Ticket          ::::::::::\n", KWHT);
			book_ticket();
			break;
                case '3':
			system("clear");
			printf("%s------------------  -Cancel Ticket Screen-   ------------------------\n", KWHT);
			cancel_ticket();
			break;

		case '4':
			system("clear");
			printf("%s+++++++++++++++   +  FEEDBACK SYSTEM   +    +++++++++++++++++++++ \n", KWHT);
			feedback();
			break;
           

		case '5':
		        exit(0);

		default:
			system("clear");
			mainmenu();


        }

}

// function for train animation from 1 option

void check_train(){
       	system("clear");
        printf("%s                    Welcome to Train  Check Screen\n", KRED);
	sleep(1.1);
        system("clear");
	char command[100];
	strcpy(command, "sl");
	system(command);
	return;
}

// function for booking ticket from 2 option

void book_ticket(){
	bt = fopen("reservation.txt", "w");
	fseek(bt, 0, SEEK_END);
	tl = ftell(bt);
	sl = sizeof(info);
	ts = tl/sl;
	fseek(bt, (ts-1)*sl, SEEK_SET);
	fread(&info, sizeof(info), 1, bt);
	fclose(bt);

        printf("%sFILL THE FOLLOWING INFORMATION FOR TICKET RESERVATION\n", KYEL);

	printf("  FirstName :  ");
	scanf("%s", info.firstName);
	printf("\n LastName : ");
	scanf("%s", info.lastName);
	printf("\n Date of Travel(mm/dd/yyyy) :    ");
	scanf("%d/%d/%d",&info.dot.month, &info.dot.day, &info.dot.year);
	printf("\n Date of Brith   :");
	scanf("%d/%d/%d", &info.dob.month, &info.dob.day, &info.dob.year);

	printf("\n Boarding Station :   ");
	scanf("%s", info.boarding_station);

	printf("\n Destination Station :   ");
	scanf("%s", info.destination_station);

	printf("\n Gender :  ");
	scanf("%s", info.gender);

	printf("\n Age  : ");

	scanf("%d", &info.age);

        seatNo();

}

// function for displaying Seat No

void seatNo(){
        
	int train = 400 , seath = 23, berth=78;
	int i, count, main_exit;
	for (i=0;i<count ; i++){
	  int seat = (rand() % (berth - seath + 2));
	  int voggy = (rand() % (train*122 - 34));

	  printf("%sYour got seat no %d in Vogage no %d", KWHT,seat, voggy);
	  break;
	}

	printf("\n\n\t Enter 1 to go main menu and 0 to exit\n ");
        scanf("%d", &main_exit);
        if(main_exit == 1){

                mainmenu();
        }
        else if(main_exit==0){

                exit(0);
        }
        else{

                printf("\nInvalid\n");
                exit(1);
        }


}

// function for writing data in text file

void write_res(){

	bt = fopen("reservation.txt", "w");
	fwrite(&info, sizeof(info), 1, bt);
	fclose(bt);
	return;
}

// function for cacelling ticket and removing data from reservation file

void cancel_ticket()
{
   remove("reservation.txt");
   printf("Your reservation is cancelled!!");

}

// function for taking feedback from users

void feedback(){
    
	char feed[90];
	printf("%s       Provide Us Your Valuable Feedback :  ", KRED);
	scanf("%s", feed);

	printf("%sThank you for your feedback :)\n", KGRN);
	FILE *fb;

	fb = fopen("feedback.txt", "r");
        if(!fb){
	  printf("File could not be opened\n\a\a");
	  getchar;
	  return;
	} 
	else {
	
		fprintf(fb,"%s", feed);
	}
        
	fclose(fb);

	fb = fopen("feedback.txt", "w");
	fwrite(&feed, sizeof(feed), 1, fb);
	fclose(fb);
	if(!fb){
	   printf("Could not be opened!");
	   return;
	}
	
	while (!feof(fb)){
                 	
		printf("Feedback Found\n");
		exit(0);
	}
	fclose(fb);

	return;

}


//Driver Code

int main(){
  
	printf("%s                             ::::::::::::::::::::::::::::::::::::: WELCOME TO TICKET BOOKING COUNTER ::::::::::::::::::::::::   \n", KGRN);
	mainmenu();
	return 0;
}







