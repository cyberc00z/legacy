#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>

void show_record();
void reset_score();
void help();
void edit_score(float, char[]);

int main(){

	int countr,r,r1,count,i,n;
	float score;
	char choice;
	char playername[20];
	main;
	system("clear");
	printf("\t\t\t C Program Quiz Game\n");
	printf("\n\t\t__________________________________________");
	printf("\n\t\t\t WELCOME ");
	printf("\n\t\t\t  to ");
	printf("\n\t\t\t  The Game");

	printf("\n\t\t__________________________________________");
	printf("\n\t\t WHO WILL BECOME THE MILLIONARE!!!!!!     ");

	printf("\n\t\t_____________________________________________");
	printf("\n\t\t > Press S to Select the game ");
	printf("\n\t\t > Press V to view the highest");
	printf("\n\t\t > Press R to reset score     ");
	printf("\n\t\t > Press H for help           ");
	printf("\n\t\t > Press Q to Exit            ");
	printf("\n\t\t________________________________________________________-");

	choice = getchar();
	
	if(choice=='S'){
	
		system("clear");
		printf("\n\n\n\n\n\n\n\n\n\n\t\tResiter your name : ");
		getchar();
		printf("\n\n    Welcome  %s  to Who will be Millionare  !!  ", playername);
		printf("\n\n Here are some tips you might wanna know before playing :");
		printf("\n________________________________________");
		printf("\n >> There are 2 rounds in this quiz Game, WarmUp Round & Challange Round");
		printf("\n >> In WarmUp round you will be asked total of 3 questions to test your");
		printf("\n >> general knowledge. You are eligible to play game if you give atleast 2 answer");
		 printf("\n    right answers, otherwise you can't proceed further to the Challenge Round.");
    printf("\n >> Your game starts with CHALLANGE ROUND. In this round you will be asked a");
    printf("\n    total of 10 questions. Each right answer will be awarded $100,000!");
    printf("\n    By this way you can win upto ONE MILLION cash prize!!!!!..........");
    printf("\n >> You will be given 4 options and you have to press A, B ,C or D for the");
    printf("\n    right option.");
    printf("\n >> You will be asked questions continuously, till right answers are given");
    printf("\n >> No negative marking for wrong answers!");
    printf("\n\n\t!!!!!!!!!!!!! ALL THE BEST !!!!!!!!!!!!!");
    printf("\n\n\n Press Y  to start the game!\n");
    printf("\n Press any other key to return to the main menu!");
    if(toupper(getchar())=='Y'){
    
	    goto main;
    }
    else {
    
	    goto main;
	    system("clear");
    }
    main;
    system("clear");
    count=0;
    for (i=1;i<=3;i++){
    
	    system("clear");
	    r1=i;
    }
    switch(r1){
    
	    case 1:
		    printf("\n\n Which of the following is Palindrome number ?");
		    printf("\n\ A.40242\t\tB.101010\n\nC.23232\t\tD.01234");
		    if (toupper(getchar()) == 'c')
		    {
		    
			    printf("\n\nCorrect!!!");
			    count++;
			    getchar();
			    break;
		    }
		    else {
		    
			    printf("\n\n Wrong !!!! The correct answer C.23232");
			    getchar();
			    break;
		    }

            case 2:
		    printf("\n\n\nThe country with the highest environment performace index is .......");
		    printf("\n\nA. France\t\tB. Denmark\n\nC.Switzerland\t\tD. Finland");
		    if (toupper(getchar())=='b'){
		    
			    printf("\n\nCorrect!!!");
			    count++;
			    getchar();
			    break;
		    }
		    else {
		    
			    printf("\n\n Wrong!!! Answer is B. Denmark");
			    getchar();
			    break;
		    }
	    case 3:
		    printf("\n\n\n Which animal laughs like human being ?");
		    printf("\n\nA. Polar Beer\t\tB. Heyna \n\nC. Donkey\t\tD. Chimpanzee  ");
		    if (toupper(getchar()) == 'd'){
		    
			    printf("\n\nCorrect!!!!");
			    count++;
			    getchar();
			    break;
		    }
		    else {
		    
			    printf("\n\n Wrong. Answer is D. Chimpanzee");
			    getchar();
			    break;
		    }

	     case 4:
		    printf("\n\n\nWho was awarded the youngest player award in Fifa World Cup 2006?");
		printf("\n\nA.Wayne Rooney\t\tB.Lucas Podolski\n\nC.Lionel Messi\t\tD.Christiano Ronaldo");
		if (toupper(getchar())=='B')
			{printf("\n\nCorrect!!!");count++;
			getchar();
			 break;}
		else
		       {printf("\n\nWrong!!! The correct answer is B.Lucas Podolski");
		       getchar();
		       break;}

           case 5:
                   printf("\n\n\nWhich is the third highest mountain in the world?");
                   printf("\n\nA.Mt. K2\t\tB.Mt. Kanchanjungha\n\nC.Mt. Makalu\t\tD.Mt. Kilimanjaro");
                   if (toupper(getchar())=='B')
                            {printf("\n\nCorrect!!!");count++;
                             getchar();
                             break;}
                    else
		             {printf("\n\nWrong!!! The correct answer is B.Mt. Kanchanjungha");
		              getchar();
		              break;
                   }
		      case 6:
		printf("\n\n\nWhat is the group of frogs known as?");
		printf("\n\nA.A traffic\t\tB.A toddler\n\nC.A police\t\tD.An Army");
		if (toupper(getchar())=='D' )
			{printf("\n\nCorrect!!!");count++;
			getchar();
			break;}
		else
		       {printf("\n\nWrong!!! The correct answer is D.An Army");
		       getchar();
		       break;}}
		       }

         	if(count>=2)
                	{goto test;}
               	else
                 	{
                   	system("cls");
                  	printf("\n\nSORRY YOU ARE NOT ELIGIBLE TO PLAY THIS GAME, BETTER LUCK NEXT TIME");
                  	getchar();
	                goto main;
	            }
            
		test:
                      system("cls");
                      printf("\n\n\t*** CONGRATULATION %s you are eligible to play the Game ***",playername);
                      printf("\n\n\n\n\t!Press any key to Start the Game!");
                      if(toupper(getchar())=='p')
		                 {goto game;}
                game:
                    countr=0;
                    for(i=1;i<=10;i++)
                    {system("cls");
                    r=i;

                     switch(r)
		{
		case 1:
		printf("\n\nWhat is the National Game of England?");
		printf("\n\nA.Football\t\tB.Basketball\n\nC.Cricket\t\tD.Baseball");
		if (toupper(getchar())=='C')
			{printf("\n\nCorrect!!!");countr++;getchar();
			 break;getchar();}
		else
		       {printf("\n\nWrong!!! The correct answer is C.Cricket");getchar();
		       goto score;
		       break;}

		case 2:
		printf("\n\n\nStudy of Earthquake is called............,");
		printf("\n\nA.Seismology\t\tB.Cosmology\n\nC.Orology\t\tD.Etimology");
		if (toupper(getchar())=='A')
			{printf("\n\nCorrect!!!");countr++;getchar();
			 break;}
		else
		       {printf("\n\nWrong!!! The correct answer is A.Seismology");getchar();
		      goto score;
		       break;
		       }

        case 3:
		printf("\n\n\nAmong the top 10 highest peaks in the world, how many lie in Nepal? ");
		printf("\n\nA.6\t\tB.7\n\nC.8\t\tD.9");
		if (toupper(getchar())=='C')
			{printf("\n\nCorrect!!!");countr++;getchar();
			 break;}
		else
		       {printf("\n\nWrong!!! The correct answer is C.8");getchar();
		       goto score;
		       break;}

        case 4:
		printf("\n\n\nThe Laws of Electromagnetic Induction were given by?");
		printf("\n\nA.Faraday\t\tB.Tesla\n\nC.Maxwell\t\tD.Coulomb");
		if (toupper(getchar())=='A')
			{printf("\n\nCorrect!!!");countr++;getchar();
			 break;}
		else
		       {
                printf("\n\nWrong!!! The correct answer is A.Faraday");getchar();
		       goto score;
		       break;
		       }

        case 5:
		printf("\n\n\nIn what unit is electric power measured?");
		printf("\n\nA.Coulomb\t\tB.Watt\n\nC.Power\t\tD.Units");
		if (toupper(getchar())=='B')
			{printf("\n\nCorrect!!!");countr++;getchar(); break;}
		else
		       {
		           printf("\n\nWrong!!! The correct answer is B.Power");
		       getchar();
		       goto score;
		       break;
		       }

		case 6:
		printf("\n\n\nWhich element is found in Vitamin B12?");
		printf("\n\nA.Zinc\t\tB.Cobalt\n\nC.Calcium\t\tD.Iron");
		if (toupper(getchar())=='B' )
			{printf("\n\nCorrect!!!");countr++;getchar();
			 break;}
		else
		       {printf("\n\nWrong!!! The correct answer is B.Cobalt");goto score;
		       getchar();
		       break;}

        case 7:
		printf("\n\n\nWhat is the National Name of Japan?");
		printf("\n\nA.Polska\t\tB.Hellas\n\nC.Drukyul\t\tD.Nippon");
		if (toupper(getchar())=='D')
			{printf("\n\nCorrect!!!");countr++;getchar();
			 break;}
		else
		       {printf("\n\nWrong!!! The correct answer is D.Nippon");getchar();
		       goto score;
		       break;}

        case 8:
		printf("\n\n\nHow many times a piece of paper can be folded at the most?");
		printf("\n\nA.6\t\tB.7\n\nC.8\t\tD.Depends on the size of paper");
		if (toupper(getchar())=='B')
			{printf("\n\nCorrect!!!");countr++;getchar(); break;}
		else
		       {printf("\n\nWrong!!! The correct answer is B.7");getchar();
		       goto score;
		       break;}

        case 9:
		printf("\n\n\nWhat is the capital of Denmark?");
		printf("\n\nA.Copenhagen\t\tB.Helsinki\n\nC.Ajax\t\tD.Galatasaray");
		if (toupper(getchar())=='A')
			{printf("\n\nCorrect!!!");countr++; getchar();
			break;}
		else
		       {printf("\n\nWrong!!! The correct answer is A.Copenhagen");getchar();
		       goto score;
		       break;}

        case 10:
		printf("\n\n\nWhich is the longest River in the world?");
		printf("\n\nA.Nile\t\tB.Koshi\n\nC.Ganga\t\tD.Amazon");
		if (toupper(getchar())=='A')
			{printf("\n\nCorrect!!!");countr++;getchar(); break;}
		else
		       {printf("\n\nWrong!!! The correct answer is A.Nile");getchar();break;goto score;}

        case 11:
		printf("\n\n\nWhat is the color of the Black Box in aeroplanes?");
		printf("\n\nA.White\t\tB.Black\n\nC.Orange\t\tD.Red");
		if (toupper(getchar())=='C')
			{printf("\n\nCorrect!!!");countr++;getchar();
			 break;}
		else
              {printf("\n\nWrong!!! The correct answer is C.Orange");getchar();
              break;goto score;}

        case 12:
		printf("\n\n\nWhich city is known at 'The City of Seven Hills'?");
		printf("\n\nA.Rome\t\tB.Vactican City\n\nC.Madrid\t\tD.Berlin");
		if (toupper(getchar())=='A')
			  {printf("\n\nCorrect!!!");countr++;getchar();
			   break;}
		else
              {printf("\n\nWrong!!! The correct answer is A.Rome");getchar();
              break;goto score;}

		case 13:
		printf("\n\n\nName the country where there no mosquitoes are found?");
		printf("\n\nA.Japan\t\tB.Italy\n\nC.Argentina\t\tD.France");
		if (toupper(getchar())=='D')
			{printf("\n\nCorrect!!!");countr++;getchar();
			break;}
		else
		       {printf("\n\nWrong!!! The correct answer is D.France");getchar();
		       break;goto score;}

        case 14:
		printf("\n\n\nWho is the author of 'Pulpasa Cafe'?");
		printf("\n\nA.Narayan Wagle\t\tB.Lal Gopal Subedi\n\nC.B.P. Koirala\t\tD.Khagendra Sangraula");
		if (toupper(getchar())=='A')
			{printf("\n\nCorrect!!!");countr++;getchar();
			 break;}
		else
		       {printf("\n\nWrong!!! The correct answer is A.Narayan Wagle");getchar();
		       break;goto score;}

		case 15:
		printf("\n\n\nWhich Blood Group is known as the Universal Recipient?");
		printf("\n\nA.A\t\tB.AB\n\nC.B\t\tD.O");
		if (toupper(getchar())=='B')
			{printf("\n\nCorrect!!!");countr++;getchar();
			 break;}
		else
		       {printf("\n\nWrong!!! The correct answer is B.AB");getchar();
		       goto score;
		       break;}

		case 16:
		printf("\n\n\nWhat is the unit of measurement of distance between Stars?");
		printf("\n\nA.Light Year\t\tB.Coulomb\n\nC.Nautical Mile\t\tD.Kilometer");
		if (toupper(getchar())=='A')
			{printf("\n\nCorrect!!!");countr++; getchar();
			break;
			}
		else
		       {printf("\n\nWrong!!! The correct answer is A.Light Year");getchar();
		       goto score;
		       break;}


		case 17:
		printf("\n\n\nThe country famous for Samba Dance is........");
		printf("\n\nA.Brazil\t\tB.Venezuela\n\nC.Nigeria\t\tD.Bolivia");
		if (toupper(getchar())=='A')
			{printf("\n\nCorrect!!!");countr++; getchar();
			break;}
		else
		       {printf("\n\nWrong!!! The correct answer is A.Brazil");getchar();goto score;
		       break;}

		case 18:
		printf("\n\n\nWind speed is measure by__________?");
		printf("\n\nA.Lysimeter\t\tB.Air vane\n\nC.Hydrometer\t\tD.Anemometer\n\n");
		if (toupper(getchar())=='D')
			{printf("\n\nCorrect!!!");countr++; getchar();
			break;}
		else
		       {printf("\n\nWrong!!! The correct answer is D.Anemometer");getchar();goto score;
		       break;}

		case 19:
		printf("\n\n\nWhich city in the world is popularly known as The City of Temple?");
		printf("\n\nA.Delhi\tB.Bhaktapur\n\nC.Kathmandu\tD.Agra\n\n");
		if (toupper(getchar())=='C')
			{printf("\n\nCorrect!!!");countr++; getchar();
			break;}
		else
		       {printf("\n\nWrong!!! The correct answer is C.Kathmandu");getchar();goto score;
		       break;}

		case 20:
		printf("\n\n\nWhich hardware was used in the First Generation Computer?");
		printf("\n\nA.Transistor\t\tB.Valves\n\nC.I.C\t\tD.S.S.I");
		if (toupper(getchar())=='B')
			{printf("\n\nCorrect!!!");countr++; getchar();
			break;}
		else
		       {printf("\n\nWrong!!! The correct answer is B.Valves");getchar();goto score;
		       break;}

		case 21:
		printf("\n\n\nOzone plate is being destroyed regularly because of____ ?");
		printf("\n\nA.L.P.G\t\tB.Nitrogen\n\nC.Methane\t\tD. C.F.C");
		if (toupper(getchar())=='D')
			{printf("\n\nCorrect!!!");countr++; getchar();
			break;}
		else
		       {printf("\n\nWrong!!! The correct answer is D. C.F.C");getchar();goto score;
		       break;}

		case 22:
		printf("\n\n\nWho won the Women's Australian Open Tennis in 2007?");
		printf("\n\nA.Martina Hingis\t\tB.Maria Sarapova\n\nC.Kim Clijster\t\tD.Serena Williams");
		if (toupper(getchar())=='D')
			{printf("\n\nCorrect!!!");countr++; getchar();
			break;}
		else
		       {printf("\n\nWrong!!! The correct answer is D.Serena Williams");getchar();goto score;
		       break;}

		case 23:
		printf("\n\n\nWhich film was awarded the Best Motion Picture at Oscar in 2010?");
		printf("\n\nA.The Secret in their Eyes\t\tB.Shutter Island\n\nC.The King's Speech\t\tD.The Reader");
		if (toupper(getchar())=='C')
			{printf("\n\nCorrect!!!");countr++; getchar();
			break;}
		else
		       {printf("\n\nWrong!!! The correct answer is C.The King's Speech");getchar();goto score;
		       break;}}}
	score:
    system("cls");
	score=(float)countr*100000;
	if(score>0.00 && score<1000000)
	{
	   printf("\n\n\t\t**************** CONGRATULATION *****************");
	     printf("\n\t You won $%.2f",score);goto go;}

	 else if(score==1000000.00)
	{
	    printf("\n\n\n \t\t**************** CONGRATULATION ****************");
	    printf("\n\t\t\t\t YOU ARE A MILLIONAIRE!!!!!!!!!");
	    printf("\n\t\t You won $%.2f",score);
	    printf("\t\t Thank You!!");
	}
	 else
{
	 printf("\n\n\t******** SORRY YOU DIDN'T WIN ANY CASH ********");
	    printf("\n\t\t Thanks for your participation");
	    printf("\n\t\t TRY AGAIN");goto go;}

	go:
	puts("\n\n Press Y if you want to play next game");
	puts(" Press any key if you want to go main menu");
	if (toupper(getchar())=='Y')
		goto main;
	else
		{
		edit_score(score,playername);
		goto main;
		
	}
	
	else if (choice=='V')
	{
                 getchar();	
		show_record();
		goto main;
	}
	else if (choice=='H')
	{
	
		help();getchar();
		goto main;
	}
	else if (choice=='R')
	{
	
	    reset_score();
	    getchar();
	    goto main;

	}
	else if (choice=='Q')
	{
	
	  exit(1);
	}
}


