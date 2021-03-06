CC = gcc
CFLAGS = -Wall -Wextra -Isrc

objects = memcache.o commands.o parse.o utils.o
test_objects = parse.o test_parse.o tests.o

memcache: $(objects)
	$(CC) $(CFLAGS) -o memcache $(objects)

tests: $(test_objects)
	$(CC) $(CFLAGS) -g -o tests $(test_objects)

tests.o: test/tests.h test/tests.c
	$(CC) $(CFLAGS) -c test/tests.c

test_parse.o: src/parse.h test/tests.h test/test_parse.c
	$(CC) $(CFLAGS) -c test/test_parse.c

test_commands.o: src/commands.h test/tests.h test/test_commands.c
	$(CC) $(CFLAGS) -c test/test_commands.c

memcache.o: src/parse.h src/commands.h src/memcache.h src/memcache.c
	$(CC) $(CFLAGS) -c src/memcache.c

commands.o: src/commands.c src/memcache.h src/commands.h
	$(CC) $(CFLAGS) -c src/commands.c

parse.o: src/memcache.h src/parse.h src/parse.c
	$(CC) $(CFLAGS) -c src/parse.c

utils.o: src/memcache.h src/utils.c
	$(CC) $(CFLAGS) -c src/utils.c

.PHONY: clean
clean:
	rm -f *.o memcache tests
