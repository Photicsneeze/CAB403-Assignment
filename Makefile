CC = gcc

SRC = ./src/*.c
INC = ./include

OUTPUT = bin/

all:
	$(CC) $(SRC) -I$(INC) -o $(OUTPUT)
