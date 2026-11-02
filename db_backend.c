#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include "db_backend.h"
#include "../inluppar/inlupp1/hash_table.h"
#include "../inluppar/inlupp1/hash_table_iterator.h"
#include "../inluppar/inlupp1/linked_list.h"
#include "../inluppar/inlupp1/list_iterator.h"

typedef struct database database_t;

struct database 
{
    ioopm_hash_table_t *merch_ht;
    ioopm_hash_table_t *shelf_ht;
};

typedef struct merch merch_t;

struct merch
{
    char *name;
    char *desc;
    int price;
    ioopm_list_t *locations;
};
//Ta bort
bool string_key_eq(elem_t k1, elem_t k2)
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

static void destroy_shelf(shelf_t *shelf)
{
    if(shelf == NULL)
    {
        return;
    }
    free(shelf->shelf_name);
    free(shelf);
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
    free(merch);
}

database_t *create_database(void)
{
    database_t *new_database = calloc(1, sizeof(database_t));
    new_database->merch_ht = ioopm_hash_table_create(string_knr_hash, string_key_eq);
    new_database->shelf_ht = ioopm_hash_table_create(string_knr_hash, string_key_eq);
    return new_database;
}

static merch_t *create_merch(char *name, char *desc, int price)
{
    merch_t *new_merch = calloc(1, sizeof(merch_t));
    new_merch->name = strdup(name); //Own place in memory with strdup
    new_merch->desc = strdup(desc);
    new_merch->price = price;
    new_merch->locations = ioopm_list_create();

    if(new_merch->name == NULL || new_merch->desc == NULL || new_merch->locations == NULL)
    {
        destroy_merch(new_merch);
        return NULL;
    }

    return new_merch;
}

static ioopm_list_t *create_shelf(ioopm_list_t *locations, char *shelf_name, int quantity)
{
    shelf_t *shelf = calloc(1, sizeof(shelf_t));
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

    elem_t key = string_elem(new_merch->name);
    elem_t value = ptr_elem(new_merch);

    ioopm_hash_table_insert(merch_ht, key, value);
    return true;
}

//Sort in alphabetic order
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
    // If its bigger than all current, add to last place
    ioopm_list_append(list, str_elem);
}

//Not in alphabetic order yet
ioopm_list_t *backend_list_merch_execute(database_t *database)
{
    assert(database != NULL);
    assert(database->merch_ht != NULL);
    ioopm_list_t *merch_list = ioopm_list_create();
    ioopm_hash_table_iterator_t *it = ioopm_hash_table_iterator_create(database->merch_ht);

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

static void remove_shelf(ioopm_hash_table_t *shelf_ht, ioopm_list_t *locations)
{
    ioopm_list_iterator_t *it = ioopm_list_iterator_create(locations);
    while(!ioopm_list_iterator_at_end(it))
    {
        shelf_t *shelf = ioopm_list_iterator_current(it).p;
        elem_t shelf_key = string_elem(shelf->shelf_name);
        elem_t removed_val;
        // ÄNDRING: Frigör den strdup-allokerade strängen som låg som värde i shelf_ht
        if (ioopm_hash_table_remove(shelf_ht, shelf_key, &removed_val))
        {
            free(removed_val.s);
        }
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

ioopm_list_t *backend_show_stock_execute(database_t *database, char *name)
{
    assert(database != NULL);
    assert(name != NULL);

    elem_t merch_elem;
    // Control if item exist
    if (!ioopm_hash_table_lookup(database->merch_ht, string_elem(name), &merch_elem))
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

    elem_t current;
    if (ioopm_hash_table_lookup(shelf_ht, shelf_name, &current))
    {
        //Checks if another merch on the shelf
        if (strcmp(current.s, merch) != 0) 
        {
            return false; // Shelf is connected with another merch!
        }

        // Merch exist on current shelf, add to quantity
        shelf_t *existing_shelf = find_shelf(target_merch->locations, shelf);
        existing_shelf->quantity += add;
        return true;
    }
    else
    {
        // If shelf doesn't exist, create a new shelf
        create_shelf(target_merch->locations, shelf, add);

        // Hämta nyskapade hyllans allokerade namn som nyckel i shelf_ht istället för stack-variabeln shelf
        shelf_t *newly_added = find_shelf(target_merch->locations, shelf);
        assert(newly_added != NULL);
        ioopm_hash_table_insert(shelf_ht, string_elem(newly_added->shelf_name), string_elem(strdup(merch))); 
        return true;
    }
}

void backend_create_cart_execute(database_t *db)
{
    // TODO: implement
}


bool backend_remove_cart_execute(int id, database_t *db)
{
    // TODO: implement
    return false; // CHEAT:
}

bool backend_add_to_cart_execute(int id, char *name, int item_amount, database_t *db)
{
    // TODO: implement
    return false; // CHEAT: 
}

bool backend_remove_from_cart_execute(int id, char *name, int item_amount, database_t *db)
{
    // TODO: implement (should be fairly simple)
    return false; // CHEAT:
}

int backend_calculate_cost_execute(int id, database_t *db)
{
    // TODO: implement, (sum price for all entries in list)
    return 0;
}

bool backend_undo_execute(database_t *db)
{
    // TODO: implement execution log to DB and use it to reverse changes with opposing operations
    return false; // CHEAT: (may change to void)
}

void backend_quit_execute(database_t *database)
{
    assert(database != NULL);

    // Free all items and their shelfs i merch_ht
    if (database->merch_ht != NULL)
    {
        ioopm_hash_table_iterator_t *it = ioopm_hash_table_iterator_create(database->merch_ht);
        while (!ioopm_hash_table_iterator_at_end(it))
        {
            elem_t val = ioopm_hash_table_iterator_current_value(it);
            destroy_merch(val.p);
            ioopm_hash_table_iterator_advance(it);
        }
        ioopm_hash_table_iterator_destroy(it);
        ioopm_hash_table_destroy(database->merch_ht);
    }

    // Free shelf_ht
    if (database->shelf_ht != NULL)
    {
        ioopm_hash_table_iterator_t *it_shelf = ioopm_hash_table_iterator_create(database->shelf_ht);
        while (!ioopm_hash_table_iterator_at_end(it_shelf))
        {
            elem_t val = ioopm_hash_table_iterator_current_value(it_shelf);
            free(val.s);
            ioopm_hash_table_iterator_advance(it_shelf);
        }
        ioopm_hash_table_iterator_destroy(it_shelf);
        ioopm_hash_table_destroy(database->shelf_ht);
    }

    free(database);
}

void backend_persistence_on_file(database_t *db)
{
    // TODO: (JSON or similar solution)
}