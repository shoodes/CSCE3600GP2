CC = gcc
CFLAGS = -Wall -Wextra -Werror -g -Wno-error=parentheses -Wno-parentheses
LDFLAGS = -lreadline

SRC = main.c alias.c myhistory.c Path.c
OBJ = $(SRC:.c=.o)

EXEC = myshell

.PHONY: all clean

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $(EXEC) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(EXEC)
