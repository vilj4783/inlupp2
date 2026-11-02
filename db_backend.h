#pragma once
#include "./libs/inlupp1/linked_list.h"
#include "./libs/inlupp1/hash_table.h"

/*
merch
- Goods sold in this web store that may or may not be in stock.
- An example merch is
- “Wouldn’t it be Nice” (name)
- which is “a book by Brian Wilson and Todd Gould” (description)
- which costs 100 SEK (price – quite a bargain).
Stock
- Inventory for a specific merch
- e.g. we might have 1000 copies of “Wouldn’t it be Nice”
- that we hope might get into the hands of eager buyers.
- These 1000 copies might be spread across different storage locations.
Storage Location
- A place that holds at least 1 copy of some merch.
- For example, the 1000 copies of “Wouldn’t it be Nice” could be stored at storage locations A25, B17 and C04.
- We do not keep track of empty storage locations. We also do not support mixing the items stored on a single location.
Be in stock
    A merch is considered in stock if there is at least one storage location that holds at least 1 copy of it.
Shopping cart
- A “list” of merch that a customer is interested in with an associated quantity for each.
- For example, a shopping cart can be empty, or hold 29 copies of “Wouldn’t it be Nice”.

*/

typedef struct database database_t;

typedef struct merch merch_t;

typedef struct shelf shelf_t;

struct shelf 
{
    char *shelf_name;
    int quantity;
};

database_t *create_database(void);

/*
This adds a new merch to the warehouse with a name (string), description (string), and price (integer).
A newly added merch is not in stock.
Adding a new merch with the same name as an existing merch is not allowed.
The action code shall be "A". The action shall read the name, description and price for the merch.
*/
bool backend_add_merch_execute(char *name, char *desc, int price, database_t *database);

/*
This should list all items in the store. Items should preferably (soft requirement) be printed in alphabetical order on their names. Because there may be more things in the database than might fit on a screen, items should be printed 20 at a time, and the user is asked to continue listing (if possible) or return to the main menu.
The action code shall be "L". The question to continue listing shall return to the main menu if the input begins with "N" or "n". For all other input (including an empty line) the listing shall continue.
*/
ioopm_list_t *backend_list_merch_execute(database_t *database);

/*
Removes an item completely from the warehouse, including all its stock.
The action code shall be "D". The action shall read the name of the merch and a confirmation string. The merch shall be removed only if the confirmation string begins with "Y" or "y".
*/
bool backend_remove_merch_execute(char *name, database_t *database);

/*
Allows changing the name, description and price of a merch. Note that this does not affect its stock.
Changing the name of a merch to the name of an existing merch is not allowed..
Note that changing the name may mean changing the key unless you use a unique id for each merch.
The action code shall be "E". The action shall read the old name of the merch, the new name, the new description, the new price for the merch and a confirmation string. The merch shall be updated only if the confirmation string begins with "Y" or "y".
*/
bool backend_edit_merch_execute(database_t *database, char *old_name, char *new_name, char *new_desc, int new_price);

/*
List all the storage locations for a particular merch, along with the quantities stored on each location. Storage locations should preferably be listed in alphabetical order (e.g., A20 before B01 and C01 before C10).
Names of storage locations follow this format always: one capital letter (A-Z) followed by two digits (0-9).
The action code shall be "S. The action shall read the name of the merch.
*/
ioopm_list_t *backend_show_stock_execute(database_t *database, char *name);

/*
Increases the stock of a merch by at least one.
You can replenish on an existing storage location or a new one.
The stock for a merch is the sum of all items on all storage locations holding that merch.
A storage location stocks items of one (type of) merch, never more.
For simplicity, there is no limit to the amount of storage locations nor is there a limit on the number of items a location can hold.
The action code shall be "P". The action shall read the id of the storage location, the name of the merch and the number of items to add.
*/
bool backend_replenish_execute(database_t *database, char *merch, char *shelf, int add);

/*
Creates a new shopping cart in the system which is empty.
A shopping cart represents a possible order.
Adding/removing merch to/from a cart does not change the stock for that merch – stocks are changed only during checkout.
Shopping carts are identified by a monotonically increasing number, i.e., the number of the i’th shopping cart created is i, regardless of how many shopping carts have been removed.
The action code shall be "C".
*/
int backend_create_cart_execute(database_t *db);


/*
Removes a shopping cart from the system.
The action code shall be "R". The action shall read the id of the cart and a confirmation string. The cart shall be removed only if the confirmation string begins with "Y" or "y".
*/
bool backend_remove_cart_execute(size_t id, database_t *db);

/*
Adds some quantity of a merch to a specific shopping cart.
All possible orders in the system must be fullfillable. For example, we may only have one or more carts with 12 items of a merch X if the total stock of X in the system at least 12. Thus, if all users go to checkout at the same time, they should all succeed.
The action code shall be "+". The action shall read the id of the cart, the name of the merch and the number of items.
*/
bool backend_add_to_cart_execute(size_t id, char *name, int item_amount, database_t *db);

/*
Removes zero or more items of some merch from a particular cart.
The action code shall be "-". The action shall read the id of the cart, the name of the merch and the number of items.
*/
bool backend_remove_from_cart_execute(size_t id, char *name, int item_amount, database_t *db);

/*
Calculate the cost of a shopping cart. If a cart holds 2 items of a merch M1 with a price of 50 and 8 items of a merch M2 with a price of 3 the cost of the cart is 2×50+8×3=124
The action code shall be "=". The action shall read the id of the cart.
*/
int backend_calculate_cost_execute(size_t id, database_t *db);

/*
Undos an action.
Multiple undos should be supported (i.e., pressing undo N times undos the N last actions for N≤16.)
You cannot “undo an undo” (aka “redo”).
The action code shall be "U". 
*/
bool backend_undo_execute(database_t *db);

/*
Quits the program.
The action code shall be "Q".
The action shall read a confirmation string. The program shall exit only if the confirmation string begins with "Y" or "y"
*/
void backend_quit_execute(database_t *database);

/*
The program should save its data on disk between runs. There is a discussion about how to implement this further down in this document.
*/
void backend_persistence_on_file(database_t *db);


bool backend_checkout_execute(size_t id, database_t *db);

char *get_merch_name(merch_t *merch);
char *get_merch_desc(merch_t *merch);
int get_merch_price(merch_t *merch);

char *get_location_name(location_t *location);
int get_location_quantity(location_t *location);