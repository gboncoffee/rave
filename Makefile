CC = clang
CFLAGS = -Wall -Wextra -pedantic -std=c99
CLIBS = -lX11 -lGL -lm -lpthread -ldl -lraylib

rave: rave.c
	$(CC) $(CFLAGS) $(CLIBS) $^ -o $@
