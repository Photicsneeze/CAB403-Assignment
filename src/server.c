/*
 *  CAB403 Assignment - Semester 2 2015
 *
 *  Created by Lachlan Cesca & Shaun Karran
 */

/* ---- Includes ---- */
#include "server.h"

/* ---- Global Variables ---- */
static int              passive_sock_fd;            /* Initial socket descriptor */
static Leaderboard      *leaderboard;               /*  */
static pthread_mutex_t  leaderboard_mutex;          /*  */
static Client_Info      clients_infos[MAX_CLIENTS]; /*  */
static pthread_t        threads[MAX_CLIENTS];       /*  */
static int              sock_fds[MAX_CLIENTS];      /*  */
static bool             server_running = false;     /*  */
static sem_t            sem_client_handler;         /*  */
static sem_t            sem_client;                 /*  */
static int              next_queue_pos = 0;         /*  */
static pthread_mutex_t  next_queue_mutex;           /*  */
static int              client_to_handle = 0;       /*  */
static pthread_mutex_t  client_to_handle_mutex;     /*  */

/* ---- Function Definitions ---- */
int main(int argc, char *argv[])
{
    addrinfo    addr;               /* Contains internet address information of server */
    char        port[PORT_LENGTH];
    int         temp_sock_fd;

    server_running = true;

    signal(SIGINT, shutdown_server); /* Tell the program which function to call when ctrl + c is pressed. */

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


    /* Initialse semaphore to max number of clients. Will decrement whenever a client is handled.
     * This represents availale client handlers. Once MAX_CLIENTS connect it will == 0.
     */
    if (sem_init(&sem_client_handler, 0, MAX_CLIENTS) == -1) {
        perror("sem_init");
        exit(EXIT_FAILURE);
    }

    /* Initialse semaphore to 0. Will increment everytime a client is ready to be handled.
     * Represents clients connected that are ready to be handled by a thread. Used to made threads wait
     * until a client is available.
     */
    if (sem_init(&sem_client, 0, 0) == -1) {
        perror("sem_init");
        exit(EXIT_FAILURE);
    }

    /* Create a pool of threads to handle clients. Threads wait until a client is available to be handled. */
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (pthread_create(&threads[i], NULL, handle_client, (void *) &clients_infos[i]) != 0) {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
    }

    /* Create a leaderboard and a mutex to ensure only 1 thread can access it at a time. */
    leaderboard = create_leaderboard();
    pthread_mutex_init(&leaderboard_mutex, NULL);
    pthread_mutex_init(&next_queue_mutex, NULL);
    pthread_mutex_init(&client_to_handle_mutex, NULL);

    /* Create a socket to listen for connections. */
    passive_sock_fd = create_passive_socket(port, &addr);




    /* Main server loop. Wait for an available handler, accept a connection, add client to queue. */
    while (server_running) {
        /* Wait for an available client handler. */
        sem_wait(&sem_client_handler);

        /* Accept a new connection, returns a new socket_descriptor, leaving original in listening state.
         * Accept is a blocking function and will wait for a connection if none available.
         */
        printf("Waiting to accept connection...\n\n");
        temp_sock_fd = accept(passive_sock_fd, addr.ai_addr, &addr.ai_addrlen);
        if (temp_sock_fd == -1) {
            perror("accept");
            continue; /* Failed to accept connection, continue to start on main loop. */
        }
        printf("Connection accepted.\n\n");

        /* Add socket for connected client to queue of clients to be handled. */
        add_client_to_queue(temp_sock_fd);

        /* Let thread know there is a client to be handled. */
        sem_post(&sem_client);
    }

    /* Close socket. */
    close(passive_sock_fd);
    exit(EXIT_SUCCESS);
}

