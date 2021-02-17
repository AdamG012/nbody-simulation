CC=gcc
CFLAGS=-g -std=c11 -Wall -Werror -lm
TARGET=program
.PHONY: clean
all: $(TARGET)

nbody: src/nbody.c
	$(CC) $(CFLAGS) $^ -o $@ -lpthread

nbody-gui: src/nbodygui.c
	$(CC) $(CFLAGS) $^ -o $@ -lpthread -lSDL2 -lSDL2_gfx

test: nbodytest.c
	$(CC) $(CFLAGS) $^ -o $@ -lpthread -lcmocka

test_functions: test/test_functions.c
	$(CC) $(CFLAGS) $^ -o $@ -lpthread -lcunit

clean:
	rm -f *.o
	rm -f nbody-gui
	rm -f nbody
	rm -f test_functions
