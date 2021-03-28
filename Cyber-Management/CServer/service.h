/*
 * Service.h module for SERVER SERVICES
 *
 */

void services(){
	system("clear");
	flag_menu=21;
	printf("\nSERVICES\n");
	printf("\n1. Add Services \n");
	printf("\n2. View Services \n");
	printf("\n3. Edit Services \n");
	printf("\n4. Search Services \n");
	printf("\n5. Delete Services \n");
	printf("Enter YOUR CHOICE: ");

}

void add_services(){
  system("clear");
  flag_menu=211;
  int temp_id,i=0;char another='y';
  printf("\nAdd Services\n");
  fp=fopen("./save/service.txt", "a");
  printf("Item ID: ");
  printf("Item Name: ");
  printf("Item Rate : ");
  printf("Qty: ");
  scanf("%d", &temp_id);
  if(check_item_id(temp_id) == 0){
   
	  printf("Item already exists");
	  printf("Try another one");
	  getchar();
	  add_services();

  }
  service1.item_id=temp_id;fflush(stdin);
  fgets(service1.item_name, buff, stdin);
  scanf("%d", &service1.item_price);
  scanf("%d", &service1.item_qty);
  fseek(fp, -sizeof(service1),SEEK_CUR);
  fwrite(&service1,sizeof(service1),1,fp);
  printf("The data is successfully saved!");
  fclose(fp);
	
}

void view_services(){

	system("clear");
	flag_menu=212;
	int i=0;
	printf("VIEW_SERVICE");
	fp = fopen("./save/service.txt", "r");
	printf("\n Item ID\n");
	printf("Item Name\n");
	printf("Price");
	while(fread(&service1, sizeof(service1),1,fp)==1)
	{
	
		printf("%d", service1.item_id);
		printf("%d", service1.item_name);
		printf("%d", service1.item_price);
		printf("%d", service1.item_qty);
		i++;
	}
	fclose(fp);
}


void search_services(){

	system("clear");
	flag_menu = 214;
	int temp_id, flag=0; char temp_name[25];
	fp=fopen("./save/service.txt", "r");
	printf("\n1. Search Services");
	printf("\n2. Search by Name ");
	printf("Enter your choice : ");
	switch(getchar()){
	
		case '1':
			{
			
				system("clear");
				printf("\n Search By Id");
				scanf("%d",&temp_id);
				while(fread(&service1,sizeof(service1),1,fp)==1)
			        {
				
					if(temp_id==service1.item_id)
					{
					
						flag=9;
						printf("\nThe record is found");
						printf("Item Id: %d", service1.item_id);
						printf("Item Name: %d", service1.item_name);
						printf("Item Price : %d", service1.item_price);
						printf("Qty : %d", service1.item_qty);

					}
				} 	
			   
			}
			if (flag==0){
			
				printf("Sorry Not Found");
			}
			break;

		case '2':
			{
			
				System("clear");
				rewind(fp);
				printf("Enter The Name of Service you want to search");
				fgets(temp_name, buff, stdin);
				fflush(stdin);
				while(fread(&service1,sizeof(service1),1,fp)==1){
				
					if(strcmp(temp_name, service1.item_name)==0){
					
						printf("Record Found!!");
						printf("Item Id : %d", service1.item_id);
						printf("Item Name : %d", service.item_name);
						printf("Item Price : %d", service.item_price);
						printf("Qty : %d", service.item_qty);

					}
				}
				if(flag==0){
				
					printf("Sorry!!");
				}
			}
			break;
		default:
			search_services();
			break;

	}
	fclose(fp);
}


void delete_services(){

	system("clear");
	FILE *temp_file;
	int temp_id, flag=0;
	flag_menu=215;
	printf("REMOVE SERVICE :");
	printf("Enter the ID to delete : ");
	scanf("%d", &temp_id);
	fp=fopen("./save/service.txt", "r");
	while(fread(&service1,sizeof(service1), 1, fp)==1){
	
		if (temp_id==service1.item_id)
		{
		
			flag=11;
			printf("The record is found!");
		}
	}
	if(flag==0){
	
		printf("Not found!");
	}
	if(flag==11){
	
		printf("Are you want to delete ?  (y/n) : ");
		if(getchar()=='y'){
		
			temp_file = fopen("./save/temp.txt", "w");
			rewind(fp);
			while(fread(&service1, sizeof(service1), 1, fp)==1){
			
				if(temp_id!=service1.item_id){
				
					fseek(fp,0,SEEK_END);
					fwrite(&service1, sizeof(service1), 1, temp_file);

				}
			}
			fclose(temp_file);
			fclose(fp);
			remove("./save/service.txt");
			rename("./save/temp.txt", "./save/service.txt");
			fp =fopen("./save/service.txt", "r");
			printf("The item is deleted!");


		}

	}
}


int check_item_id(int id){

	FILE *item_id_Chk;
	item_id_Chk=fopen("service.txt", "r");
	while (fread(&service1, sizeof(service1), 1, item_id_Chk)==1)
	
	{
	
		if(id==service1.item_id)
		{
		
			fclose(item_id_Chk);
			return 0;
		}

	}
	return 1;
}

void edit_services(){

	system("clear");
	int temp_id, flag=0, temp_id2, temp_name2,item_price2,item_qty2;
	fp=fopen("./save/service.txt", "r");
	flag_menu=213;
	printf("EDIT SERVICE");
	printf("Enter the ID to modify :");
	fflush(stdin);
	scanf("%d", &temp_id);
	while(fread(&service1,sizeof(service1),1,fp)==1){
	
		if(temp_id==service1.item_id){
		
			printf("Item found....");
                         
			printf("\nEnter new ID :");
			scanf("%d", &item_id2);
			printf("\nEnter new Name:");
			scanf("%s", item_name2);
			printf("\nEnter new Price:");
			scanf("%d",&item_price2);
			printf("\nEnter new quantity :");
			scanf("%d", &item_qty2);

			if (check_item_id(temp_id2)==0 && temp_id2!=temp_id){
			
				printf("The Item ID already exists!");
			        printf("Try another one");
				getchar();
				edit_services();
			}
			service1.item_id=temp_id2;
			fflush(stdin);
			fgets(service1, buff, stdin);
			scanf("%f", &service1.item_price);
			fseek(fp,-sizeof(service1),SEEK_CUR);
			fwrite(&service1,sizeof(service1),1,fp);
			fclose(fp);
			flag=12;

		}
	}
	if(flag==12)
	{
	
		printf("The item is successfully modified!!");
	}
	if (flag==0){
	
		printf("Sorry !! No record");
		printf("Press Any Key..");
	}
}









