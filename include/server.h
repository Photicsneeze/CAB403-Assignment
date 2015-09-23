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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>     /* Needed for using sockets */
#include <sys/types.h>      /* Contains definitions of data types used in system calls */
#include <unistd.h>         /* For close() */

/* ---- Defines ---- */
#define DEFAULT_PORT    "12345"
#define NO_FLAGS        0
#define LISTEN_BACKLOG  10
#define BUF_SIZE        256

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
const char *PASSWORD_PROMT = "\nPlease enter your password --> ";

/* typedef to remove need for struct. */
typedef struct addrinfo addrinfo;

/* ---- Function Declarations ---- */
bool request_login(int sock_fd);

int create_passive_socket(char *port, addrinfo *addr);
