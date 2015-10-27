
all:
	gcc -std=c99 -D_POSIX_SOURCE -Wall -pthread src/server.c src/authentication.c src/hangman.c src/leaderboard.c -I./include -o bin/server
	gcc -std=c99 -D_POSIX_SOURCE -Wall -pthread src/client.c -I./include -o bin/client

clientgdb:
	gcc -g -std=c99 -D_POSIX_SOURCE -pthread src/client.c -I./include -o bin/client

servergdb:
	gcc -g -std=c99 -D_POSIX_SOURCE -pthread src/server.c -I./include -o bin/server

test:
	gcc -std=c99 -D_POSIX_SOURCE -pthread src/test.c src/leaderboard.c -I./include -o bin/test

clean:
	rm bin/server
	rm bin/client
