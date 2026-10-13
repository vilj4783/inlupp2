#include "db_backend.h"
#include <stdlib.h>

// NOTE: use void pointers in elem_t to used structs



database_t *database_create(void)
{
    database_t *db =calloc(2, sizeof(database_t));
    db->merch_ht = ioopm_hash_table_create(string_knr_hash, string_equal);
    db->location_ht = ioopm_hash_table_create(string_knr_hash, string_equal);
}

void database_destroy(database_t *db)
{
    // TODO: make it destroy all internal owning pointers (perhasp use an iterator)
    ioopm_hash_table_destroy(db->location_ht);
    ioopm_hash_table_destroy(db->merch_ht);
    free(db);
}

void backend_add_merch_execute(char *location_name, char *name, char *desc, int price, database_t *db)
{
    // TODO: solve the hen and egg problem of insertion into merch_ht and location_ht
}


void backend_list_merch_execute(database_t *db)
{
    // TODO: create an iterator that prints all merch in merch_ht (double check format)
}



bool backend_remove_merch_execute(char *name, database_t *db)
{
    // TODO: solve the hen and egg problem of removal from merch_ht and location_ht
    return false; // CHEAT: 
}


bool backend_edit_merch_execute(char *old_name, char *new_name, char *new_desc, int new_price, database_t *db)
{
    // TODO: solve the hen and egg problem of removal from merch_ht and location_ht
    return false; // CHEAT: 
}

bool backend_show_stock_execute(char *name, database_t *db)
{
    // TODO: implement
    return false; // CHEAT: 
}

bool backend_replenish_execute(int id, char *merch_name, int items_amount, database_t *db)
{
    // TODO: implement
    return false; // CHEAT: 
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

void backend_quit_execute(database_t *db)
{
    // TODO: should free the DB and all other elements
}

void backend_persistence_on_file(database_t *db)
{
    // TODO: (JSON or similar solution)
}