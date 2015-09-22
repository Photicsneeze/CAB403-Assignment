/*
 *  CAB403 Assignment - Semester 2 2015
 *
 *  Created by Lachlan Cesca & Shaun Karran
 */

/* ---- Includes ---- */
#include "client.h"

/* ---- Function Definitions ---- */
int main(int argc, char *argv[])
{
    int         err;                /* Error code for getaddrinfo() */
    int         socket_descriptor;
    addrinfo    hints;              /* Used to set criteria for getaddrinfo() */
    addrinfo    *addr_list;         /* Linked list of addresses returned by getaddrinfo() */
    addrinfo    *addr;              /* Pointer to individual address in addr_list */

    /* Check the user provided the correct arguments. */
    if (argc != 3) {
        printf("Usage: ./client <hostname> <port>\n");
        exit(EXIT_FAILURE);
    }

    /* Set critera for the addresses returned by getaddrinfo. */
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = 0;
    hints.ai_flags = 0;

    /* Get address information of the server. Returns a list of all matches to host, port & hints. */
    if ((err = getaddrinfo(argv[1], argv[2], &hints, &addr_list)) != 0) {
        printf("Failed to get address info: %s\n", gai_strerror(err));
    }

    /* Attempt to conenct to each address from the list. If connection made, leave loop. */
    for (addr = addr_list; addr != NULL; addr = addr->ai_next) {

        /* Create socket using addrinfo. */
        if ((socket_descriptor = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol)) == -1) {
            continue; /* Creating socket failed so move onto the next address in the list. */
        }

        /* Make connection using opened socket. */
        if (connect(socket_descriptor, addr->ai_addr, addr->ai_addrlen) == 0) {
            break; /* Connection succeded. */
        }

        close(socket_descriptor); /* Connection failed for this address, so close. */
    }

    if (addr == NULL) { /* No connections successful. */
        printf("Could not connect.\n");
        exit(EXIT_FAILURE);
    }

    // While connection remains open
        // send & receive data using send & recv OR write & read

    // Close connection using close() system call

    return EXIT_SUCCESS;
}