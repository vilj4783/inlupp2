#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include "./libs/inlupp1/hash_table.h"
#include "./libs/inlupp1/hash_table_iterator.h"
#include "./libs/inlupp1/linked_list.h"
#include "./libs/inlupp1/linked_list_iterator.h"

typedef struct database database_t;

struct database 
{
    ioopm_hash_table_t *merch_ht;
    ioopm_hash_table_t *shelf_ht;
    ioopm_hash_table_t *cart_ht;
};

typedef struct merch merch_t;

struct merch
{
    char *name;
    char *desc;
    int price;
    size_t unclaimed_stock;
    ioopm_list_t *locations;
    ioopm_list_t *carts; // should contain elem_t(s) with a pointer to the cart
};

struct cart_entry
{
    size_t merch_amount;
    merch_t *merch;
};

typedef struct cart_entry cart_entry_t;

struct cart
{
    int cart_id;
    ioopm_list_t *entries;
};

typedef struct cart cart_t;

size_t id_hash(elem_t id)
{
    return id.i;
}

typedef struct shelf shelf_t;

struct shelf 
{
    char *shelf_name;
    size_t quantity;
};

bool id_key_eq(elem_t k1, elem_t k2)
{
    return k1.i != k2.i;
}

/// @brief Destroys a shelf structure and frees its allocated shelf name
/// @param shelf The shelf structure to destroy
static void destroy_shelf(shelf_t *shelf)
{
    if(shelf == NULL)
    {
        return;
    }
    free(shelf->shelf_name);
    free(shelf);
}

static void destroy_cart_entry(cart_entry_t *cart_entry, size_t cart_id)
{
    ioopm_list_iterator_t *cart_id_iterator = ioopm_list_iterator_create(cart_entry->merch->carts);

    while (!ioopm_list_iterator_at_end(cart_id_iterator))
    {
        if (((cart_t *)ioopm_list_iterator_current(cart_id_iterator).p)->cart_id == cart_id)
        {
            ioopm_list_iterator_remove(cart_id_iterator);
            break; // done with loop, can exit function
        }
        ioopm_list_iterator_advance(cart_id_iterator);
    }

    ioopm_list_iterator_destroy(cart_id_iterator);
    free(cart_entry);
}

static void destroy_cart(cart_t *cart)
{
    ioopm_list_iterator_t *iter = ioopm_list_iterator_create(cart->entries);

    while(!ioopm_list_iterator_at_end(iter))
    {
        destroy_cart_entry(ioopm_list_iterator_current(iter).p, cart->cart_id);
        ioopm_list_iterator_advance(iter); 
    }

    ioopm_list_iterator_destroy(iter);
    ioopm_list_destroy(cart->entries);
    free(cart);
}

static bool merch_cart_entry_destroy(merch_t *merch, cart_t *cart) // removes a entry of a merch in a particular cart
{
    // ioopm_hash_table_lookup(db->cart_ht, key, cart)
    if (cart != NULL)
    {
        
        ioopm_list_iterator_t *entry_iterator = ioopm_list_iterator_create(cart->entries);
        while(!ioopm_list_iterator_at_end(entry_iterator)) // look for the entry with the relevant merch link
        {
            cart_entry_t *entry = ioopm_list_iterator_current(entry_iterator).p;
            if (entry->merch == merch) // compares addresses
            {
                // destroys cart entry and removes it from list
                destroy_cart_entry(entry, cart->cart_id);
                free(entry);
            } else {
                ioopm_list_iterator_advance(entry_iterator);
            }
        }
        ioopm_list_iterator_destroy(entry_iterator);
        return true;
    }
    return false;
}


static bool merch_cart_destroy(merch_t *merch) // removes entries of a merch in all carts
{
    if(!ioopm_list_is_empty(merch->carts))
    {
        ioopm_list_iterator_t *cart_pointer_iterator = ioopm_list_iterator_create(merch->carts);

        while(!ioopm_list_iterator_at_end(cart_pointer_iterator)) // through all cart ids in the merch struct (list)
        {

            merch_cart_entry_destroy(merch, ioopm_list_iterator_current(cart_pointer_iterator).p);   
            ioopm_list_iterator_remove(cart_pointer_iterator);
        }

        ioopm_list_iterator_destroy(cart_pointer_iterator);
        return true;
    }
    return false;
}

