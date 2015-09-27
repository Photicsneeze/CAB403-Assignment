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
#include <unistd.h>         /* For close() */

/* ---- Defines ---- */
#define NO_FLAGS            0
#define BUF_SIZE            256
#define DISCONNECT_SIGNAL   "QUIT"

/* typedef to remove need for struct. */
typedef struct addrinfo addrinfo;

/* ---- Global Variables ---- */
static bool quit = false;

/* ---- Function Declarations ---- */
static void *write_socket(void *sock_fd);

static void *read_socket(void *data);

int create_connection(char *host, char *port);

int get_input(char *msg, char *input_str);
