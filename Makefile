UNAME := $(shell uname)
CC = gcc
CFLAGS = -Wall -std=c11 -g
LDFLAGS= -L.

INC = include/
SRC = src/
BIN = bin/
PARSER_SRC_FILES = $(wildcard src/VC*.c)
PARSER_OBJ_FILES = $(patsubst src/VC%.c,bin/VC%.o,$(PARSER_SRC_FILES))

parser: $(BIN)libvcparser.so

$(BIN)libvcparser.so: $(PARSER_OBJ_FILES) $(BIN)LinkedListAPI.o
	gcc -shared -o $(BIN)libvcparser.so $(PARSER_OBJ_FILES) $(BIN)LinkedListAPI.o -lm

#Compiles all files named VC*.c in src/ into object files, places all coresponding VC*.o files in bin/
$(BIN)VC%.o: $(SRC)VC%.c $(INC)LinkedListAPI.h $(INC)VC*.h
	gcc $(CFLAGS) -I$(INC) -c -fpic $< -o $@
	
$(BIN)StructListDemo: $(BIN)StructListDemo.o $(BIN)liblist.so
ifeq ($(UNAME), Linux)
	$(CC) $(CFLAGS) $(LDFLAGS) -L$(BIN) -o $(BIN)StructListDemo $(BIN)StructListDemo.o  -llist
endif
ifeq ($(UNAME), Darwin)
	$(CC) $(CFLAGS) $(LDFLAGS) -L$(BIN) -o $(BIN)StructListDemo $(BIN)StructListDemo.o  -llist; \
	install_name_tool -change $(BIN)liblist.so liblist.so $(BIN)StructListDemo; \
	install_name_tool -id liblist.so $(BIN)liblist.so
endif
	
$(BIN)StructListDemo.o: $(SRC)StructListDemo.c
	$(CC) $(CFLAGS) -I$(INC) -c $(SRC)StructListDemo.c -o $(BIN)StructListDemo.o
	
$(BIN)liblist.so: $(BIN)LinkedListAPI.o
	$(CC) -shared -o $(BIN)liblist.so $(BIN)LinkedListAPI.o

$(BIN)LinkedListAPI.o: $(SRC)LinkedListAPI.c $(INC)LinkedListAPI.h
	$(CC) $(CFLAGS) -c -fpic -I$(INC) $(SRC)LinkedListAPI.c -o $(BIN)LinkedListAPI.o

clean:
	rm -rf $(BIN)StructListDemo $(BIN)xmlExample $(BIN)*.o $(BIN)*.so
