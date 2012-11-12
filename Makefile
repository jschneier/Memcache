CC = gcc
CFLAGS = -Wall -Wextra

objects = memcache.o commands.o parse.o utils.o
test_objects = parse.o test_parse.o tests.o

memcache: $(objects)
	$(CC) $(CFLAGS) -o memcache $(objects)

tests: $(test_objects)
	$(CC) $(CFLAGS) -g -o tests $(test_objects)

tests.o: test/tests.h test/tests.c
	$(CC) $(CFLAGS) -I inc -c test/tests.c

test_parse.o: inc/parse.h test/tests.h test/test_parse.c
	$(CC) $(CFLAGS) -I inc -c test/test_parse.c

memcache.o: inc/parse.h inc/commands.h inc/memcache.h src/memcache.c
	$(CC) $(CFLAGS) -I inc -c src/memcache.c

commands.o: src/commands.c inc/memcache.h inc/commands.h
	$(CC) $(CFLAGS) -I inc -c src/commands.c

parse.o: inc/memcache.h inc/parse.h src/parse.c
	$(CC) $(CFLAGS) -I inc -c src/parse.c

utils.o: inc/memcache.h src/utils.c
	$(CC) $(CFLAGS) -I inc -c src/utils.c

.PHONY: clean
clean:
	rm -f *.o memcache tests
