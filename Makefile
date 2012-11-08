CC = gcc
CFLAGS = -Wall -Wextra

objects = memcache.o commands.o parse.o utils.o

memcache: $(objects)
	$(CC) $(CFLAGS) -o memcache $(objects)

commands.o: memcache.h

parse.o: memcache.h

utils.o: memcache.h

.PHONY: clean
clean:
	rm -f *.o memcache
