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

/// @brief Structure representing the entire webstore database
typedef struct database database_t;

/// @brief Structure representing a merchandise item in the store
typedef struct merch merch_t;

/// @brief Represents a specific storage location and the quantity of an item stored there.
typedef struct shelf shelf_t;

/// @brief Details of a storage location.
struct shelf 
{
    char *shelf_name; /// Shelf identifier
    size_t quantity; /// Number of items stored on this shelf
};

/// @brief Creates a new database
/// @return A database that holds three hashtables, for merch, shelf and cart
database_t *create_database(void);

/// @brief Adds a new merch to the database
/// @pre db != NULL
/// @pre name != NULL && desc != NULL
/// @pre price >= 0
/// @param name Name for the new merch
/// @param desc Description of the new merch
/// @param price Price of the new merch
/// @param db The database to add the new merch into
/// @return True if the new merch could be added to the database, otherwise false
bool backend_add_merch_execute(char *name, char *desc, int price, database_t *db);

/// @brief Retrieves a list of all the merchandise names currently from the database (in alphabetical order)
/// @pre db != NULL
/// @param db The database to list all the merch from
/// @return A list with all the merchandise in alphabetical order
ioopm_list_t *backend_list_merch_execute(database_t *db);

/// @brief Removes a merch completely from the database, including all its stock and shelves 
/// @pre db != NULL
/// @pre name != NULL
/// @param name The name of the merchandise to remove
/// @param db The database to remove the merch from
/// @return True if the merchandise was successfully removed, otherwise false
bool backend_remove_merch_execute(char *name, database_t *db);

/// @brief Allows to edit an existing merchandise name, price and description. Does not affect its stock
/// @pre db != NULL
/// @pre old_name != NULL && new_name != NULL && new_desc != NULL
/// @pre new_price >= 0 
/// @param db The database containing the merch
/// @param old_name Current name of the merch
/// @param new_name New name for the merch
/// @param new_desc New description for the merch
/// @param new_price The new price for the merch
/// @return True if the edit of the merchandise was successful, otherwise false
bool backend_edit_merch_execute(database_t *db, char *old_name, char *new_name, char *new_desc, int new_price);

/// @brief List all the storage locations for a particular merch along with the quantities stored 
/// on each location. Storage locations are listed in alphabetical order
/// @pre db != NULL
/// @pre name != NULL
/// @param db The database to list all the storage locations from
/// @param name The name of the merch to list all the storage locations from
/// @return A list in alphabetical order with the storage locations and the quantities stored on each location
ioopm_list_t *backend_show_stock_execute(database_t *db, char *name);

/// @brief Increases the stock of a merch by adding items to an existing or new shelf
/// @pre db != NULL
/// @pre merch != NULL && shelf != NULL
/// @pre add > 0
/// @param db The database containing the merch
/// @param merch Name of the merch to replenish
/// @param shelf The shelf location ID
/// @param add The number of items to add 
/// @return True if the stock were successfully increased, false otherwise
bool backend_replenish_execute(database_t *db, char *merch, char *shelf, size_t add);

/// @brief Creates a new shopping cart in the system which is empty
/// @pre db != NULL
/// @param db The database to add the cart into
/// @return The id of the new cart
int backend_create_cart_execute(database_t *db);

/// @brief Removes a shopping cart from the system.
/// @pre db != NULL
/// @param id The id of the cart to remove
/// @param db The database to remove the cart from
/// @return True if the cart is removed, false otherwise
bool backend_remove_cart_execute(size_t id, database_t *db);

/// @brief Adds some quantity of a merch to a specific shopping cart
/// @pre name != NULL
/// @pre db != NULL
/// @param id The id of the cart 
/// @param name The name of the merch 
/// @param item_amount The amount of merch items to add into cart 
/// @param db The database where the cart and merch exist 
/// @return True if the amount could be added into cart, false otherwise
bool backend_add_to_cart_execute(size_t id, char *name, size_t item_amount, database_t *db);

/// @brief Removes zero or more items of some merch from a particular cart
/// @pre name != NULL
/// @pre db != NULL
/// @param id The id of the cart
/// @param name The name of the merch
/// @param item_amount THe amount of merch items to remove from the cart
/// @param db The database wehere the cart and merch exist
/// @return True if the amount could be removed from the cart, false otherwise
bool backend_remove_from_cart_execute(size_t id, char *name, size_t item_amount, database_t *db);

/// @brief Calculate the total cost of the items in the shopping cart
/// @pre db != NULL
/// @param id The id of the cart
/// @param db The database where the cart exist
/// @return The total cost of the items in the cart
int backend_calculate_cost_execute(size_t id, database_t *db);

/*
Undos an action.
Multiple undos should be supported (i.e., pressing undo N times undos the N last actions for N≤16.)
You cannot “undo an undo” (aka “redo”).
The action code shall be "U". 
*/
bool backend_undo_execute(database_t *db);

/// @brief Destroys the database and frees all allocated memory associated with it.
/// Frees all merchandise, shelf allocations, duplicated strings, internal hash tables,
/// and the database struct itself.
/// @pre db != NULL
/// @param db The database to destroy. If database is NULL, no operation is performed
void backend_quit_execute(database_t *db);

/*
The program should save its data on disk between runs. There is a discussion about how to implement this further down in this document.
*/
void backend_persistence_on_file(database_t *db);

/// @brief A user going through with the purchase of all items in their shopping cart 
/// and also removes the cart from the system
/// @pre db != NULL
/// @param id The id of the cart
/// @param db The database where the cart exist
/// @return True if the checkout went through otherwise false
bool backend_checkout_execute(size_t id, database_t *db);

/// @brief Retrieves the name of the merch for db_main
/// @pre merch != NULL
/// @param merch The merch to get the name from
/// @return The name of the merch
char *backend_get_merch_name(merch_t *merch);

/// @brief Retrieves the description of the merch for db_main
/// @pre merch != NULL
/// @param merch The merch to get the description from
/// @return The description of the merch
char *backend_get_merch_desc(merch_t *merch);

/// @brief Retrieves the price of the merch for db_main
/// @pre merch != NULL
/// @param merch The merch to get the price from
/// @return The price of the merch
int backend_get_merch_price(merch_t *merch);

/// @brief Retrieves the shelf name for db_main
/// @pre location != NULL
/// @param location The shelf to get the name from
/// @return The name of the shelf
char *backend_get_location_name(shelf_t *location);

/// @brief Retrieves the quantity of the shekf for db_main
/// @pre location != NULL
/// @param location The shelf to get the quantity from
/// @return The quantity of the shelf
int backend_get_location_quantity(shelf_t *location);