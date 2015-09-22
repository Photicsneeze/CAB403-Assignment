/*
 *    CAB403 Assignment - Semester 2 2015
 *
 *    Created by Lachlan Cesca & Shaun Karran
 */

/* ---- Includes ---- */
#include <errno.h>          /* For perror() */
#include <netdb.h>          /* For hostent struct */
#include <netinet/in.h>     /* Contains structures for internet domain addresses */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>      /* Contains definitions of data types used in system calls */
#include <sys/socket.h>     /* Needed for using sockets */
#include <unistd.h>         /* For close() */

/* ---- Defines ---- */

/* typedef to remove need for struct. */
typedef struct sockaddr_in sockaddr_in;
typedef struct addrinfo addrinfo;

/* ---- Function Declarations ---- */
