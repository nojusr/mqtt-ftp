CFLAGS:=-Wall -Wextra -g
LDFLAGS:=-lconfig -lmosquitto -ljson-c -lpthread

PREFIX:=/usr/local

OUTPUTDIR=./bin/

SRC:=$(wildcard src/*.c)
OBJ:=$(SRC:.c=.o)
BIN:=$(OUTPUTDIR)mqtt-ftp


.PHONY: all clean

all: $(BIN)

$(BIN): $(OBJ)
	mkdir -p $(OUTPUTDIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)


clean:
	$(RM) $(BIN) $(OBJ)
