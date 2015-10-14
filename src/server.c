/*
 *  CAB403 Assignment - Semester 2 2015
 *
 *  Created by Lachlan Cesca & Shaun Karran
 */

/* ---- Includes ---- */
#include "server.h"

/* ---- Global Variables ---- */
static int              passive_sock_fd;            /* Initial socket descriptor */
static Leaderboard      *leaderboard;
static Client_Info      global_clients[MAX_CLIENTS];
static int              num_clients_connected = 0;  /* Number of clients currently connected to server. */
static bool             server_running;
static pthread_mutex_t  leaderboard_mutex;

/* ---- Function Definitions ---- */
int main(int argc, char *argv[])
{
    addrinfo    addr;               /* Contains internet address information of server */
    char        port[PORT_LENGTH];
    int         temp_sock_fd;

    /* Check the user provided the correct arguments. If no port provided, use default. */
    if (argc < 2) {
        printf("No port provided. Using default port %s.\n", DEFAULT_PORT);
        strcpy(port, DEFAULT_PORT);
    } else if (argc == 2) { /* FIX THIS! */
        strcpy(port, argv[1]);
    } else {
        printf("Usage: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    signal(SIGINT, shutdown_server); /* Tell the program which function to call when ctrl + c is pressed. */

    leaderboard = create_leaderboard();
    pthread_mutex_init(&leaderboard_mutex, NULL);

    passive_sock_fd = create_passive_socket(port, &addr);

    server_running = true;

    /* Main server loop. Accept incomming connections, send/recv data, close connection. */
    while (server_running) {
        /* Accept a new connection, returns a new socket_descriptor, leaving original in listening state.
         * Accept is a blocking function and will wait for a connection if none available.
         */
        printf("\nWaiting for connection...\n");
        temp_sock_fd = accept(passive_sock_fd, addr.ai_addr, &addr.ai_addrlen);
        if (temp_sock_fd == -1) {
            perror("accept");
            continue; /* Failed to accept connection, continue to start on main loop. */
        }
        printf("Connection accepted.\n\n");

        global_clients[num_clients_connected].sock_fd = temp_sock_fd;
        global_clients[num_clients_connected].connected = true;

        if (pthread_create(&global_clients[num_clients_connected].pthread_id, NULL, handle_client,
                           (void *) &global_clients[num_clients_connected]) != 0) {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }

        num_clients_connected++;
    }

    /* Close socket. */
    close(passive_sock_fd);

    exit(EXIT_SUCCESS);
}

void* handle_client(void *client_Info)
{
    bool            authenticated = false;
    int             menu_selection;
    bool            win;
    Client_Info     *client;

    client = (Client_Info *) client_Info;

    /* Send welcome message. */
    printf("Sending welcome message to client on socket %d...\n", client->sock_fd);
    write_to_client(client->sock_fd, WELCOME_MESSAGE);

    while (server_running && client->connected) {
        if (!authenticate_client(client)) {
            printf("Sending auth failed message to client on socket %d...\n", client->sock_fd);
            write_to_client(client->sock_fd, AUTH_FAILED);
            client->connected = false;
            break;
        }

        printf("Sending main menu to client on socket %d...\n", client->sock_fd);
        write_to_client(client->sock_fd, MAIN_MENU);

        menu_selection = get_menu_selection(client);

        switch (menu_selection) {
            case PLAY_HANGMAN:;
                win = play_hangman(client);

                pthread_mutex_lock(&leaderboard_mutex);
                update_score(leaderboard, client->username, win);
                pthread_mutex_unlock(&leaderboard_mutex);
                break;
            case SHOW_LEADERBOARD:
                send_leaderboard(leaderboard, client);
                break;
            case QUIT:
                client->connected = false;
                break;
        }
    }

    disconnect_client(client);

    pthread_exit(NULL);
}

bool play_hangman(Client_Info *client) {
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
        printf("Sending game interface to client on socket %d...\n", client->sock_fd);
        write_to_client(client->sock_fd, game_interface);

        if (check_complete(&game)) { /* Win */
            win = true;
            sprintf(game_over_message,
                    "\n\nGame Over\nWell done %s! You won this round of Hangman!", client->username);

            printf("Sending win message to client on socket %d...\n", client->sock_fd);
            write_to_client(client->sock_fd, game_over_message);
            return win;
        }

        if (game.guess_count >= game.number_guesses) { /* Lose */
            sprintf(game_over_message,
                    "\n\nGame Over\nBad luck %s! You have run out of guesses. The Hangman got you!",
                    client->username);

            printf("Sending lose message to client on socket %d...\n", client->sock_fd);
            write_to_client(client->sock_fd, game_over_message);
            return win;
        }

        printf("Waiting for guess from client on socket %d...\n", client->sock_fd);
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

void send_leaderboard(Leaderboard *leaderboard, Client_Info *client) {
    char score_str[BUF_SIZE];

    for (int i = 0; i < leaderboard->num_scores; i++) {
        memset(score_str, 0, BUF_SIZE);
        score_to_string(score_str, leaderboard->entries[i]);
        write_to_client(client->sock_fd, score_str);
    }
}

void get_username(Client_Info *client)
{
    /* Prompt for username. */
    printf("Sending username prompt to client on socket %d...\n", client->sock_fd);
    write_to_client(client->sock_fd, USERNAME_PROMPT);

    printf("Waiting for username from client on socket %d...\n", client->sock_fd);
    if (read(client->sock_fd, client->username, BUF_SIZE) == -1) {
        perror("read");
        exit(EXIT_FAILURE);
    }
    printf("Username received from client on socket %d...\n", client->sock_fd);
}

void get_password(Client_Info *client)
{
    /* Prompt for password. */
    printf("Sending password prompt to client on socket %d...\n", client->sock_fd);
    write_to_client(client->sock_fd, PASSWORD_PROMPT);

    printf("Waiting for password from client on socket %d...\n", client->sock_fd);
    if (read(client->sock_fd, client->password, BUF_SIZE) == -1) {
        perror("read");
        exit(EXIT_FAILURE);
    }
    printf("Password received from client on socket %d...\n", client->sock_fd);
}

bool authenticate_client(Client_Info *client)
{
    get_username(client);
    get_password(client);

    return check_login(client->username, client->password);
}

bool check_login(char *username, char *password)
{
    FILE *file;
    char file_username[USERNAME_LENGTH];
    char file_password[PASSWORD_LENGTH];
    bool match;

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
            match = true;
            break;
        }
    }

    fclose(file);
    return match;
}

int get_menu_selection(Client_Info *client)
{
    char selection_str[BUF_SIZE];

    /* Prompt for main menu. */
    printf("Sending menu selection prompt to client on socket %d...\n", client->sock_fd);
    write_to_client(client->sock_fd, MENU_PROMPT);

    printf("Waiting for menu selection from client on socket %d...\n", client->sock_fd);
    if (read(client->sock_fd, selection_str, BUF_SIZE) == -1) {
        perror("read");
        exit(EXIT_FAILURE);
    }
    printf("Menu selection received from client on socket %d...\n", client->sock_fd);

    return atoi(selection_str); /* Convert string to int. */
}

void write_to_client(int sock_fd, const char *message)
{
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

    memset(&hints, 0, sizeof(struct addrinfo));
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

void disconnect_client(Client_Info *client)
{
    printf("Sending disconnect signal to client on socket %d...\n", client->sock_fd);
    write(client->sock_fd, DISCONNECT_SIGNAL, BUF_SIZE);
    close(client->sock_fd);

    num_clients_connected--;
}

void shutdown_server(int sig)
{
    int num_clients = num_clients_connected;

    server_running = false;

    for (int i = 0; i < num_clients; i++) {
        disconnect_client(&global_clients[i]);
    }

    for (int i = 0; i < num_clients; i++) {
        pthread_join(global_clients[i].pthread_id, NULL);
    }

    free_leaderboard(leaderboard);
    close(passive_sock_fd);

    exit(EXIT_SUCCESS);
}