/// @brief Destroys a merchandise item and frees all associated memory
/// @param merch The merch to destroy
static void destroy_merch(merch_t *merch)
{
    if(merch == NULL)
    {
        return;
    }

    free(merch->name);
    free(merch->desc);

    ioopm_list_t *locations = merch->locations;
    if (!ioopm_list_is_empty(locations)) 
    {
        ioopm_list_iterator_t *it = ioopm_list_iterator_create(locations);
        while (!ioopm_list_iterator_at_end(it))
        {
            shelf_t *shelf = ioopm_list_iterator_current(it).p;
            destroy_shelf(shelf);
            ioopm_list_iterator_advance(it);
        }
        ioopm_list_iterator_destroy(it);
    }
    ioopm_list_destroy(locations);
    merch_cart_destroy(merch); // destroys all pointers to this merch from carts
    ioopm_list_destroy(merch->carts);
    free(merch);
}

database_t *create_database(void)
{
    database_t *new_database = calloc(1, sizeof(database_t));
    new_database->merch_ht = ioopm_hash_table_create(string_knr_hash, string_equal);
    new_database->shelf_ht = ioopm_hash_table_create(string_knr_hash, string_equal);
    new_database->cart_ht = ioopm_hash_table_create(id_hash, id_key_eq);
    return new_database;
}

/// @brief Creates a new merch
/// @param name Name of the merch
/// @param desc Description of the merch
/// @param price Price of the merch
/// @return Pointer to newly allocated merch_t, or NULL if allocation fails
static merch_t *create_merch(char *name, char *desc, int price)
{
    merch_t *new_merch = calloc(1, sizeof(merch_t));
    new_merch->name = strdup(name); //Own place in memory with strdup
    new_merch->desc = strdup(desc);
    new_merch->price = price;
    new_merch->locations = ioopm_list_create();
    new_merch->carts = ioopm_list_create();
    new_merch->unclaimed_stock = 0;

    if(new_merch->name == NULL || new_merch->desc == NULL || new_merch->locations == NULL)
    {
        destroy_merch(new_merch);
        return NULL;
    }

    return new_merch;
}

static bool is_in_cart(merch_t *merch, cart_t *cart)
{
    if(!ioopm_list_is_empty(merch->carts))
    {
        ioopm_list_iterator_t *cart_pointer_iterator = ioopm_list_iterator_create(merch->carts);

        while(!ioopm_list_iterator_at_end(cart_pointer_iterator)) // through all cart ids in the merch struct (list)
        {

            if(ioopm_list_iterator_current(cart_pointer_iterator).p == cart)   
            {
                ioopm_list_iterator_destroy(cart_pointer_iterator);
                return true;
            }
            ioopm_list_iterator_advance(cart_pointer_iterator);
        }

        ioopm_list_iterator_destroy(cart_pointer_iterator);
    }
    return false;
}

static cart_t *create_cart(size_t id)
{
    cart_t *cart = calloc(1, sizeof(cart_t));

    cart->cart_id = id; // creator keeps track of this via ht size
    cart->entries = ioopm_list_create();

    return cart;
}

static cart_entry_t *create_cart_entry(merch_t *merch, size_t merch_amount, cart_t *cart)
{
    cart_entry_t *cart_entry = calloc(1, sizeof(cart_entry_t));
    cart_entry->merch = merch;
    cart_entry->merch_amount = merch_amount;


    ioopm_list_append(cart->entries, ptr_elem(cart_entry));
    ioopm_list_append(merch->carts, ptr_elem(cart));

    return cart_entry;
}

