/*
 *  CAB403 Assignment - Semester 2 2015
 *
 *  Created by Lachlan Cesca & Shaun Karran
 */

/* ---- Includes ---- */
#include <errno.h>          /* For perror() */
#include <netdb.h>          /* For hostent struct */
#include <netinet/in.h>     /* Contains structures for internet domain addresses */
#include <pthread.h>
#include <signal.h>         /* To handle SIGINT */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>     /* Needed for using sockets */
#include <sys/types.h>      /* Contains definitions of data types used in system calls */
#include <termios.h>        /* To stop terminal from echoing ^C */
#include <unistd.h>         /* For close() */

#include "authentication.h"
#include "hangman.h"
#include "leaderboard.h"

/* ---- Defines ---- */
#define DEFAULT_PORT        "12345"
#define NO_FLAGS            0
#define LISTEN_BACKLOG      10
#define BUF_SIZE            256
#define DISCONNECT_SIGNAL   "QUIT"
#define PLAY_HANGMAN        1
#define SHOW_LEADERBOARD    2
#define QUIT                3

/* ---- Menu Graphics ---- */
const char WELCOME_MESSAGE[] = "\n"
                              "=============================================\n"
                              "\n"
                              " Welcome to the Online Hangman Gaming System\n"
                              "\n"
                              "=============================================\n"
                              "\n";

const char LOGIN_PROMPT[] = "You are required to logon with your username and password:\n";

const char USERNAME_PROMPT[] = "\nPlease enter your username --> ";
const char PASSWORD_PROMPT[] = "\nPlease enter your password --> ";

const char AUTH_FAILED[] = "\nYou entered either an incorrect username or password - disconnecting...\n";

const char MAIN_MENU[] = "\n"
                        "\n"
                        "===== Main Manu =====\n"
                        "\n"
                        "<1> Play Hangman\n"
                        "<2> Show Leaderboard\n"
                        "<3> Quit\n";

const char MENU_PROMPT[] = "\nSelection option (1 - 3) --> ";

/* ---- Type Definitions ---- */
typedef struct {
    int id;
    int sock_fd;
    bool connected;
} Client_Data;

/* typedef to remove need for struct keyword. */
typedef struct addrinfo addrinfo;

/* ---- Global Variables ---- */
static int  sock_fd;                      /* Initial socket descriptor */
static int  new_sock_fd;                  /* Socket descriptor for new connection */
static bool client_connected = false;
static Leaderboard *leaderboard;
Client_Data client1_data;

/* ---- Function Declarations ---- */
void* handle_client(void *client_data);

bool play_hangman(char *user);

void send_leaderboard();

void get_username(char *username);

void get_password(char *password);

int get_menu_selection();

void write_to_client(int sock_fd, const char *message);

int create_passive_socket(char *port, addrinfo *addr);

void disconnect_client(int sock_fd);

void shutdown_server(int sig);
