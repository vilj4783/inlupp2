
# compilation
CC = gcc
CFLAGS = -Wall -Wextra -pedantic
GDBFLAGS = -g $(CFLAGS)
GCOVFLAGS = -O0 --coverage
CUNIT_LINK = -lcunit

# lists


# rules
all: *.o


# db:
# 	$(CC) $(CFLAGS) $^ -o db

%.o:  %.c
	$(CC) $(CFLAGS) $^ -c

.PHONY: clean

clean:
	@rm -fv *.o
