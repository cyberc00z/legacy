#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>


#define RETURNTIME 15

char catagories[][15] = {"Computer", "Electronics", "Eletrical", "Civil", "Mechinal","Material", "Architecture"};
void returnfun();
void menu();
void addbooks();
void deletebooks();
void searchbooks();
void issuebooks();
void viewbooks();
void editbooks();
void closelib();

int getdata();
int t();

void Password();
void issuercrd();
void loaderanim();

FILE *fp , *ft, *fs;

int s;
char findbook;
char password[10] = {"xxxx345690"};



struct date{
   int month;
   int day;
   int year;
};

struct books {
   int id;
   char stname[20];
   char name[20];
   char Author[20];
   int quantity;
   float price;
   int count;
   int rckno;
   char *cat;
   struct date issued;
   struct date duedate;

};

struct books a;



// main function

void main(){
   
          
   Password();
   return ;
   
}

// password protection

void Password(){
    
	system("clear");
	char d[25] = "Password Protection";
	char gh, pass[10];
	int i=0,j;

	for (j=0;j<20;j++){
	
		sleep(2);
		printf("*");

	}
	for (j=0;j<20; j++){
	
		sleep(1);
		printf("%c", d[j]);
	}
	for (j=0;j<20;j++){
	  
		sleep(1);
		printf("*");
	}
	printf("Enter Password : ");
        
	
         	
	while (gh!=13){
	
		gh = getchar();
		if (gh!=13 && gh!=8){
		  putchar('*');
		  pass[i] = gh;
		  i++;
		}
	}
	pass[i] = '\0';
	if(strcmp(pass, password)==0){
		printf("Password Matched!");
	        printf("Press any key to continue .........");
		getchar();
		menu();
	}else{
		printf("\aWarning!! Incorrect Password!");
	        getchar();
		Password();
	 }
}

// display issued book's info


void issuercrd(){


	system("clear");
	printf("The book has taken by Mr. %s", a.stname);
	printf("Issued Date : %d-%d-%d", a.issued.day, a.issued.month , a.issued.year);
	printf("Returning Date : %d-%d-%d", a.duedate.day,a.duedate.month, a.duedate.year);
}



// main menu
void menu(){

	printf("\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2MAIN MENU\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2 \n\n  ");
	printf("   \n 1.  Add Books      ");
	printf("   \n 2.  Delete Books   ");
	printf("   \n 3.  Search Books   \n");
	printf("   \n 4.  Issue Books   \n");
	printf("   \n 5.  View Book List  \n");
	printf("   \n 6.  Edit Book's Record \n");
	printf("   \n 7. Close Library   \n ");

	char ch;
	ch = getchar();
	printf("\n Enter your choicee! : ");
	scanf("%hhd", &ch);
	switch(ch){
	  
		case '1':
			addbooks();
			break;
		case '2':
			deletebooks();
			break;
		case '3':
			searchbooks();
			break;
		case '4':
			issuebooks();
			break;
		case '5':
			viewbooks();
			break;
		case '6':
			editbooks();
			break;
		case '7':
		{
		   system("clear");
		   printf("\n Library Management System");
		   sleep(1);
		   exit(0);
		
		}
		default:
		{
		
		 printf("\a Wrong Option , Please re-enter correct one!");
		 
		 if(ch)
			 menu();
		}
	
	}



}

