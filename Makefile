CC = gcc
EXEC ?= main

CFLAGS := -Wall -Wextra -lcurses -O2

source := source/*.c

$(EXEC): $(source)
	$(CC) $(source) $(CFLAGS) -o $(EXEC)