void show_record(){

	system("clear");
	char name[20];
	float scr;
	FILE *f;
	f=fopen("score.txt", "r");
	scanf("%s %f", &name, &scr);

	printf("\n\n\t\t**************************************");
	printf("\n\n\t\t %s has secured the Highest Score %0.2f", name, scr);
	printf("\n\n\t\t ********************************************");
	fclose(f);
	getchar();

}


void reset_record(){

	system("clear");
	printf("\n\n                              HELP");
    printf("\n -------------------------------------------------------------------------");
    printf("\n ......................... C program Quiz Game...........");
    printf("\n >> There are two rounds in the game, WARMUP ROUND & CHALLANGE ROUND");
    printf("\n >> In warmup round you will be asked a total of 3 questions to test your general");
    printf("\n    knowledge. You will be eligible to play the game if you can give atleast 2");
    printf("\n    right answers otherwise you can't play the Game...........");
    printf("\n >> Your game starts with the CHALLANGE ROUND. In this round you will be asked");
    printf("\n    total 10 questions each right answer will be awarded $100,000.");
    printf("\n    By this way you can win upto ONE MILLION cash prize in USD...............");
    printf("\n >> You will be given 4 options and you have to press A, B ,C or D for the");
    printf("\n    right option");
    printf("\n >> You will be asked questions continuously if you keep giving the right answers.");
    printf("\n >> No negative marking for wrong answers");

	printf("\n\n\t*********************BEST OF LUCK*********************************");
}

void edit_score(float score, char pinm[20]){
	system("clear");
	float sc;
	char nm[20];
	FILE *f;
	f=fopen("score.txt", "r");
	scanf("%s %f", &nm,&sc);

	if (score >= sc)
	{
	
		sc= score;
		fclose(f);
		f=fopen("score.txt", "w");
		fprintf(f,"%s\n%.2f",pinm,sc);
		fclose(f);
	}

	
}






