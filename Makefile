CC ?= gcc
CFLAGS ?= -Wall -Wextra -Wno-unused-parameter `pkg-config fuse --cflags --libs`

all: dffs

dffs: dffs.c
	$(CC) -o dffs dffs.c $(CFLAGS)

clean:
	rm dffs