/// @brief Creates a new shelf
/// @param locations Location list where the new shelf is added
/// @param shelf_name Name of the shelf
/// @param quantity Initial quantity placed on this shelf
/// @return Pointer to newly allocated shelf_t
static shelf_t *create_shelf(ioopm_list_t *locations, char *shelf_name, int quantity)
{
    assert(locations != NULL);
    assert(shelf_name != NULL);
    assert(quantity > 0);
    shelf_t *shelf = calloc(1, sizeof(shelf_t));
    shelf->shelf_name = strdup(shelf_name);
    if (shelf->shelf_name == NULL)
    {
        free(shelf);
        return NULL;
    }
    shelf->quantity = quantity;
    ioopm_list_append(locations, ptr_elem(shelf));
    return shelf;
}

/// @brief Validates if a shelf string follows the correct format (eg. A01)
/// @param shelf The shelf identifier to validate
/// @return True if format is valid, false otherwise
static bool is_valid_shelf(const char *shelf)
{
    if (shelf == NULL || strlen(shelf) != 3)
    {
        return false;
    }
    return isupper((unsigned char)shelf[0]) && 
           isdigit((unsigned char)shelf[1]) && 
           isdigit((unsigned char)shelf[2]);
}


bool backend_add_merch_execute(char *name, char *desc, int price, database_t *db)
{
    assert(db != NULL);
    assert(db->merch_ht != NULL);
    assert(name != NULL);
    assert(desc != NULL);
    assert(price >= 0);

    elem_t key_check = string_elem(name);
    ioopm_hash_table_t *merch_ht = db->merch_ht;

    // Check if item name already exists in database
    if(ioopm_hash_table_has_key(merch_ht, key_check))
    {
        return false;
    }
    
    merch_t *new_merch = create_merch(name, desc, price);
    if(new_merch == NULL)
    {
        return false;
    }

    // Insert merch into hash table
    elem_t key = string_elem(new_merch->name);
    elem_t value = ptr_elem(new_merch);
    ioopm_hash_table_insert(merch_ht, key, value);
    return true;
} // note to self indirect link from merch_t to carts via ID


/// @brief Inserts a string element into a linked list in ascending alphabetical order
/// @param list Target linked list to insert into
/// @param str_elem String element (elem_t.s) to insert
static void list_insert_sorted_string(ioopm_list_t *list, elem_t str_elem)
{
    size_t size = ioopm_list_size(list);
    
    for (size_t i = 0; i < size; i++)
    {
        elem_t current = ioopm_list_get(list, i);
        // strcmp gives < 0 if str_elem should be before current
        if (strcmp(str_elem.s, current.s) < 0)
        {
            ioopm_list_insert(list, i, str_elem);
            return;
        }
    }
    // If its greater than all elements, add to last place
    ioopm_list_append(list, str_elem);
}

//Not in alphabetic order yet
ioopm_list_t *backend_list_merch_execute(database_t *db)
{
    assert(db != NULL);
    assert(db->merch_ht != NULL);
    ioopm_list_t *merch_list = ioopm_list_create();
    ioopm_hash_table_iterator_t *it = ioopm_hash_table_iterator_create(db->merch_ht);

    while (!ioopm_hash_table_iterator_at_end(it))
    {
        elem_t merch_name = ioopm_hash_table_iterator_current_key(it);
        
        // Get the list sorted
        list_insert_sorted_string(merch_list, merch_name);
        
        ioopm_hash_table_iterator_advance(it);
    }
    ioopm_hash_table_iterator_destroy(it);

    return merch_list;
}

/// @brief Removes and frees all shelves associated with a merchandise item from shelf_ht
/// @param shelf_ht Hash table mapping shelf names to merchandise names
/// @param locations List of shelf locations belonging to the item
static void remove_shelf(ioopm_hash_table_t *shelf_ht, ioopm_list_t *locations)
{
    ioopm_list_iterator_t *it = ioopm_list_iterator_create(locations);
    while(!ioopm_list_iterator_at_end(it))
    {
        shelf_t *shelf = ioopm_list_iterator_current(it).p;
        elem_t shelf_key = string_elem(shelf->shelf_name);
        elem_t removed_val;

        if (ioopm_hash_table_remove(shelf_ht, shelf_key, &removed_val))
        {
            free(removed_val.s);
        }
        ioopm_list_iterator_advance(it);
    }
    ioopm_list_iterator_destroy(it);
}

