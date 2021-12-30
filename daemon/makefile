shared_mem = ../shared_mem/
cmd = ../cmd/
CC = gcc

CPPFLAGS = -D_XOPEN_SOURCE -D_FORTIFY_SOURCE=2

CFLAGS = -std=c18 -Wpedantic -Wall -Wextra -Wconversion \
         -Wwrite-strings -Werror -Wfatal-errors \
          -fstack-protector-all -fpie -O2 -g \
					-I$(shared_mem) -I$(cmd)
LDFLAGS = -lrt -pthread -lm
VPATH =$(shared_mem)
objects = $(shared_mem)shared_fifo.o $(shared_mem)client_resources.o daemon.o \
					$(cmd)cmd.o

executable = daemon

all: $(executable)

clean:
	$(RM) $(objects) $(executable)

$(executable): $(objects)
	$(CC) $(objects) $(LDFLAGS) -o $(executable)

client_resources.o: client_resources.c client_resources.h
shared_fifo.o: shared_fifo.c shared_fifo.h
cmd.o: cmd.c cmd.h
daemon.o: daemon.c shared_fifo.h
