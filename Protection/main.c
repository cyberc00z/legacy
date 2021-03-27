#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(){

        FILE *fp;
	int i, c;
	char ch,s[100];
	char r[100]="REN  ";
	char u[] = ".{xxxxxxx}";
	char v[50];
	fp = fopen("input.txt", "w+");
	if (fp==NULL){
	
		printf("Error in opening file a.txt");
		exit(0);
	}
	printf("\n Enter the Path of file : ");
	fflush(stdin);
	fgets(s, 100, stdin);
	for (i=0;i<5;i++){
	
		puts("\n This program is not responsible for any loss in date");
	}
	printf("\n Enter choice :");
	printf("\n1. Protect Folder/File");
	printf("\n2. Unprotect folder/File");
	printf("\n3. Exit\n");
	scanf("%d", &c);

	switch(c)
	{
	
		case 1:
			strcat(r,s);
			printf("\n Enter new name of folder/file : ");
			fflush(stdin);
			fgets(v,50,stdin);
			strcat(r,"");
			strcat(r,v);
			strcat(r,u);
			break;

		case 2:
			strcat(r,s);
			strcat(r,u);
			printf("\n Enter new name of your folder/file : ");
			fflush(stdin);
			fgets(v,50,stdin);
			strcat(r,v);
			break;

		case 3:
			fclose(fp);
			remove("a.txt");
			exit(0);
	}
	fputs(r,fp);
	for(i=0;i<5;i++)
	       	("\n This software is not responsible for any loss of data,...");
	fputs("exit", fp);

	fclose(fp);

	system("a.txt");
	remove("a.txt");
	return 0;




	
}
