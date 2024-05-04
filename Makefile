CC = clang
CFLAGS = -Wall -Wextra -pedantic -std=c99
CLIBS = -lX11 -lGL -lm -lpthread -ldl
RAYLIB = /usr/local/lib/libraylib.a

rave: rave.c
	$(CC) $(CFLAGS) $^ $(RAYLIB) $(CLIBS) -o $@
