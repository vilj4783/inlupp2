# IOOPM Assignment 2: Webstore

---

## Authors

* **Axel Byback**
* **Victor Ljungberg**

---

## 1. Building, Testing and Coverage

### Build and Run
- Build the binary: `make db`
- Run the program: `./db`

### Test Suites
- Run all test suites: `make test_all`
- Run backend tests: `make test_backend`
- Run with Valgrind: `valgrind --leak-check=full ./test_backend`

### Code Coverage
Coverage measured with `gcov -b`:

# Coverage Report
**`db_backend.c`**
* Lines executed: 94,63%
* Branches executed: 100%

**`db_io.c`**
* Lines executed: 97,75%
* Branches executed: 100%

**`db_main.c`**
* Lines executed: 90,98%
* Branches executed: 96,23%


## 2. Integrated Components and Attribution
Generic data structures are integrated from two separate sources according to assignment requirements:
- **Hash Table:** Comes from Victor Ljungberg.
- **Linked List:** Comes from Victor Ljungberg.
- **Utils:** Comes from Axel Byback.

## 3. Modules
this project is primarily split into:
- `db_io.c`: Handles user input and output
- `db_backend.c`: Handles the implementation of the database
- `db_main.c`: Runs the program

## 4. Make Rules
- `make all` : compiles all `.c` files into `.o` files
- `make %.o` : will compile the `.c` file of the same name to a `.o` file
- `make db` : compile the complete binary
- `make test_all` compiles all test binaries
- `make test_main` : compile a testversion of `db_main.c`
- `make test_io` : compile test for `db_io.c`
- `make test_backend` : compile test for `db_backend.c`
- `make clean` : object files and binaries in the directory
