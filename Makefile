CC = clang
CFLAGS = -g -Wall -Wextra -pedantic -std=c99
CLIBS = -lX11 -lGL -lm -lpthread -ldl

rave: rave.c
	$(CC) $(CFLAGS) `pkg-config --cflags raylib` $^ `pkg-config --libs raylib` -o $@
