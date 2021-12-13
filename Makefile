CC = gcc
CPPFLAGS = -D_XOPEN_SOURCE=700 -D_FORTIFY_SOURCE=2
CFLAGS = -std=c18 -Wpedantic -Wall -Wextra -Wconversion -Wwrite-strings -Werror -fstack-protector-all -fpie -O2 -g
LDFLAGS =  -fsanitize=address -lrt -pthread

EXEC = main
OBJ = $(EXEC:=.o)

all: $(EXEC)

main.o = main.c

main: main.o
	$(CC) main.o $(LDFLAGS) $(LDLIBS) -o $@

clean:
	rm -rf $(OBJ) $(EXEC)
