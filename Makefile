
all:
	gcc -std=c99 -D_POSIX_SOURCE src/server.c -I./include -o bin/server
	gcc -std=c99 -D_POSIX_SOURCE src/client.c -I./include -o bin/client

clientgdb:
	gcc -g -std=c99 -D_POSIX_SOURCE src/client.c -I./include -o bin/client

servergbd:
	gcc -g -std=c99 -D_POSIX_SOURCE src/server.c -I./include -o bin/server

clean:
	rm bin/server
	rm bin/client
