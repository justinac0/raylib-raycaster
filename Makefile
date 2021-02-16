CC=clang
CFLAGS=-W -Wextra -g -lm
LDFLAGS=-lraylib

all: main.c
	${CC} ${CFLAGS} main.c -o raycaster ${LDFLAGS}

run:
	./raycaster
