shared_list = ../shared_list/
CC = gcc

CPPFLAGS = -D_XOPEN_SOURCE -D_FORTIFY_SOURCE=2

CFLAGS = -std=c18 -Wpedantic -Wall -Wextra -Wconversion \
         -Wwrite-strings -Werror -Wfatal-errors \
          -fstack-protector-all -fpie -O2 -g \
					-I$(shared_list)
LDFLAGS = -lrt -pthread
VPATH =$(shared_list)
objects = $(shared_list)shared_list.o client.o
executable = client

all: $(executable)

clean:
	$(RM) $(objects) $(executable)

$(executable): $(objects)
	$(CC) $(objects) $(LDFLAGS) -o $(executable)

shared_list.o: shared_list.c shared_list.h
client.o: client.c shared_list.h