bool backend_remove_merch_execute(char *name, database_t *db)
{
    assert(db != NULL);
    assert(db->merch_ht != NULL);
    assert(db->shelf_ht != NULL);
    assert(name != NULL);

    ioopm_hash_table_t *merch_ht = db->merch_ht;
    ioopm_hash_table_t *shelf_ht = db->shelf_ht;
    elem_t key = string_elem(name);
    elem_t result_lookup;
    
    // Return false if item does not exist
    if(!ioopm_hash_table_lookup(merch_ht, key, &result_lookup))
    {
        return false;
    }
    merch_t *merch = result_lookup.p;

    // Free all associated shelves and remove mapping from shelf_ht
    remove_shelf(shelf_ht, merch->locations);
    
    elem_t remove_val;
    ioopm_hash_table_remove(merch_ht, key, &remove_val);
    destroy_merch(merch);

    return true;
}

/// @brief Updates shelf_ht mapping when a merchandise item changes name
/// @param db The database where the merch and shelf exist in
/// @param merch The merchandise item being renamed
/// @param new_name The new name assigned to the merchandise
static void update_shelf_edit_merch(database_t *db, merch_t *merch, const char *new_name)
{
    assert(db != NULL);
    assert(db->shelf_ht != NULL);
    assert(merch != NULL);
    assert(merch->locations != NULL);
    assert(new_name != NULL);

    ioopm_list_iterator_t *it = ioopm_list_iterator_create(merch->locations);
    while (!ioopm_list_iterator_at_end(it))
    {
        shelf_t *shelf = ioopm_list_iterator_current(it).p;
        elem_t shelf_k = string_elem(shelf->shelf_name);

        elem_t old_val;
        ioopm_hash_table_remove(db->shelf_ht, shelf_k, &old_val);
        free(old_val.s);

        ioopm_hash_table_insert(db->shelf_ht, shelf_k, string_elem(strdup(new_name)));
        ioopm_list_iterator_advance(it);
    }
    ioopm_list_iterator_destroy(it);
}

bool backend_edit_merch_execute(database_t *db, char *old_name, char *new_name, char *new_desc, int new_price)
{
    assert(db != NULL);
    assert(db->merch_ht != NULL);
    assert(db->shelf_ht != NULL);
    assert(old_name != NULL);
    assert(new_name != NULL);
    assert(new_desc != NULL);
    assert(new_price >= 0);

    elem_t old_key = string_elem(old_name);
    elem_t new_key = string_elem(new_name);
    ioopm_hash_table_t *merch_ht = db->merch_ht;
    elem_t dummy;

    // Item must exist to be edited
    if(!ioopm_hash_table_lookup(merch_ht, old_key, &dummy) )
    { 
        return false;
    }
    // New name cannot collide with another existing merch
    if (strcmp(old_name, new_name) != 0 && ioopm_hash_table_lookup(merch_ht, new_key, &dummy))
    {
        return false;
    }

    elem_t removed_merch;
    ioopm_hash_table_remove(merch_ht, old_key, &removed_merch);
    merch_t *merch = removed_merch.p;
    
    // Update name and associated shelf registrations if renamed
    if (strcmp(old_name, new_name) != 0)
    {
        free(merch->name);
        merch->name = strdup(new_name);
        update_shelf_edit_merch(db, merch, new_name);
    }

    // Update description and price
    free(merch->desc);
    merch->desc = strdup(new_desc);
    merch->price = new_price;
    
    // Re-insert into hash table
    ioopm_hash_table_insert(merch_ht, string_elem(merch->name), ptr_elem(merch));
    return true;
}

/// @brief Inserts a shelf into a list in alphabetical order by shelf_name
/// @param list Target linked list
/// @param shelf Shelf to insert
static void list_insert_sorted_shelf(ioopm_list_t *list, elem_t shelf_elem)
{
    shelf_t *new_shelf = shelf_elem.p;
    size_t size = ioopm_list_size(list);

    for (size_t i = 0; i < size; i++)
    {
        shelf_t *current_shelf = ioopm_list_get(list, i).p;
        if (strcmp(new_shelf->shelf_name, current_shelf->shelf_name) < 0)
        {
            ioopm_list_insert(list, i, shelf_elem);
            return;
        }
    }
    ioopm_list_append(list, shelf_elem);
}

