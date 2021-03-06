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
    pthread_t   read_thread, write_thread;

    /* Check the user provided the correct arguments. */
    if (argc != 3) {
        printf("Usage: %s <hostname> <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    signal(SIGINT, shutdown_client); /* Tell the program which function to call when ctrl + c is pressed. */

    /* Function to create socket and connect to the server. */
    sock_fd = create_connection(argv[1], argv[2]);

    if (pthread_create(&write_thread, NULL, write_socket, (void *) &sock_fd) != 0) {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }

    if (pthread_create(&read_thread, NULL, read_socket, (void *) &sock_fd) != 0) {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }

    if (pthread_join(write_thread, NULL) != 0) {
        perror("pthread_join");
        exit(EXIT_FAILURE);
    }

    if (pthread_join(read_thread, NULL) != 0) {
        perror("pthread_join");
        exit(EXIT_FAILURE);
    }

    /* Close socket. */
    close(sock_fd);

    exit(EXIT_SUCCESS);
}

static void *write_socket(void *data)
{
    int     *sock_fd;
    char    send_buf[BUF_SIZE];
    int     input_len;

    sock_fd = (int *) data;

    while (!quit) {
        input_len = get_input("", send_buf);

        if (write(*sock_fd, send_buf, input_len) != input_len) {
            perror("write");
            exit(EXIT_FAILURE);
        }
    }

    pthread_exit(NULL);
}

static void *read_socket(void *data)
{
    int     *sock_fd;
    char    recv_buf[BUF_SIZE];

    sock_fd = (int *) data;

    while (!quit) {
        if (read(*sock_fd, recv_buf, BUF_SIZE) == -1) {
            perror("read");
            exit(EXIT_FAILURE);
        }

        if (strcmp(recv_buf, DISCONNECT_SIGNAL) == 0) {
            printf("\nReceived disconnect signal from server. Press any key to quit.\n");
            write(*sock_fd, DISCONNECT_SIGNAL, BUF_SIZE);
            break;
        }

        printf("%s", recv_buf);
        fflush(stdout);
    }

    pthread_exit(NULL);
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
        exit(EXIT_FAILURE);
    }

    /* Attempt to conenct to each address from the list. If connection made, leave loop. */
    for (addr = addr_list; addr != NULL; addr = addr->ai_next) {
        /* Create socket using addrinfo. */
        if ((sock_fd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol)) != -1) {
            /* Make connection using opened socket. */
            if (connect(sock_fd, addr->ai_addr, addr->ai_addrlen) == 0) {
                break; /* Connection succeded. */
            }

            close(sock_fd); /* Connection failed for this address, so close. */
        }
    }

    if (addr == NULL) { /* No connections successful. */
        perror("connect");
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(addr_list); /* Free dynamically allocated memory from getaddrinfo(). */

    return sock_fd;
}

int get_input(char *msg, char *input_str)
{
    int input_len;
    char *new_line;

    printf("%s", msg);
    fgets(input_str, BUF_SIZE, stdin);

    /* Replace the new line inserted by pressing the enter key with end of line. */
    if ((new_line = strchr(input_str, '\n')) != NULL) {
        *new_line = '\0';
    } else {
        /* If input is longer than BUF_SIZE, getchar to clear input stream */
        while (getchar() != '\n') { ; }
    }

    input_len = strlen(input_str) + 1;

    return input_len;
}

void shutdown_client(int sig)
{
    exit(EXIT_SUCCESS);
}
