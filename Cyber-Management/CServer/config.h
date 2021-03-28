/* Config Module For Server
 *
 *  Date : 28-March-2021
 
 */

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <process.h>
#include <sys/socket.h>
#include <inet/in.h>
#define DEFAULT_PORT 5000
#define STRLEN 256
#define NUM_CLIENTS 10
#define MAX_SERVICES 10
#define NUM_MSG 4


// keys define

#define F1  59  //Client1
#define F2  60 //Client2
#define F3  61  //Client3
#define ESC 37   //Escape Key
#define ALT_A  30

// Threading Handles declaration

HANDLE handleAccept,handleMessages,clientHandle.HandleClients[NUM_CLIENTS];
HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);


int flag_menu =0;
int no_of_clients = 0;
int no_of_msgs = 0;
char recMessage[STRLEN];
char sendMessage[STRLEN];

bool done = false;  // if it is true, the communication ends

SOCKET socket;

struct cTIme{

	int hr, min;
}logIn,logOut,reqTime[MAX_SERVICES];


// Service request structure

struct request{

	int num_request;
	int service_id[MAX_SERVICES];
	int no_of_services;
	struct cTime  reqTime[MAX_SERVICES];
};

struct client 
{

	int id;
	sockaddr_in c_address;
	int c_addr_len;
	SOCKET c_socket;
	bool connected;
	bool turn;
	struct request r;
	struct cTime logIn;
	struct cTime logOut;
	int cost;
	bool active;
};

struct client c[NUM_CLIENTS];   //struct client c[n]; if  multiple clients

int clients[NUM_CLIENTS];
bool SOCKET_START = false;
bool login_server = false;

time_t t; //struct for accessing system time

char *client_ip_address;
char *program_msg[STRLEN];
char cyberName[50];

// Function declarations

void menu();
int get_code(void);

//Utility Function
void readMessage(int msgExtracted[NUM_MSG],char username[20],char password[20], char msg[STRLEN]);
void clearAge(int left, int top, int right, int bottom);
void message_display(char* msg[STRLEN], int disType);
void findStrArrlen( char* str[STRLEN]);
void intToString(int n , char str[]);

void readTime(int timeExtracted[2], char timeString[10]);
void systemTime(char Ms[20]);
void getTime(int x[2]);
void getDate(char sDate[13]);
void newMessage(char msg[]);
bool send_to_network(char *buffer, int client_id);

//Server client related functions

void start_socket();
void start_server();

//bool send_data(char *); void disconnect_client(int)
void shutdown_server();
void echo_message(char *message);
void send_data(client *temp, char *buffer, int size);
bool s_recv_data(char *buffer, int client_id);
bool recv_from_network(char *buffer);
void send_services_list(int client_no);
bool flag_gotoxy;   // checks if gotoxy is busy or not true=busy and false=free
bool flag_clear;
bool flag_newMsg;
bool flag_reset;
void show_admin();
void gotoPrint(int x, int y, char msg[STRLEN]);


// Threads
// void message_display(void *param, int disType)
void clients_table(void *param);
void thd_start_client_thread(void *param);
void thd_accept_connections(void *param);
void thd_receive_data(void *param);
void thd_program_messages(void *param);

//Admin Function part

FILE *login;   // file pointer for login process
FILE *fcost;   // file pointer for cost of login time
FILE *fp;     // file pointer for id , price , qty etc of services


struct userlogin
{

	char username[25];
	char password[25];
	int flag_admin;
};

struct userlogin login1;

struct cost_mem
{

	int rate;
};
struct cost_mem cost1;
struct service_mem
{

	int item_id;
	char item_name[25];
	int item_price;
	int item_qty;
};
struct service_mem service;


// decelaration of functions

char* password(void);
void member_settings_signup();
void member_settings_signin(void);
int check_username(char username[25]);
void menu(void);
void main_lomenu(void);
void settings(void);
void requests();
void cost(void);
void setcost(void);
void viewcost(void);
void admin_settings();
void admin_settings_signup();
void admin_settings_signin();
void change_password();
void services();
void add_services();
void edit_services();
void view_services();
void delete_services();
void search_services();
int check_item_id(int id);
void window();
char* getServiceName(int s_id);
int getRate();
void requests();
int checkUser(char username[20], char password[20]);
int getNumServices();
void decServiceQty(int ser_id);

