ioopm_list_t *backend_show_stock_execute(database_t *db, char *name)
{
    assert(db != NULL);
    assert(name != NULL);

    elem_t merch_elem;

    if (!ioopm_hash_table_lookup(db->merch_ht, string_elem(name), &merch_elem))
    {
        return NULL; // Item doesn't exist
    }

    merch_t *merch = merch_elem.p;
    ioopm_list_t *sorted_shelves = ioopm_list_create();

    // Iterate over items unsorted locations
    ioopm_list_iterator_t *it = ioopm_list_iterator_create(merch->locations);
    while (!ioopm_list_iterator_at_end(it))
    {
        elem_t shelf = ioopm_list_iterator_current(it);
        list_insert_sorted_shelf(sorted_shelves, shelf);
        ioopm_list_iterator_advance(it);
    }
    ioopm_list_iterator_destroy(it);

    return sorted_shelves;
}

/// @brief Finds a shelf matching the shelf name within a location list
/// @param locations List containing shelf_t elements
/// @param shelf Shelf identifier to search for
/// @return Pointer to the found shelf_t, or NULL if not found
static shelf_t *find_shelf(ioopm_list_t *locations, const char *shelf_name)
{
    if (locations == NULL || shelf_name == NULL)
    {
        return NULL;
    }
    ioopm_list_iterator_t *it = ioopm_list_iterator_create(locations);
    shelf_t *shelf = NULL;
    while(!ioopm_list_iterator_at_end(it))
    {
        shelf_t *current_shelf = ioopm_list_iterator_current(it).p;
        if(strcmp(current_shelf->shelf_name, shelf_name) == 0)
        {
            shelf = current_shelf;
            break;
        }
        ioopm_list_iterator_advance(it);
    }
    ioopm_list_iterator_destroy(it);
    return shelf;
}

bool backend_replenish_execute(database_t *db, char *merch, char *shelf, size_t add)
{
    assert(db != NULL);
    assert(db->merch_ht != NULL);
    assert(db->shelf_ht != NULL);
    assert(merch != NULL);
    assert(shelf != NULL);

    if (add == 0 || !is_valid_shelf(shelf))
    {
        return false;
    }

    ioopm_hash_table_t *merch_ht = db->merch_ht; 
    ioopm_hash_table_t *shelf_ht = db->shelf_ht; 
    elem_t merch_name = string_elem(merch);
    elem_t shelf_name = string_elem(shelf);
    elem_t merch_elem;

    // Merchandise item must exist
    if (!ioopm_hash_table_lookup(merch_ht, merch_name, &merch_elem))
    {
        return false;
    }

    merch_t *target_merch = merch_elem.p;
    elem_t *current = &string_elem("placeholder");

    // Shelf already in use
    if (ioopm_hash_table_lookup(shelf_ht, shelf_name, current))
    {
        //Checks if another merch uses the shelf
        if (strcmp(current->s, merch) != 0) 
        {
            return false; // Shelf is connected with another merch!
        }

        // Increase stock on existing shelf
        shelf_t *existing_shelf = find_shelf(target_merch->locations, shelf);
        existing_shelf->quantity += add;
        target_merch->unclaimed_stock += add;
        return true;
    }
    else
    {
        // Create new shelf for merch and insert shelf name to merch in shelf_ht
        target_merch->unclaimed_stock += add;
        shelf_t *new_shelf = create_shelf(target_merch->locations, shelf, add);
        assert(new_shelf != NULL);
        ioopm_hash_table_insert(shelf_ht, string_elem(new_shelf->shelf_name), string_elem(strdup(merch))); 
        return true;
    }
}

int backend_create_cart_execute(database_t *db)
{
    // TODO: implement
    size_t cart_amount = ioopm_hash_table_size(db->cart_ht);
    cart_t *cart = create_cart(cart_amount);
    elem_t key = int_elem(cart_amount);
    ioopm_hash_table_insert(db->cart_ht, key, ptr_elem(cart));
    return cart->cart_id;
}


