
# compilation
CC = gcc
CFLAGS = -Wall -Wextra -pedantic
GDBFLAGS = -g $(CFLAGS)
GCOVFLAGS = -O0 --coverage
CUNIT_LINK = -lcunit
BINS = test_io test_main

# lists
IO_TEST = db_io.o db_io_tests.o utils.o
MAIN_TEST = db_io.o db_main.o utils.o db_backend_mockup.o

# rules
all: *.o


utils.o: ./libs/utils/utils.o

%.o:  %.c
	$(CC) $(CFLAGS) $^ -c

.PHONY: clean

clean:
	@rm -fv *.o $(BINS)

	
test_io: $(IO_TEST)
	$(CC) $(CFLAGS) $^ -o test_io $(CUNIT_LINK)
	# ./test_io < test_input.txt

test_main: $(MAIN_TEST)
	$(CC) $(CFLAGS) $^ -o test_main $(CUNIT_LINK)
	# ./test_main