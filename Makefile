CC = gcc
CFLAGS = -Wall -Wextra

objects = socket.o server.o commands.o parse.o utils.o

memcache: $(objects)
	$(CC) $(CFLAGS) -o memcache $(objects)

server.o: memcache.h socket.h utils.h

socket.o: memcache.h socket.h

commands.o: memcache.h

parse.o: memcache.h

utils.o: utils.h

clean:
	rm -f *.o memcache
