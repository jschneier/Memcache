CC = gcc
CFLAGS = -Wall -Wextra

objects = socket.o server.o commands.o parse.o utils.o hash.o

mem: $(objects)
	$(CC) $(CFLAGS) -o memcache $(objects)

server.o: memcache.h socket.h parse.h

socket.o: memcache.h

commands.o: memcache.h hash.h

parse.o: memcache.h

utils.o: memcache.h

clean:
	rm -f *.o
