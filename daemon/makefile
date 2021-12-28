shared_fifo = ../shared_fifo/
CC = gcc

CPPFLAGS = -D_XOPEN_SOURCE -D_FORTIFY_SOURCE=2

CFLAGS = -std=c18 -Wpedantic -Wall -Wextra -Wconversion \
         -Wwrite-strings -Werror -Wfatal-errors \
          -fstack-protector-all -fpie -O2 -g \
					-I$(shared_fifo)
LDFLAGS = -lrt -pthread
VPATH =$(shared_fifo)
objects = $(shared_fifo)shared_fifo.o daemon.o
executable = daemon

all: $(executable)

clean:
	$(RM) $(objects) $(executable)

$(executable): $(objects)
	$(CC) $(objects) $(LDFLAGS) -o $(executable)

shared_fifo.o: shared_fifo.c shared_fifo.h
daemon.o: daemon.c shared_fifo.h