bool backend_remove_cart_execute(size_t id, database_t *db)
{
    // TODO: restore  cart reserved merch
    elem_t *cart_elem = &ptr_elem(NULL); // placeholder

    elem_t key = int_elem(id);
    
    if(ioopm_hash_table_remove(db->cart_ht, key, cart_elem))
    {
        destroy_cart(cart_elem->p);
        return true;
    }
    return false;;
}

bool backend_add_to_cart_execute(size_t id, char *name, size_t item_amount, database_t *db)
{   
    elem_t *merch_elem = &string_elem("hello world"); // placeholder string
    elem_t *cart_elem = &ptr_elem(NULL); // placeholder ptr_elem
    
    if(ioopm_hash_table_lookup(db->merch_ht, string_elem(name), merch_elem) &&
       ioopm_hash_table_lookup(db->cart_ht, int_elem(id), cart_elem) &&
       ((merch_t *)merch_elem->p)->unclaimed_stock >= item_amount)
    {
        merch_t *merch = merch_elem->p;
        cart_t *cart = cart_elem->p;

        merch->unclaimed_stock -= item_amount;

        if(is_in_cart(merch, cart))
        { // TOOD extract this function
            ioopm_list_iterator_t *entry_iterator = ioopm_list_iterator_create(cart->entries);
            while(!ioopm_list_iterator_at_end(entry_iterator)) // look for the entry with the relevant merch link
            {
                cart_entry_t *entry = ioopm_list_iterator_current(entry_iterator).p;
                if (entry->merch == merch) // compares addresses
                {
                    // destroys cart entry and removes it from list
                    entry->merch_amount += item_amount;
                }
                ioopm_list_iterator_advance(entry_iterator);
            }
            ioopm_list_iterator_destroy(entry_iterator);
            return true;

        } else
        {
            create_cart_entry(merch, item_amount, cart);
            
            return true;
        }

    }
    return false;

}

bool backend_remove_from_cart_execute(size_t id, char *name, size_t item_amount, database_t *db)
{
    elem_t *merch_elem = &string_elem("placeholder");
    elem_t *cart_elem = &string_elem("placeholder");
    elem_t key = int_elem(id);
    
    ioopm_hash_table_lookup(db->cart_ht, key, cart_elem);
    ioopm_hash_table_lookup(db->merch_ht, string_elem(name), merch_elem);

    cart_t *cart = cart_elem->p;
    merch_t *merch = merch_elem->p;

    ioopm_list_iterator_t *entry_iterator = ioopm_list_iterator_create(cart->entries);

    cart_entry_t *entry;
    while (!ioopm_list_iterator_at_end(entry_iterator))
    {
        entry = ioopm_list_iterator_current(entry_iterator).p;
        if (entry->merch == merch) // FIXME: once again identifer comparison might be needed
        {
            if (entry->merch_amount > item_amount)
            {
                entry->merch_amount -= item_amount;
                merch->unclaimed_stock += item_amount;

            } else
            {
                destroy_cart_entry(entry, id);
                merch->unclaimed_stock += entry->merch_amount;
                ioopm_list_iterator_destroy;(entry_iterator);
            }
        }
        ioopm_list_iterator_advance(entry_iterator);
    }

    ioopm_list_iterator_destroy(entry_iterator);


    return false; // CHEAT:
}

int backend_calculate_cost_execute(size_t id, database_t *db)
{
    

    elem_t *cart_elem = &string_elem("placeholder");
    elem_t key = int_elem(id);
    
    if (!ioopm_hash_table_lookup(db->cart_ht, key, cart_elem)) return 0;
    cart_t *cart = cart_elem->p;


    ioopm_list_iterator_t *entry_iterator = ioopm_list_iterator_create(cart->entries);
    cart_entry_t *entry;
    int sum = 0;

    while (!ioopm_list_iterator_at_end(entry_iterator))
    {
        entry = ioopm_list_iterator_current(entry_iterator).p;
        sum += entry->merch_amount * entry->merch->price;

        ioopm_list_iterator_advance(entry_iterator);
    }

    ioopm_list_iterator_destroy(entry_iterator);
    return sum;

}

