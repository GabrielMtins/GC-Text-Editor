CC = gcc
EXEC ?= main

CCFLAGS := -Wall -Wextra -O2

LIBFLAGS := -lcurses

SOURCE := $(wildcard source/*.c)

$(EXEC): $(SOURCE)
	$(CC) $(SOURCE) $(CCFLAGS) $(LIBFLAGS) -o $(EXEC)

clean:
	rm main
