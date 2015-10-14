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
    int         ret;                        /* Return value for recv() */
    addrinfo    *addr;                      /* Contains internet address information of server */
    char        recv_buf[BUF_SIZE];         /* Buffer to store data received from client */
    char        send_buf[BUF_SIZE];         /* Buffer to store data to send to the client */
    char        username[USERNAME_LENGTH];
    char        password[PASSWORD_LENGTH];
    pthread_t   client_threads[10];
    pthread_t   client_thread;
    int         num_clients_connected = 0;  /* Number of clients currently connected to server. */
    int         client_id = 0;              /* ID to give client. Increment for every connection. */
    Client_Data current_client;

    /* Check the user provided the correct arguments. If no port provided, use default. */
    if (argc < 2) {
        printf("No port provided. Using default port %s.\n", DEFAULT_PORT);
        port = DEFAULT_PORT;
    } else if (argc == 2) { /* FIX THIS! */
        port = argv[1];
    } else {
        printf("Usage: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    signal(SIGINT, shutdown_server); /* Tell the program which function to call when ctrl + c is pressed. */

    leaderboard = create_leaderboard();

    passive_sock_fd = create_passive_socket(port, addr);

    /* Main server loop. Accept incomming connections, send/recv data, close connection. */
    for (;;) {
        /* Accept a new connection, returns a new socket_descriptor, leaving original in listening state.
         * Accept is a blocking function and will wait for a connection if none available.
         */
        printf("\nWaiting for connection...\n");
        global_client.sock_fd = accept(passive_sock_fd, addr->ai_addr, &addr->ai_addrlen);
        if (global_client.sock_fd == -1) {
            perror("accept");
            continue; /* Failed to accept connection, continue to start on main loop. */
        }
        printf("Connection accepted.\n\n");

        global_client.id = ++client_id;
        global_client.connected = true;

        if (pthread_create(&client_thread, NULL, handle_client, (void *) &global_client) != 0) {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }

        num_clients_connected++;

        authenticate_user(username, password);
    }

    /* Close socket. */
    close(passive_sock_fd);

    exit(EXIT_SUCCESS);
}

void* handle_client(void *client_data)
{
    // char            username[10];
    // char            password[6];
    int             menu_selection;
    bool            win;
    Client_Data     *client;

    client = (Client_Data *) client_data;

    /* Send welcome message. */
    printf("Sending welcome message to client %d...\n", client->id);
    write_to_client(client->sock_fd, WELCOME_MESSAGE);

    get_username(client);
    get_password(client);
    //strcpy(client->username, "Maolin");
    //strcpy(client->password, "111111");

    if (!authenticate_user(client->username, client->password)) {
        printf("Sending thread auth failed message to client %d...\n", client->id);
        write_to_client(client->sock_fd, AUTH_FAILED);

        disconnect_client(client->sock_fd);
        client->connected = false;
    }

    while (client->connected) {
        printf("Sending main menu to client %d...\n", client->id);
        write_to_client(client->sock_fd, MAIN_MENU);

        menu_selection = get_menu_selection(client);

        switch (menu_selection) {
            case PLAY_HANGMAN:;
                win = play_hangman(client);
                update_score(leaderboard, client->username, win);
                break;
            case SHOW_LEADERBOARD:
                send_leaderboard(leaderboard, client);
                break;
            case QUIT:
                disconnect_client(client->sock_fd);
                client->connected = false;
                break;
        }
    }

    pthread_exit(NULL);
}

bool play_hangman(Client_Data *client) {
    Game game;
    char guess[2];                  /* Not sure why this had to be 2 char array. Breaks as a char. */
    char game_interface[BUF_SIZE];
    char game_over_message[BUF_SIZE];
    bool win = false;

    choose_words(&game, get_number_words_available());
    number_of_guesses(&game);

    for (;;) {
        memset(game_interface, 0, sizeof(game_interface)); /* Clear the interface from previous round. */
        display_game(&game, game_interface);
        printf("Sending game interface...\n");
        write_to_client(client->sock_fd, game_interface);

        if (check_complete(&game)) { /* Win */
            win = true;
            sprintf(game_over_message,
                    "\n\nGame Over\nWell done %s! You won this round of Hangman!", client->username);

            printf("Sending win message...\n");
            write_to_client(client->sock_fd, game_over_message);
            return win;
        }

        if (game.guess_count >= game.number_guesses) { /* Lose */
            sprintf(game_over_message,
                    "\n\nGame Over\nBad luck %s! You have run out of guesses. The Hangman got you!",
                    client->username);

            printf("Sending lose message...\n");
            write_to_client(client->sock_fd, game_over_message);
            return win;
        }

        printf("Waiting for guess from client...\n");
        /* Have to receive 2 bytes otherwise it seems to read the enter key character on next loop. */
        if (read(client->sock_fd, guess, BUF_SIZE) == -1) {
            perror("read");
            exit(EXIT_FAILURE);
        }

        update_guess(&game, guess[0]);

        game.guesses_made[game.guess_count] = guess[0];
        game.guess_count++;
    }
}

void send_leaderboard(Leaderboard *leaderboard, Client_Data *client) {
    char score_str[BUF_SIZE];

    for (int i = 0; i < leaderboard->num_scores; i++) {
        memset(score_str, 0, BUF_SIZE);
        score_to_string(score_str, leaderboard->entries[i]);
        write_to_client(client->sock_fd, score_str);
    }
}

void get_username(Client_Data *client)
{
    /* Prompt for username. */
    printf("Sending username prompt...\n");
    write_to_client(client->sock_fd, USERNAME_PROMPT);

    printf("Waiting for username from client...\n");
    if (read(client->sock_fd, client->username, BUF_SIZE) == -1) {
        perror("read");
        exit(EXIT_FAILURE);
    }
    printf("Username received.\n");
}

void get_password(Client_Data *client)
{
    /* Prompt for password. */
    printf("Sending password prompt...\n");
    write_to_client(client->sock_fd, PASSWORD_PROMPT);

    printf("Waiting for password from client...\n");
    if (read(client->sock_fd, client->password, BUF_SIZE) == -1) {
        perror("read");
        exit(EXIT_FAILURE);
    }
    printf("Password received.\n");
}

bool authenticate_user(char *username, char *password)
{
    FILE *file;
    char file_username[USERNAME_LENGTH];
    char file_password[PASSWORD_LENGTH];

    /* !!! REMOVE THE bin/ FROM FILE NAME BEFOER SUBMISSION. Only there for convenience while testing. */
    if ((file = fopen("bin/Authentication.txt", "r")) == NULL) {
        perror("fopen");
        return false;
    }

    while (fgetc(file) != '\n'); /* Move past the column titles in the file. */

    /* Each line in the file contains the username and password separated by a space. */
    while (fscanf(file, "%s %s\n", file_username, file_password) > 0) {
        /* Check if the username matches one in the file, and if the password matches for that username. */
        if (strcmp(file_username, username) == 0 && strcmp(file_password, password) == 0) {
            return true;
        }
    }

    return false; /* No matches found. */
}

int get_menu_selection(Client_Data *client)
{
    char selection_str[BUF_SIZE];

    /* Prompt for main menu. */
    printf("Sending menu selection prompt...\n");
    write_to_client(client->sock_fd, MENU_PROMPT);

    printf("Waiting for menu selection from client...\n");
    if (read(client->sock_fd, selection_str, BUF_SIZE) == -1) {
        perror("read");
        exit(EXIT_FAILURE);
    }
    printf("Menu selection received.\n");

    return atoi(selection_str); /* Convert string to int. */
}

void write_to_client(int sock_fd, const char *message)
{
    // if (write(sock_fd, message, strlen(message)) == -1) {
    if (write(sock_fd, message, BUF_SIZE) == -1) {
        perror("write");
        exit(EXIT_FAILURE);
    }
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

void disconnect_client(int sock_fd)
{
    printf("Sending disconnect signal...\n");
    write(sock_fd, DISCONNECT_SIGNAL, BUF_SIZE);

    close(sock_fd);
}

void shutdown_server(int sig)
{
    disconnect_client(global_client.sock_fd);
    free_leaderboard(leaderboard);
    close(passive_sock_fd);

    exit(EXIT_SUCCESS);
}