bool backend_undo_execute(database_t *db)
{
    // TODO: implement execution log to DB and use it to reverse changes with opposing operations
    return false; // CHEAT: (may change to void)
}

void backend_quit_execute(database_t *db)
{
    assert(db != NULL);

    // Free all items and their shelfs i merch_ht
    if (db->merch_ht != NULL)
    {
        ioopm_hash_table_iterator_t *it = ioopm_hash_table_iterator_create(db->merch_ht);
        while (!ioopm_hash_table_iterator_at_end(it))
        {
            elem_t val = ioopm_hash_table_iterator_current_value(it);
            destroy_merch(val.p);
            ioopm_hash_table_iterator_advance(it);
        }
        ioopm_hash_table_iterator_destroy(it);
        ioopm_hash_table_destroy(db->merch_ht);
    }

    // Free all duplicated merch names stored in shelf_ht
    if (db->shelf_ht != NULL)
    {
        ioopm_hash_table_iterator_t *it_shelf = ioopm_hash_table_iterator_create(db->shelf_ht);
        while (!ioopm_hash_table_iterator_at_end(it_shelf))
        {
            elem_t val = ioopm_hash_table_iterator_current_value(it_shelf);
            free(val.s);
            ioopm_hash_table_iterator_advance(it_shelf);
        }
        ioopm_hash_table_iterator_destroy(it_shelf);
        ioopm_hash_table_destroy(db->shelf_ht);
    }
    // Destroy cart hash table (if allocated)
    if (db->cart_ht != NULL)
    {
        ioopm_hash_table_destroy(db->cart_ht);
    }

    free(db);
}

void backend_persistence_on_file(database_t *db)
{
    // TODO: (JSON or similar solution)
}

char *get_merch_name(merch_t *merch)
{
    return merch->name;
}

char *get_merch_desc(merch_t *merch)
{
    return merch->desc;
}

int get_merch_price(merch_t *merch)
{
    return merch->price;
}



char *get_location_name(shelf_t *location)
{
    return location->shelf_name;
}

size_t get_location_quantity(shelf_t *location)
{
    return location->quantity;
}


/*
ioopm_list_t *backend_show_stock_execute(char *name, database_t *db) {
    elem_t result;
    if(ioopm_hash_table_lookup(db->merch_ht, string_elem(name), &result))
    {
        return ((merch_t *)result.p)->locations;
    }
    return NULL;
}
*/
bool backend_checkout_execute(size_t id, database_t *db)
{
    elem_t cart_elem = string_elem("placeholder");

    if(ioopm_hash_table_lookup(db->cart_ht, int_elem(id), &cart_elem))
    {
        cart_t *cart = cart_elem.p;

        ioopm_list_iterator_t *entries = ioopm_list_iterator_create(cart->entries);

        while(!ioopm_list_iterator_at_end(entries))
        {
            // create a iterator for locations
            cart_entry_t *cart_entry = ioopm_list_iterator_current(entries).p;
            ioopm_list_iterator_t *locations = ioopm_list_iterator_create(cart_entry->merch->locations);
        


            while(!ioopm_list_iterator_at_end(locations) && cart_entry->merch_amount > 0)
            {
                shelf_t *location = ioopm_list_iterator_current(locations).p;

                if(location->quantity > cart_entry->merch_amount)
                {
                    location->quantity -= cart_entry->merch_amount;
                    cart_entry->merch_amount = 0;
                    ioopm_list_iterator_advance(locations);
                } else
                {
                    cart_entry->merch_amount -= location->quantity;
                    location->quantity = 0;
                    destroy_shelf(location); // FIXME: destroy location
                    ioopm_list_iterator_remove(locations);
                }
            }
            ioopm_list_iterator_destroy(locations);

            
            ioopm_list_iterator_advance(entries);
        }

        ioopm_list_iterator_destroy(entries);


        backend_remove_cart_execute(cart->cart_id, db);
        return  true;
    }

    return false;
}