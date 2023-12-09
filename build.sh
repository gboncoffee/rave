#!/usr/bin/sh

set -xe

CLIBS="libraylib.a -lGL -lm -lpthread -ldl -lrt -lX11"
CFLAGS="-Wall -Wextra -pedantic -std=c99"

cc rave.c $CLIBS $CFLAGS -o rave
