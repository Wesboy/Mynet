DIR_INC = ./include
DIR_SRC = ./src
DIR_OBJ = ./obj
DIR_BIN = ./bin
DIR_CLT:= $(DIR_SRC)/client
DIR_SER:= $(DIR_SRC)/service
DIR_TEST:= $(DIR_SRC)/test

CC = gcc


main : $(DIR_CLT)/client.c $(DIR_SER)/service.c
	$(CC) $(DIR_CLT)/client.c -o $(DIR_BIN)/client
	$(CC) $(DIR_SER)/service.c -o $(DIR_BIN)/service -lpthread
	$(CC) $(DIR_TEST)/more_client.c -o $(DIR_BIN)/test -lpthread

clean :
	rm $(DIR_BIN)/*