void* handle_client(void *client_Info)
{
    int             menu_selection;
    bool            win;
    Client_Info     *client;

    client = (Client_Info *) client_Info;

    client->connected = false;
    client->sock_fd = 0;

    while (server_running) {
        sem_wait(&sem_client);

        if (!server_running) { /* Tells thread to quit when server is shutdown while waiting for client. */
            break;
        }

        client->sock_fd = get_client_from_queue();
        client->connected = true;

        /* Send welcome message. */
        printf("Sending welcome message to client on socket %d...\n", client->sock_fd);
        write_to_socket(client->sock_fd, WELCOME_MESSAGE);

        if (get_username(client) == -1 || get_password(client) == -1) {
            client->connected = false;
        }

        if (!server_running) { /* Tells thread to quit when server is shutdown while waiting for client. */
            break;
        }

        while (client->connected) {/* added in server_running check so that it doesnt enter loop if server quits at users login */
            if (!authenticate_login(client->username, client->password)) {
                printf("Sending auth failed message to client on socket %d...\n", client->sock_fd);
                write_to_socket(client->sock_fd, AUTH_FAILED);
                client->connected = false;
                break;
            }

            printf("Sending main menu to client on socket %d...\n", client->sock_fd);
            write_to_socket(client->sock_fd, MAIN_MENU);

            menu_selection = get_menu_selection(client);

            if (!server_running) { /* Tells thread to quit when server is shutdown while waiting for client. */
                break;
            }

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

        if (!server_running) { /* Tells thread to quit when server is shutdown while waiting for client. */
            break;
        }

        disconnect_client(client);
        sem_post(&sem_client_handler);
    }

    //pthread_exit(NULL);
    return NULL;
}

void add_client_to_queue(int sock_fd)
{
    pthread_mutex_lock(&next_queue_mutex);
    printf("next_queue_pos: %d\n", next_queue_pos);
    sock_fds[next_queue_pos] = sock_fd;
    next_queue_pos++;
    next_queue_pos = next_queue_pos % MAX_CLIENTS;
    pthread_mutex_unlock(&next_queue_mutex);
}

int get_client_from_queue()
{
    pthread_mutex_lock(&client_to_handle_mutex);
    printf("client_to_handle: %d\n", client_to_handle);
    int client = sock_fds[client_to_handle];
    client_to_handle++;
    client_to_handle = client_to_handle % MAX_CLIENTS;
    pthread_mutex_unlock(&client_to_handle_mutex);

    return client;
}

bool play_hangman(Client_Info *client) {
    Game game;
    char guess[2];                  /* Not sure why this had to be 2 char array. Breaks as a char. */
    char game_interface[BUF_SIZE];
    char game_over_message[BUF_SIZE];
    bool win = false;

    choose_words(&game, get_number_words_available());
    number_of_guesses(&game);

    while (client->connected) {
        memset(game_interface, 0, sizeof(game_interface)); /* Clear the interface from previous round. */
        display_game(&game, game_interface);
        printf("Sending game interface to client on socket %d...\n", client->sock_fd);
        write_to_socket(client->sock_fd, game_interface);

        if (check_complete(&game)) { /* Win */
            win = true;
            sprintf(game_over_message,
                    "\n\nGame Over\nWell done %s! You won this round of Hangman!", client->username);

            printf("Sending win message to client on socket %d...\n", client->sock_fd);
            write_to_socket(client->sock_fd, game_over_message);
            return win;
        }

        if (game.guess_count >= game.number_guesses) { /* Lose */
            sprintf(game_over_message,
                    "\n\nGame Over\nBad luck %s! You have run out of guesses. The Hangman got you!",
                    client->username);

            printf("Sending lose message to client on socket %d...\n", client->sock_fd);
            write_to_socket(client->sock_fd, game_over_message);
            return win;
        }

        printf("Waiting for guess from client on socket %d...\n", client->sock_fd);
        /* Have to receive 2 bytes otherwise it seems to read the enter key character on next loop. */
        if (read_from_socket(client->sock_fd, guess) == -1) {
            client->connected = false;
            break;
        }

        update_guess(&game, guess[0]);

        game.guesses_made[game.guess_count] = guess[0];
        game.guess_count++;
    }

    return win;
}

void send_leaderboard(Leaderboard *leaderboard, Client_Info *client) {
    char score_str[BUF_SIZE];
    if(leaderboard->num_scores==0){
        strcat(score_str, "\n==============================================================================\n");
        strcat(score_str, "\nThere is no information currently stored in the Leader Board. Try again later.\n");
        strcat(score_str, "\n==============================================================================\n");
        write_to_socket(client->sock_fd, score_str);
    }else{
        for (int i = 0; i < leaderboard->num_scores; i++) {
            memset(score_str, 0, BUF_SIZE);
            score_to_string(score_str, get_score_at_index(leaderboard,i));
            write_to_socket(client->sock_fd, score_str);
        }
    }


}

int get_username(Client_Info *client)
{
    /* Prompt for username. */
    printf("Sending username prompt to client on socket %d...\n", client->sock_fd);
    write_to_socket(client->sock_fd, USERNAME_PROMPT);

    printf("Waiting for username from client on socket %d...\n", client->sock_fd);
    if (read_from_socket(client->sock_fd, client->username) == -1) {
        return -1;
    }
    printf("Username received from client on socket %d...\n", client->sock_fd);

    return 1;
}

int get_password(Client_Info *client)
{
    /* Prompt for password. */
    printf("Sending password prompt to client on socket %d...\n", client->sock_fd);
    write_to_socket(client->sock_fd, PASSWORD_PROMPT);

    printf("Waiting for password from client on socket %d...\n", client->sock_fd);
    if (read_from_socket(client->sock_fd, client->password) == -1) {
        return -1;
    }
    printf("Password received from client on socket %d...\n", client->sock_fd);

    return 1;
}

bool authenticate_login(char *username, char *password)
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
    write_to_socket(client->sock_fd, MENU_PROMPT);

    printf("Waiting for menu selection from client on socket %d...\n", client->sock_fd);
    if (read_from_socket(client->sock_fd, selection_str) == -1) {
        return QUIT;
    }
    printf("Menu selection received from client on socket %d...\n", client->sock_fd);

    return atoi(selection_str); /* Convert string to int. */
}

