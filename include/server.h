/*
 *  CAB403 Assignment - Semester 2 2015
 *
 *  Created by Lachlan Cesca & Shaun Karran
 */

/* ---- Includes ---- */
#include <stdbool.h>
#include <errno.h>          /* For perror() */
#include <netdb.h>          /* For hostent struct */
#include <netinet/in.h>     /* Contains structures for internet domain addresses */
#include <pthread.h>
#include <signal.h>         /* To handle SIGINT */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>     /* Needed for using sockets */
#include <sys/types.h>      /* Contains definitions of data types used in system calls */
#include <termios.h>        /* To stop terminal from echoing ^C */
#include <unistd.h>         /* For close() */

/* ---- Defines ---- */
#define DEFAULT_PORT    "12345"
#define NO_FLAGS        0
#define LISTEN_BACKLOG  10
#define BUF_SIZE        256
#define SHUTDOWN_SIGNAL "QUIT"

/* ---- Menu Graphics ---- */
const char *WELCOME_MESSAGE = "\n"
                              "=============================================\n"
                              "\n"
                              " Welcome to the Online Hangman Gaming System\n"
                              "\n"
                              "=============================================\n"
                              "\n";

const char *LOGIN_PROMPT = "You are required to logon with your username and password:\n";

const char *USERNAME_PROMPT = "\nPlease enter your username --> ";
const char *PASSWORD_PROMPT = "\nPlease enter your password --> ";

/* typedef to remove need for struct. */
typedef struct addrinfo addrinfo;

/* ---- Global Variables ---- */
static int sock_fd;        /* Initial socket descriptor */
static int new_sock_fd;    /* Socket descriptor for new connection */

/* ---- Function Declarations ---- */
bool request_login(int sock_fd);

int create_passive_socket(char *port, addrinfo *addr);

void shutdown_server(int sig);
