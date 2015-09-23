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
    int         sock_fd;

    /* Check the user provided the correct arguments. */
    if (argc != 3) {
        printf("Usage: %s <hostname> <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Function to create socket and connect to the server. */
    sock_fd = create_connection(argv[1], argv[2]);

    /* Attempt to send 1 message to make sure it works. REMOVE LATER */
    if (send(sock_fd, "test", 4, NO_FLAGS) != 4) {
        printf("Failed to send.\n");
        exit(EXIT_FAILURE);
    }

    // While connection remains open
        // send & receive data using send & recv OR write & read

    /* Close socket. */
    close(sock_fd);

    exit(EXIT_SUCCESS);
}

int create_connection(char *host, char *port) {
    int         err;                /* Error code for getaddrinfo() */
    int         sock_fd;
    addrinfo    hints;              /* Used to set criteria for getaddrinfo() */
    addrinfo    *addr_list;         /* Linked list of addresses returned by getaddrinfo() */
    addrinfo    *addr;              /* Pointer to individual address in addr_list */

    /* Set critera for the addresses returned by getaddrinfo. */
    hints.ai_family =   AF_INET;        /* Use IPv4 internet protocols */
    hints.ai_socktype = SOCK_STREAM;    /* Use connection-based byte streams */
    hints.ai_protocol = 0;              /* Any protocol */
    hints.ai_flags =    0;              /* No flags */

    /* Get address information of the server. Returns a list of all matches to host, port & hints. */
    if ((err = getaddrinfo(host, port, &hints, &addr_list)) != 0) {
        printf("Failed to get address info: %s\n", gai_strerror(err));
    }

    /* Attempt to conenct to each address from the list. If connection made, leave loop. */
    for (addr = addr_list; addr != NULL; addr = addr->ai_next) {

        /* Create socket using addrinfo. */
        if ((sock_fd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol)) == -1) {
            continue; /* Creating socket failed so move onto the next address in the list. */
        }

        /* Make connection using opened socket. */
        if (connect(sock_fd, addr->ai_addr, addr->ai_addrlen) == 0) {
            break; /* Connection succeded. */
        }

        close(sock_fd); /* Connection failed for this address, so close. */
    }

    if (addr == NULL) { /* No connections successful. */
        printf("Could not connect.\n");
        exit(EXIT_FAILURE);
    }

    return sock_fd;
}
