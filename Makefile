
all:
	gcc -std=c99 -D_POSIX_SOURCE -pthread src/server.c src/hangman.c src/leaderboard.c -I./include -o bin/server
	gcc -std=c99 -D_POSIX_SOURCE -pthread src/client.c -I./include -o bin/client

clientgdb:
	gcc -g -std=c99 -D_POSIX_SOURCE -pthread src/client.c -I./include -o bin/client

servergdb:
	gcc -g -std=c99 -D_POSIX_SOURCE -pthread src/server.c src/hangman.c src/leaderboard.c -I./include -o bin/server

clean:
	rm bin/server
	rm bin/client
