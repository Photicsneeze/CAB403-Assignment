/*
 *  CAB403 Assignment - Semester 2 2015
 *
 *  Created by Lachlan Cesca & Shaun Karran
 */

/* ---- Includes ---- */
#include "server.h"

/* ---- Function Definitions ---- */
int main(int argc, char *argv[])
{
    int         ret;                    /* Return value for recv() */
    int         sock_fd;                /* Initial socket descriptor */
    int         new_sock_fd;            /* Socket descriptor for new connection */
    addrinfo    *addr;                  /* Contains internet address information of server */
    char        recv_buf[BUF_SIZE];     /* Buffer to store data received from client */

    /* Check the user provided the correct arguments. */
    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Function to create socket, bind it and mark it to accept incoming connections. */
    sock_fd = create_passive_socket(argv[1], addr);

    /* Main server loop. Accept incomming connections, send/recv data, close connection. */
    for (;;) {
        /* Accept a new connection, returns a new socket_descriptor, leaving original in listening state.
         * Accept is a blocking function and will wait for a connection if none available.
         */
        if ((new_sock_fd = accept(sock_fd, addr->ai_addr, &addr->ai_addrlen)) == -1) {
            perror("Error accepting connection");
            exit(EXIT_FAILURE);
        }

        printf("Connection accepted.\n");

        for (;;) {
            /* Receive message from new socket and store data in buffer. */
            if ((ret = recv(new_sock_fd, recv_buf, BUF_SIZE, NO_FLAGS)) == -1) {
                perror("Error receiving data");
                exit(EXIT_FAILURE);
            } else if (ret == 0) {
                printf("Connection closed.\n");
                break;
            }

            /* Just printing received data for testing purposes. */
            printf("%s\n", recv_buf);
        }

        /* Close the newly created socket once client if finished. */
        //close(new_sock_fd);
    }

    exit(EXIT_SUCCESS);
}

int create_passive_socket(char *port, addrinfo *addr)
{
    int         ret;            /* Return value for getaddrinfo() */
    int         sock_fd;        /* Socket descriptor to return */
    addrinfo    hints;          /* Used to set criteria for getaddrinfo() */
    addrinfo    *addr_list;     /* Linked list of addresses returned by getaddrinfo() */

    /* Set critera for the addresses returned by getaddrinfo. */
    hints.ai_family =   AF_INET;        /* Use IPv4 internet protocols */
    hints.ai_socktype = SOCK_STREAM;    /* Use connection-based byte streams */
    hints.ai_protocol = 0;              /* Any protocol */
    hints.ai_flags =    AI_PASSIVE;     /* AI_PASSIVE and NULL node return addresses suitable for binding */

    /* Get address information of the server. Returns a list of all matches to host, port & hints. */
    if ((ret = getaddrinfo(NULL, port, &hints, &addr_list)) != 0) {
        printf("Failed to get address info: %s\n", gai_strerror(ret));
    }

    /* Attempt to bind to each address from the list. If bind successful, leave loop. */
    for (addr = addr_list; addr != NULL; addr = addr->ai_next) {
        /* Create socket using addrinfo. */
        if ((sock_fd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol)) != -1) {
            /* Bind using opened socket. */
            if (bind(sock_fd, addr->ai_addr, addr->ai_addrlen) == 0) {
                break; /* Bind succeded. */
            }

            close(sock_fd); /* Bind failed for this address, so close. */   
        }
    }

    if (addr == NULL) { /* No bind successful. */
        perror("Could not bind");
        exit(EXIT_FAILURE);
    }

    /* Start listnening. */
    if (listen(sock_fd, LISTEN_BACKLOG) == -1) {
        perror("Error listening");
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(addr_list); /* Free dynamically allocated memory from getaddrinfo(). */

    return sock_fd;
}
