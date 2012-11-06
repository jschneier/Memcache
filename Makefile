CC = gcc
CFLAGS = -Wall -Wextra

objects = socket.o server.o commands.o

memcache: $(objects)
	$(CC) $(CFLAGS) -o memcache $(objects)

server.o: memcache.h socket.h

socket.o: memcache.h socket.h

commands.c: memcache.h

clean:
	rm -f *.o memcache
