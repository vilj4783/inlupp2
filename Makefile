# compilation
CC = gcc
CFLAGS = -Wall -Wextra -pedantic -g
GDBFLAGS = -g $(CFLAGS)
GCOVFLAGS = -O0 --coverage
CUNIT_LINK = -lcunit
BINS = test_io test_main test_backend

# lists
IO_TEST = db_io.o db_io_tests.o utils.o
MAIN_TEST = db_io.o db_main.o utils.o db_backend.o linked_list.o linked_list_iterator.o hash_table.o hash_table_iterator.o
BACKEND_TEST = db_backend.o  db_backend_tests.o linked_list.o linked_list_iterator.o hash_table.o hash_table_iterator.o

# rules
all: $(BINS)

test_all: $(BINS)

# special rules
utils.o: ./libs/utils/utils.o
hash_table.o: ./libs/inlupp1/hash_table.o
hash_table_iterator.o: ./libs/inlupp1/hash_table_iterator.o
linked_list.o: ./libs/inlupp1/linked_list.o
linked_list_iterator.o: ./libs/inlupp1/linked_list_iterator.o


# normal rules
%.o:  %.c
	$(CC) $(CFLAGS) $^ -c

.PHONY: clean test_all test_backend

clean:
	@rm -fv *.o ../inlupp1/*.o $(BINS)

test_io: $(IO_TEST)
	$(CC) $(CFLAGS) $^ -o test_io $(CUNIT_LINK)
	# ./test_io < test_input.txt

test_main: $(MAIN_TEST)
	$(CC) $(CFLAGS) $^ -o test_main $(CUNIT_LINK)
	# ./test_main

test_backend: $(BACKEND_TEST)
	$(CC) $(CFLAGS) $^ -o test_backend $(CUNIT_LINK)
	# && valgrind --leak-check=full ./test_backend