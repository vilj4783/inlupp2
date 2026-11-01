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

typedef struct shelf shelf_t;

struct shelf 
{
    char *shelf_name;
    int quantity;
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
    int merch_amount;
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

bool id_key_eq(elem_t k1, elem_t k2)
{
    return k1.i != k2.i;
}

//Ta bort
/*
bool string_equal(elem_t k1, elem_t k2)
{
  return strcmp(k1.s, k2.s) == 0; // if elem_t has field char *string
}
//Ta bort
size_t string_knr_hash(elem_t key)
{
  const char *str = key.s;
  size_t result = 0;
  while (*str != '\0')
  {
    result = result * 31 + ((unsigned char) *str);
    str++;
  }
  return result;
}
*/

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
                ioopm_list_iterator_remove(entry_iterator);
            }
            ioopm_list_iterator_advance(entry_iterator);
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
            ioopm_list_iterator_advance(cart_pointer_iterator);
        }

        ioopm_list_iterator_destroy(cart_pointer_iterator);
        return true;
    }
    return false;
}

void destroy_merch(merch_t *merch)
{
    if(merch == NULL)
    {
        return;
    }

    free(merch->name);
    free(merch->desc);
    //Iterator istället
    ioopm_list_t *locations = merch->locations;
    if (locations != NULL)
    {
        size_t size = ioopm_list_size(locations);
        while(size > 0)
        {
            shelf_t *shelf = ioopm_list_remove(locations, 0).p;
            destroy_shelf(shelf);
            size--;
        }
        ioopm_list_destroy(locations);
    }
    merch_cart_destroy(merch); // destroys all pointers to this merch from carts
    free(merch);
}

database_t *create_database(void)
{
    //Kör Calloc istället 
    database_t *new_database = malloc(sizeof(database_t));
    if(new_database == NULL)
    {
        return NULL;
    }

    new_database->merch_ht = ioopm_hash_table_create(string_knr_hash, string_equal);
    new_database->shelf_ht = ioopm_hash_table_create(string_knr_hash, string_equal);
    new_database->cart_ht = ioopm_hash_table_create(id_hash, id_key_eq);

    /*if(new_database->merch_ht == NULL || new_database->shelf_ht == NULL)
    {
        destroy_database(new_database); TODO
    }*/

    return new_database;
}

static merch_t *create_merch(char *name, char *desc, int price)
{
    merch_t *new_merch = malloc(sizeof(merch_t));
    if(new_merch == NULL)
    {
        return NULL;
    }

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
        return false;
    }
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

static ioopm_list_t *create_shelf(ioopm_list_t *locations, char *shelf_name, int quantity)
{
    if(locations == NULL)
    {
        return NULL;
    }
    shelf_t *shelf = malloc(sizeof(shelf_t));
    if(shelf == NULL)
    {
        return NULL;
    }
    shelf->shelf_name = strdup(shelf_name);
    if (shelf->shelf_name == NULL)
    {
        free(shelf);
        return NULL;
    }
    shelf->quantity = quantity;
    ioopm_list_append(locations, ptr_elem(shelf));
    return locations;
}

static bool is_valid_shelf(char *shelf)
{
    if(shelf == NULL || strlen(shelf) < 2)
    {
        return false;
    }
    if (isalpha(*shelf) && strlen(shelf) > 1)
    {
        shelf++;
        while (*shelf != '\0')
        {
            if (!isdigit(*shelf))
            {
                return false;
            }
            shelf++;
        }
        return true;
    }
    return false;
}


bool backend_add_merch_execute(char *name, char *desc, int price, database_t *database)
{
    assert(database != NULL);
    assert(database->merch_ht != NULL);

    elem_t key_check = string_elem(name);
    ioopm_hash_table_t *merch_ht = database->merch_ht;

    if(ioopm_hash_table_has_key(merch_ht, key_check))
    {
        return false; //Merch already exists
    }
    
    merch_t *new_merch = create_merch(name, desc, price);
    if(new_merch == NULL)
    {
        return false;
    }

    elem_t key = string_elem(name);
    elem_t value = ptr_elem(new_merch);

    ioopm_hash_table_insert(merch_ht, key, value);
    return true;
} // note to self indirect link from merch_t to carts via ID


//Not in alphabetic order yet
ioopm_list_t *backend_list_merch_execute(database_t *database)
{
    ioopm_list_t *merch_list = ioopm_list_create();
    ioopm_hash_table_t *merch_ht = database->merch_ht;
    ioopm_hash_table_iterator_t *it = ioopm_hash_table_iterator_create(merch_ht);

    while(!ioopm_hash_table_iterator_at_end(it))
    {
        elem_t merch = ioopm_hash_table_iterator_current_value(it);
        ioopm_list_append(merch_list, merch);
        ioopm_hash_table_iterator_advance(it);
    }
    ioopm_hash_table_iterator_destroy(it);  
    return merch_list;
}

static void remove_shelf(ioopm_hash_table_t *shelf_ht, ioopm_list_t *locations)
{
    ioopm_list_iterator_t *it = ioopm_list_iterator_create(locations);
    while(!ioopm_list_iterator_at_end(it))
    {
        shelf_t *shelf = ioopm_list_iterator_current(it).p;
        elem_t shelf_key = string_elem(shelf->shelf_name);
        elem_t dummy;
        ioopm_hash_table_remove(shelf_ht, shelf_key, &dummy);
        ioopm_list_iterator_advance(it);
    }
    ioopm_list_iterator_destroy(it);
}

