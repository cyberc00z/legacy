#include <stdio.h>
#include <stdlib.h>

int i,j;     
char n[20], e, c;
FILE *f1, *f2, *fp;
void mainmenu();
void Create();
void Append();
void Copy();
void Display();
void Delete();

void main(){
	mainmenu();
}

void mainmenu(){

	  do {
             system("clear");
             printf("~~~~~~~~~~~~~~TEXT EDITOR~~~~~~~~~~~~~~~~~~");
             printf("\n\t\t MAIN MENU:\n\t\n");
             printf("\n\t1. CREATE\n\t2.DISPLAY\n\t3.APPEND\n\t4.COPY\n\t5.DELETE\n\t6.EXIT\n\t: Choice One Option : \n");
	     char ch;
             ch =  getchar();
	     switch(ch){
	     
		     case '1':
			    Create();
			    break;
	             case '2':
			    Display();
			    break;
	             case '3':
			    Append();
			    break;
		     case '4':
			    Copy();
			    break;
	             case '5':
			    Delete();
	                    break;
		     case '6':
			    exit(0); 
	     }
        }
        while(1);

}

void Create(){
	f1 = fopen("file.txt", "w");
	printf("\n\t Enter Something here! and press '.' to save it\n\n\t ");
	
}


void Append(){


}

void Display(){
}


void Copy(){


}

void Delete(){

}






