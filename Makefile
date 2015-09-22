
all:
	gcc src/server.c -I./include -o bin/server
	gcc src/client.c -I./include -o bin/client