bool backend_remove_merch_execute(char *name, database_t *database)
{
    assert(database != NULL);
    assert(database->merch_ht != NULL);
    assert(database->shelf_ht != NULL);

    ioopm_hash_table_t *merch_ht = database->merch_ht;
    ioopm_hash_table_t *shelf_ht = database->shelf_ht;

    elem_t key = string_elem(name);

    elem_t result_lookup;
    
    if(!ioopm_hash_table_lookup(merch_ht, key, &result_lookup))
    {
        return false; //Merch doesn't exists, can't be removed
    }
    merch_t *merch = result_lookup.p;
    ioopm_list_t *locations = merch->locations;

    remove_shelf(shelf_ht, locations);
    elem_t remove_val;

    ioopm_hash_table_remove(merch_ht, key, &remove_val);
    destroy_merch(merch);

    return true;
}

static void update_shelf_edit_merch(database_t *db, merch_t *merch, const char *new_name)
{
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

bool backend_edit_merch_execute(database_t *database, char *old_name, char *new_name, char *new_desc, int new_price)
{
    if (database == NULL || old_name == NULL || new_name == NULL || new_desc == NULL)
    {
        return false;
    }

    elem_t old_key = string_elem(old_name);
    elem_t new_key = string_elem(new_name);
    ioopm_hash_table_t *merch_ht = database->merch_ht;
    elem_t dummy;
    if(!ioopm_hash_table_lookup(merch_ht, old_key, &dummy) )
    { 
        return false;
    }
    if (strcmp(old_name, new_name) != 0 && ioopm_hash_table_lookup(merch_ht, new_key, &dummy))
    {
        return false;
    }
    //Ändra i merch->ht till det nya
    elem_t removed_merch;
    ioopm_hash_table_remove(merch_ht, old_key, &removed_merch);
    merch_t *merch = removed_merch.p;
    
    if (strcmp(old_name, new_name) != 0)
    {
        free(merch->name);
        merch->name = strdup(new_name);
        update_shelf_edit_merch(database, merch, new_name);
    }
    free(merch->desc);
    merch->desc = strdup(new_desc);
    merch->price = new_price;
    
    ioopm_hash_table_insert(merch_ht, new_key, ptr_elem(merch));
    return true;
}

static shelf_t *find_shelf(ioopm_list_t *locations, const char *shelf_name)
{
    if(locations == NULL)
    {
        return NULL;
    }
    ioopm_list_iterator_t *it = ioopm_list_iterator_create(locations);
    if(it == NULL)
    {
        return NULL;
    }
    shelf_t *shelf;
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

bool backend_replenish_execute(database_t *database, char *merch, char *shelf, int add)
{
    assert(database != NULL);
    assert(database->merch_ht != NULL);
    assert(database->shelf_ht != NULL);

    if(add <= 0 || !is_valid_shelf(shelf))
    {
        return false;
    }    
    ioopm_hash_table_t *merch_ht = database->merch_ht; 
    ioopm_hash_table_t *shelf_ht = database->shelf_ht; 

    elem_t merch_name = string_elem(merch);
    elem_t shelf_name = string_elem(shelf);

    elem_t merch_elem;
    if (!ioopm_hash_table_lookup(merch_ht, merch_name, &merch_elem))
    {
        return false; //Checks if we have item
    }
    merch_t *target_merch = merch_elem.p;

    elem_t *current;
    if (ioopm_hash_table_lookup(shelf_ht, shelf_name, current))
    {
        //Checks if another merch on the shelf
        if (strcmp(current->s, merch) != 0) 
        {
            return false; // Shelf is connected with another merch!
        }

        // Merch exist on current shelf, add to quantity
        shelf_t *existing_shelf = find_shelf(target_merch->locations, shelf);
        existing_shelf->quantity += add;
        target_merch->unclaimed_stock += add;

        return true;
    }
    else
    {
        // If shelf doesn't exist, create a new shelf
        create_shelf(target_merch->locations, shelf, add);
        target_merch->unclaimed_stock += add;
        // Insert shelf to shelf_ht
        ioopm_hash_table_insert(shelf_ht, shelf_name, string_elem(strdup(merch))); 
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

bool backend_add_to_cart_execute(size_t id, char *name, int item_amount, database_t *db)
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
        return false;

    }

}

bool backend_remove_from_cart_execute(size_t id, char *name, int item_amount, database_t *db)
{
    elem_t *merch_elem;
    elem_t *cart_elem;
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
    

    elem_t *cart_elem;
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
    // TODO: should free the DB and all other elements
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

int get_location_quantity(shelf_t *location)
{
    return location->quantity;
}



ioopm_list_t *backend_show_stock_execute(char *name, database_t *db) {
    elem_t result;
    if(ioopm_hash_table_lookup(db->merch_ht, string_elem(name), &result))
    {
        return ((merch_t *)result.p)->locations;
    }
    return NULL;
}

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

            if(ioopm_list_is_empty(cart_entry->merch->locations))
            {
                backend_remove_merch_execute(cart_entry->merch->name, db);
            }
            ioopm_list_iterator_advance(entries);
        }

        ioopm_list_iterator_destroy(entries);


        backend_remove_cart_execute(cart->cart_id, db);
        return  true;
    }

    return false;
}