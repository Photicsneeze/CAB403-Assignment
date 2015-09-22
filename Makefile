
all:
	gcc -std=c99 -D_POSIX_SOURCE src/server.c -I./include -o bin/server
	gcc -std=c99 -D_POSIX_SOURCE src/client.c -I./include -o bin/client

clean:
	rm bin/server
	rm bin/client