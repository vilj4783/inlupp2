# make rules
- `make all` : compiles all `.c` files into `.o` files
- `make %.o` : will compile the `.c` file of the same name to a `.o` file
- `make db` : compile the binary
- `make db_test` : compile test for binary
- `make db_io_test` : compile test for `db_io.c`
- `make db_backend_test` : compile test for `db_backend.c`
- `make clean` : object files and binaries in the directory
# modules
this project is primarily split into
- db_io.c : which handles user input and output
- db_backend.c : which handles the implementation of the database
- db_main.c : which runs the program