void write_to_socket(int sock_fd, const char *str)
{
    if (write(sock_fd, str, BUF_SIZE) == -1) {
        perror("write");
        exit(EXIT_FAILURE);
    }
}

int read_from_socket(int sock_fd, char *str)
{
    if (read(sock_fd, str, BUF_SIZE) == -1) {
        perror("read");
        exit(EXIT_FAILURE);
    }

    if (strcmp(str, DISCONNECT_SIGNAL) == 0) {
        return -1;
    }

    return 0;
}

/* Function to create socket, bind it and mark it to accept incoming connections. */
int create_passive_socket(char *port, addrinfo *addr)
{
    int         ret;            /* Return value for getaddrinfo() */
    int         sock_fd;        /* Socket descriptor to return */
    addrinfo    hints;           /* Used to set criteria for getaddrinfo() */
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
    memset(client, 0, sizeof(Client_Info)); /* Clear the client info for next client to use. */
}

void shutdown_server(int sig)
{
    server_running = false;

    for (int i = 0; i < MAX_CLIENTS; i++) {
        printf("\n");
        printf("Client[%d].sock_fd = %d\n", i, clients_infos[i].sock_fd);
        printf("Client[%d].connected = %d\n", i, clients_infos[i].connected);
        printf("Client[%d].username = %s\n", i, clients_infos[i].username);
        printf("Client[%d].password = %s\n", i, clients_infos[i].password);
    }

    for (int i = 0; i < MAX_CLIENTS; i++) {
        sem_post(&sem_client);
    }

    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients_infos[i].connected) {
            disconnect_client(&clients_infos[i]);
        }
    }

    printf("\n\n");
    for (int i = 0; i < MAX_CLIENTS; i++) {
        printf("Joining thread %d...\n", i);
        pthread_join(threads[i], NULL);
    }

    free_leaderboard(leaderboard);

    close(passive_sock_fd);
    exit(EXIT_SUCCESS);
}
