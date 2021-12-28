shared_mem = ../shared_mem/
CC = gcc

CPPFLAGS = -D_XOPEN_SOURCE -D_FORTIFY_SOURCE=2

CFLAGS = -std=c18 -Wpedantic -Wall -Wextra -Wconversion \
         -Wwrite-strings -Werror -Wfatal-errors \
          -fstack-protector-all -fpie -O2 -g \
					-I$(shared_mem)
LDFLAGS = -lrt -pthread -lm
VPATH =$(shared_mem)
objects = $(shared_mem)shared_fifo.o $(shared_mem)client_resources.o client.o
executable = client

all: $(executable)

clean:
	$(RM) $(objects) $(executable)

$(executable): $(objects)
	$(CC) $(objects) $(LDFLAGS) -o $(executable)

client_resources.o: client_resources.c client_resources.h
shared_fifo.o: shared_fifo.c shared_fifo.h
client.o: client.c shared_fifo.h
