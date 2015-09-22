/*
 *	CAB403 Assignment - Semester 2 2015
 *
 *	Created by Lachlan Cesca & Shaun Karran
 */

/* ---- Includes ---- */
#include "server.h"

/* ---- Function Definitions ---- */
int main(int argc, char *argv[])
{
	int         err;                /* Error code for getaddrinfo() */
    int         socket_descriptor;
    addrinfo    hints;              /* Used to set criteria for getaddrinfo() */
    addrinfo    *addr_list;         /* Linked list of addresses returned by getaddrinfo() */
    addrinfo    *addr;              /* Pointer to individual address in addr_list */

    /* Check the user provided the correct arguments. */
    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Set critera for the addresses returned by getaddrinfo. */
    hints.ai_family =   AF_INET;        /* Use IPv4 internet protocols */
    hints.ai_socktype = SOCK_STREAM;    /* Use connection-based byte streams */
    hints.ai_protocol = 0;              /* Any protocol */
    hints.ai_flags =    AI_PASSIVE;     /* AI_PASSIVE and NULL node return addresses suitable for binding */

    /* Get address information of the server. Returns a list of all matches to host, port & hints. */
    if ((err = getaddrinfo(NULL, argv[1], &hints, &addr_list)) != 0) {
        printf("Failed to get address info: %s\n", gai_strerror(err));
    }

    /* Attempt to bind to each address from the list. If bind successful, leave loop. */
    for (addr = addr_list; addr != NULL; addr = addr->ai_next) {

        /* Create socket using addrinfo. */
        if ((socket_descriptor = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol)) == -1) {
            continue; /* Creating socket failed so move onto the next address in the list. */
        }

        /* Bind using opened socket. */
        if (bind(socket_descriptor, addr->ai_addr, addr->ai_addrlen) == 0) {
            break; /* Bind succeded. */
        }

        close(socket_descriptor); /* Bind failed for this address, so close. */
    }

    if (addr == NULL) { /* No bind successful. */
        printf("Could not bind.\n");
        exit(EXIT_FAILURE);
    }

    /* Start listnening. */
	if (listen(socket_descriptor, LISTEN_BACKLOG) == -1) {
		perror("listen");
		exit(EXIT_FAILURE);
	}

	/* Loop forever. */
	for (;;) {

	}

	return EXIT_SUCCESS;
}
