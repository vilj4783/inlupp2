# compilation
CC = gcc
CFLAGS = -Wall -Wextra -pedantic
GDBFLAGS = -g $(CFLAGS)
GCOVFLAGS = -O0 --coverage
CUNIT_LINK = -lcunit
BINS = test_io test_main test_backend

# lists
IO_TEST = db_io.o db_io_tests.o utils.o
MAIN_TEST = db_io.o db_main.o utils.o db_backend_mockup.o
INLUPP1_OBJS = ../inlupp1/hash_table.o ../inlupp1/linked_list.o

# rules
all: $(BINS)

test_all: $(BINS)
	./test_io
	./test_main
	./test_backend

utils.o: ./libs/utils/utils.o

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

../inlupp1/%.o: ../inlupp1/%.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean test_all test_backend

clean:
	@rm -fv *.o ../inlupp1/*.o $(BINS)

test_io: $(IO_TEST)
	$(CC) $(CFLAGS) $^ -o test_io $(CUNIT_LINK)
	# ./test_io < test_input.txt

test_main: $(MAIN_TEST)
	$(CC) $(CFLAGS) $^ -o test_main $(CUNIT_LINK)
	# ./test_main