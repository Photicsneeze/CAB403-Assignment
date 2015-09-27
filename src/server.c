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
    char        *port;
    int         ret;                    /* Return value for recv() */
    addrinfo    *addr;                  /* Contains internet address information of server */
    char        recv_buf[BUF_SIZE];     /* Buffer to store data received from client */
    char        send_buf[BUF_SIZE];     /* Buffer to store data to send to the client */
    char        username[8];
    char        password[6];
    int         menu_selection;

    /* Check the user provided the correct arguments. If no port provided, use default. */
    if (argc < 2) {
        printf("No port provided. Using default port %s.\n", DEFAULT_PORT);
        port = DEFAULT_PORT;
    } else if (argc == 2) {
        port = argv[1];
    } else {
        printf("Usage: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    signal(SIGINT, shutdown_server); /* Tell the program which function to call when ctrl + c is pressed. */

    sock_fd = create_passive_socket(port, addr);

    /* Main server loop. Accept incomming connections, send/recv data, close connection. */
    for (;;) {
        /* Accept a new connection, returns a new socket_descriptor, leaving original in listening state.
         * Accept is a blocking function and will wait for a connection if none available.
         */
        printf("\nWaiting for connection...\n");
        if ((new_sock_fd = accept(sock_fd, addr->ai_addr, &addr->ai_addrlen)) == -1) {
            perror("accept");
            continue; /* Failed to accept connection, continue to start on main loop. */
        }
        printf("Connection accepted.\n\n");

        /* Send welcome message. */
        printf("Sending welcome message...\n");
        if (write(new_sock_fd, WELCOME_MESSAGE, BUF_SIZE) == -1) {
            perror("write");
            exit(EXIT_FAILURE);
        }

        get_username(username);

        get_password(password);

        if (!authenticate(username, password)) {
            printf("Sending auth failed message...\n");
            if (write(new_sock_fd, AUTH_FAILED, BUF_SIZE) == -1) {
                perror("write");
                exit(EXIT_FAILURE);
            }

            /* Send message to client to disconnect. */
            write(new_sock_fd, DISCONNECT_SIGNAL, BUF_SIZE);

            close(new_sock_fd);
            continue;
        }

        /* Send main menu. */
        printf("Sending main menu...\n");
        if (write(new_sock_fd, MAIN_MENU, BUF_SIZE) == -1) {
            perror("write");
            exit(EXIT_FAILURE);
        }

        menu_selection = get_menu_selection();

        printf("%d\n", menu_selection);

        while (1);

        /* Receive message from new socket and store data in buffer. */
        // if ((ret = recv(new_sock_fd, recv_buf, BUF_SIZE, NO_FLAGS)) == -1) {
        //     perror("recv");
        //     exit(EXIT_FAILURE);
        // } else if (ret == 0) {
        //     printf("Connection closed.\n");
        //     break;
        // }
    }

    /* Close socket. */
    close(sock_fd);

    exit(EXIT_SUCCESS);
}

void get_username(char *username)
{
    /* Prompt for username. */
    printf("Sending username prompt...\n");
    if (write(new_sock_fd, USERNAME_PROMPT, BUF_SIZE) == -1) {
        perror("write");
        exit(EXIT_FAILURE);
    }
    printf("Waiting for username from client...\n");
    if (read(new_sock_fd, username, BUF_SIZE) == -1) {
        perror("read");
        exit(EXIT_FAILURE);
    }
    printf("Username received.\n");
}

void get_password(char *password)
{
    /* Prompt for password. */
    printf("Sending password prompt...\n");
    if (write(new_sock_fd, PASSWORD_PROMPT, BUF_SIZE) == -1) {
        perror("write");
        exit(EXIT_FAILURE);
    }
    printf("Waiting for password from client...\n");
    if (read(new_sock_fd, password, BUF_SIZE) == -1) {
        perror("read");
        exit(EXIT_FAILURE);
    }
    printf("Password received.\n");
}

int get_menu_selection()
{
    char *selection_str;

    /* Prompt for main menu. */
    printf("Sending menu selection prompt...\n");
    if (write(new_sock_fd, MENU_PROMPT, BUF_SIZE) == -1) {
        perror("write");
        exit(EXIT_FAILURE);
    }
    printf("Waiting for menu selection from client...\n");
    if (read(new_sock_fd, selection_str, BUF_SIZE) == -1) {
        perror("read");
        exit(EXIT_FAILURE);
    }
    printf("Menu selection received.\n");

    return atoi(selection_str); /* Convert string to int. */
}

/* Function to create socket, bind it and mark it to accept incoming connections. */
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
        exit(EXIT_FAILURE);
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
        perror("bind");
        exit(EXIT_FAILURE);
    }

    /* Start listnening. */
    if (listen(sock_fd, LISTEN_BACKLOG) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(addr_list); /* Free dynamically allocated memory from getaddrinfo(). */

    return sock_fd;
}

void shutdown_server(int sig)
{
    /* Send message to client to disconnect. */
    write(new_sock_fd, DISCONNECT_SIGNAL, BUF_SIZE);

    fflush(stdout);

    exit(EXIT_SUCCESS);
}
