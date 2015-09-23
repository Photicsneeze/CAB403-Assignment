/*
 *	CAB403 Assignment - Semester 2 2015
 *
 *	Created by Lachlan Cesca & Shaun Karran
 */

/* ---- Includes ---- */
#include <errno.h>          /* For perror() */
#include <netdb.h>          /* For hostent struct */
#include <netinet/in.h>     /* Contains structures for internet domain addresses */
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>     /* Needed for using sockets */
#include <sys/types.h>      /* Contains definitions of data types used in system calls */
#include <unistd.h>         /* For close() */

/* ---- Defines ---- */
#define NO_FLAGS    	0
#define LISTEN_BACKLOG	10
#define RECV_BUF_SIZE	256

/* typedef to remove need for struct. */
typedef struct addrinfo addrinfo;

/* ---- Function Declarations ---- */
int create_passive_socket(char *port, addrinfo *addr);