// function for adding books
void addbooks(){
	system("clear");
	int i;
	
	printf("                       Select an Catagory\n");
	printf("\n1. Computer ");
	printf("\n2. Electronics ");
	printf("\n3. Electrical");
	printf("\n4. Civil ");
	printf("\n5. Mechnical");
	printf("\n6. Architecture");
	printf("\n7. Materials");
	printf("\n8. Back to Menu");
	printf("Enter your choice : ");
	scanf("%d",&s)
        if(s==8){
	   menu();
	   system("clear");
	   fp = fopen("Books.txt", "a");
	   if (getdata() == 1){
	      a.cat=catagories[s-1];
	      fseek(fp,0,SEEK_END);
	      fwrite(&a, sizeof(a),1, fp);
	      fclose(fp);
	      printf("Record is successfully Added!");
	      if(getchar() == 'n'){
		      menu();
               }else{
	           system("clear");
		   addbooks();
	       }


	   }


	}


//function for deleting books
void deletebooks(){

	system("clear");
	int d;
	char another = 'y';
	while(another=='y'){
	    system("clear");
	    printf("Enter book Id to Delete! : ");
	    scanf("%d", &d);
	    fp = fopen("Book.txt", 
			    "r");
	    rewind(fp);
	    while(fread(&a, sizeof(a), 1, fp)==1){
	        if(a.id==d){
		     printf("The book record is available\n");
		     printf("Book name  : %s", a.name);
		     printf("Rack No. : %d", &a.rackno);
		     findbook = 't';
		}
	    }
	    if(findbook != 't'){
	       printf("No record found!");
	       if(getchar()){
	         menu();
	       }
	    }
	    if(findbook=='t'){
	       printf("Do you want to delete this book? (y/n) : ");
	       if (getchar()=='y'){
	          fp= fopen("test.txt", "w");
		  rewind(fp);
		  while(fread(&aq, sizeof(a), 1, fp)==1){
		     if (a.id!=d){
		        fseek(ft,0,SEEK_CUR);
			fwrite(&a,sizeof(a), 1, ft);
			fclose(ft);
		     }
		  }
		  fclose(fp);
		  remove("Book.txt");
		  rename("test.txt", "Book.txt");
		  fp = fopen("Book.txt", "r");
		  if (findbook=='t')
		  {
		     printf("The record is successfully deleted!\n");
		     printf("Delete Another record (y/n) ? ");

		  }
		 
	       }

	    
	    else {
	       menu();
	       fflush(stdin);
	       another=getchar();

	      }
	   }

	   

	}

	//searching function

void searchbooks(){

	system("clear");
	int d;
	printf("********************************Search Books********************"\n);
	printf("\n1. Search By ID \n");
	printf("\n2. Search By Name \n");
	fp = fopen("Boook.txt", "r");
	rewind(fp);
	switch(getchar()){
	
		case '1':
			system("clear");
			printf("**** Search Books By Id *****\n\n");
			printf("Enter the book id: \n");
			scanf("%d", &d);
			printf("\nSearching......\n");
			while (fread(&a, sizeof(a), 1, fp) == 1){
			  if(a.id == d){
			     sleep(2);
			     printf("\nThe book is available\n");
			     printf("ID : %d", a.id);
			     printf("Name : %s", a.name);
			     printf("Author : %s", a.Author);
			     printf("Quantity : %d", a.quantity);
			     printf("Price  : %.2f", a.Price );
			     printf("Rack no : %d", a.rckno);
			     findbook = 't';

			  }
			}
			if (findbook!='t'){
			 
				printf("Not found\n");
			}
			printf("Try Another Search ? (y/n) : ");
			if(getchar()=='y'){
			  searchbooks();
			}
			else {
			   menu();
			}
			break;

		case '2':
			char s[15];
			system("clear");
			printf("*** Search Book by Name****** \n");
			printf("Enter Book Name : ");
			scanf("%s", s);
			int d = 0;
			while (fread(&a, sizeof(a), 1, fp)==1){
			  
				if(strcmp(a.name, (s))==0){
				
					printf("The book is available");
					printf("ID : %d", a.id);
                                        printf("Name : %s", a.name);
                                        printf("Author : %s", a.Author);
                                        printf("Quantity : %d", a.quantity);
                                        printf("Price  : %.2f", a.Price );
                                   	printf("Rack no : %d", a.rckno);
					d++;
				}
			}
			if (d==0){
			
			    printf("\nNo Record Available");
			}
			printf("Try another search(y/n) ? ");
			if (getchar() == 'y')
			{
			      searchbooks();
			}
			else {
			   menu();
		         
			}
			break;
		
	 
                default:
		      getchar();
	              searchbooks();
            
          	} 
                fclose(fp);	
          }



void issuebooks(){


	int t;
	system("clear");
	printf("************************ISSUE SECTION **********************");
	printf("\n1. Issue a Book\n");
	printf("\n2. View Issued Book \n");
	printf("\n3. Search Issued Book \n");
	printf("\n4. Remove Issue Book\n");
	printf("Enter a Choice : ");
	switch(getchar())
	{
	      case '1':
		      system("clear");
		      int c = 0;
		      char another = 'y';
		      while(another=='y'){
		         system("clear");
			 printf("\n**** Issue Book Section ****\n");
			 printf("Enter the fucking book Id :\n");
			 scanf("%d", &t);
			 fp= fopen("Book.txt", "r");
			 fs = fopen("Issue.txt", "a");
			 if(checkid(t)==0){
			 
				 printf("The book record is available\n");
				 printf("There are %d unissued books in library",a.quantity);
				 printf("The name of book is %s ", a.name);
				 printf("Enter student name : ");
				 scanf("%s", a.stname);
                                 printf("Issued date : %d-%d-%d", a.issued.day,a.issued.month,a.issued.year);
				 printf("The book of ID %d is issued ",a.id);
				 a.duedate.day=a.issued.day+RETURNTIME;
				 a.duedate.month=a.issued.month;
				 a.duedate.year = a.issued.year;

				 if (a.duedate.day>30){
				 
					 a.duedate.month+= a.duedate.day/30;
					 a.duedate.day-=30;
				 }
				 if (a.duedate.month>12){
				   
					 a.duedate.year+=a.duedate.month/12;
					 a.duedate.month-=12;
				 }
				 printf("To be Return : %d-%d-%d",a.duedate.day,a.duedate.month, a.duedate.year);
				 fseek(fs, sizeof(a), SEEK_END);
				 fwrite(&a, sizeof(a),1,fs);
				 fclose(fs);
				 c=1;

			 }
			 if (c=0){
			 
				 printf("No record Found\n");
			 }
			 printf("Issue more books (y/n) ? ");
			 fflush(stdin);
			 another = getchar();
			 fclose(fp);


		      }
		      break;

		     
	         case '2' :
		      
		      {
		      
			      system("clear");
			      int j= 4;
			      printf("****************** Issued Book*************\n");
			      printf("STUDENT NAME CATEGORY  ID BOOK NAME  ISSUED  DATE   RETURN   DATE  ");
			      fs = fopen("Issue.txt", "r");
			      while(fread(&a, sizeof(a), 1, fs)==1)
			      {
			         printf("%s", a.stname);
				 printf("%s", a.cat);
				 printf("%s", a.id);
				 printf("%s", a.name);
				 printf("%d-%d-%d", a.issued.day, a.issued.month, a.issued.year);
				 printf("%d-%d-%d", a.duedate.day, a.duedate.month, a.duedate.year);
				 j++;
			      }

		      
			      fclose(fs);
			      returnfunc();

		      }
		      break;

		   case '3':
		      system("clear");
		      printf("Enter book Id : ");
		      int p, c =0;
		      char another = 'y';
		      while (another=='y'){
		
		          scanf("%d", &p);
			  fs= fopen("Issue.txt", "r");
			  while(fread(&a, sizeof(a), 1, fs)==1){
			     if (a.id==p){
			        issuercrd();
				printf("Press any key...\n");
				getchar();
				issuercrd();
				c=1;

			     }
			  }
			  fflush(stdin);
			  fclose(fs);
			  if(c==0){
			    printf("No Record Found!");
			  }
			  printf("Try another Search ? (y/n)");
			  another= getchar();

		        	  
		      }
		      break;

		    case '4':
		      {
		      
			      system("clear");
			      int b;
			      FILE *fg;
			      while (another=='y'){
			      
				      printf("Enter book id to remove : ");
				      scanf("%d", &b);
				      fs = fopen("Issue.txt", "r");
				      while(fread(&a, sizeof(a), 1,fs)==1){
				      
					      if (a.id==b)
					      {
					      
						      issuercrd();
						      findbook = 't''
					      }
					      if (findbook=='t'){
					      
						      printf("DO you want to delte it ? (y/n)");
						      if (getchar()=='y'){
						      
							      fg= fopen("record.txt", "w");
							      rewind(fs);
							      while (fread(&a, sizeof(a), 1, fg);
						      }
					      }
					      fclose(fs);
					      fclose(fg);
					      remove("Issue.txt");
					      rename("record.txt", "Issue.txt");
					      printf("The issued book is removed from list");

				      }
			

			      }
			      if (findbook!=t){
			           printf("No record FOund!");

			      }	
			  }
			  printf("Delete any more?(y/n)");
			  another = getchar();

                   
                          }
		      
	default:
                printf("\nWrong Entry\n");
		getchar();
		issuebooks();
		break;

       }
       returnfun();
	

}



void viewbooks(){


	int library;
	system("clear");
	printf("******************************Book************************\n");
	printf("CATEGORY                ID              BOOK NAME        AUTHOR   QTY     PRICE    RackNo ");
	j = 4;
	fp = fopen("Issue.txt", "r");
	while (fread(&a, sizeof(a), 1, fp) == 1){
	
		printf("%s", a.cat);
		printf("%d", a.id);
		printf("%s", a.name);
		printf("%s", a.Author);
		printf("%d", a.quantity);
		printf("%.2f", a.Price);
		printf("%d", a.rckno);
		printf("\n\n");
		j++;
		i = i+a.quantity;

	}
	printf("Total Books = %d", i);
	fclose(fp);
	returnfun();

}



//editing information of books
void editbooks(){
	system("clear");
	int c = 0;
	int d, e;
	printf("\n***** Edit Books Section *********\n");
	char another = 'y';
	while (another=='y'){
	
	        sleep(1);
		system("clear");
		printf("\nEnter Book Id to be Edited: \n");
		scanf("%d", &d);
		fp = fopen("Book.txt", "r");
		while (fread(&a, sizeof(a), 1, fp)==1){
				
			if (checkid(d)==0){
			
				printf("\nThe book is availble\n");
				printf("The book ID : %d", a.id);
				printf("\nEnter new name : ");
				scanf("%s", a.name);
				printf("\nEnter new Author : ");
				scanf("%s", a.Author);
				printf("\nEnter new quantity : ");
				scanf("%d", &a.quantity);
				printf("\nEnter new price : ");
				scanf("%f", &a.Price);
				printf("\n Enter new rackno : \n");
				scanf("%d", &a.rackno);

				printf("The record is modified");
				fseek(&a, sizeof(a), 1, fp);
				fclose(fp);
				c = 1;


			}
			if (c==0){
			
				printf("No record found\n");
			}
				}
		printf("Modify another record ?  (y/n)");
		fflush(stdin);
		another = getchar();

		

	}
	returnfun();



}


void returnfun(){

	{
	
		printf("Press Enter to return to Main Menu");
	}
	a;
	if (getchar()==13){
	
		menu();
	}
	else {
	
		goto a;
		

	}
	int getdata()
	{
	
		int t;
		printf("Catagory : ");
		printf("%s", catagories[s-1]);
		printf("Book Id:\t");
		scanf("%d", &t);
		if (checkid(t)==0){
		
			printf("\aThe book id already exists\a");
			getchar();
			menu();
			return 0;


		}
	
		a.id = t;
		printf("Book Name : ");
		scanf("%s",a.name);
		printf("Author : ");
		scanf("%s",a.Author);
		printf("Quantity : ");
		scanf("%d", &a.quantity);
		printf("Price :  ");
		scanf("%f", &a.Price);
		printf("Rack No : ");
		scanf("%d", &a.rckno);

	}
        return 1;
}


int checkid(int t)
{

	rewind(fp);
	while (fread(&a, sizeof(a), 1, fp)==1)
	{
	
		if (a.id==t)
			return 0; // returns 0 if book exists
                        
	
		return 0;
	}
}

int t(){


	time_t t;
	time(&t);
	printf("Date and time : %s\n", ctime(&t));

	return 0;
}









