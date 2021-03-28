/*  	Date : 28-March-2021
 *   	Description: Contains all the variable, functions, constant declarations definition..
 *      Author : xadhrit (@xadhrit)
 * */


#include <stdio.h>   //core input and output capillities  of C lang
#include <ctype.h>   // contains functions used to classify characters by their types or to convert between upper and lower case in a way that is independent of used character set
#include <string.h> // Operating all string operation
#include <stdlib.h>  //For performing a variety of operations like conversion, pseudo-random environment, signalling, searching and sorting.
#include <time.h>   // for performing time related operations
#include <unistd.h>  // for sleep 
#include <sys/socket.h>  // for socket programming
#include <netinet/in.h>
#include <process.h>
#include <pthread.h>  //for threading operations

#define DEFAULT_PORT 5000
#define STRLEN 256
#define NUM_CLIENTS 2
#define ESC 27
#define F2 60


FILE *ip;

int port_add; //default
char ipaddress[50];
int no_of_clients = 0;  // counter for number of clients being connected
char recMessage[STRLEN];
char sendMessage[STRLEN];
char *client_ip_address;


//variables to be sent to the server


int computer_id=5;
int service_id=0;
int log_stat = 0;
bool is_memeber = 0;  // to see if the current customer is a member or not 1 mean yes member and 2 mean admin  and 0 mean guest
char username[20];
char password[20];
int tot_cost=0;
char CyberName[20];
int fixed_rate;
int tot_service;
int cost_changed=0;
int check_flag=0;



// Input and Output Handles


HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);

SOCKET Socket;
struct client {


	sockaddr_in c_address;
	int c_addr_len;
	SOCKET c_socket;
	bool connected;
	bool turn;
};

struct client c[NUM_CLIENTS]; // struct client c[n]; if multiple clients

struct service {

	char name[15];
	int rate;
}items[10];

struct ip_info
{

	char ip_address[50];
	int port_no;
	int client_no;
};

struct ip_info ip1;

// checking the sockets

bool SOCKET_START = false;
bool login_server = false;
bool done = false;

time_t t;

int i,j,k;

int flag_menu=0;  //flag for menu
int to_tray = 0; //to check if the user wants to go back to tray mode
void menu(); // Main User Interface of the system

//Some Utility Functions
void login_screen();
void admin_page();
void show_menu();
void service_menu();
void settings_menu();
void order(int ord);
void chg_compID();
void chg_password();
void credits();
void log_out();



// Server client related functions

void start_socket();
void connect_server();
void cl_send_data(char * menu_selected, char *log_status);
int cl_recv_data();
int get_code();
void reset_var();

// thread functions

void show_time(void *param);
void update_price(void *param);

void readMessage(int msgExtracted[4], char msg[STRLEN]);
void intToString(int n, char str[]);
void get_password(char* pass);
void confirm_member();
char* createMessage(char Message[]);
int get_service_info();   // returns the size of the array






