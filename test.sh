make test_parse.o
make parse.o

cc -o test_parse test_parse.o parse.o
./test_parse

rm test_parse.o parse.o
