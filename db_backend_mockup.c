#pragma once
#include "./libs/inlupp1/linked_list.h"
#include "./libs/inlupp1/hash_table.h"
#include "db_backend.h"
#include <stdlib.h>









void backend_add_merch_execute(char *location_name, char *name, char *desc, int price, database_t *db)
{

}

void backend_list_merch_execute(database_t *db)
{

}


bool backend_remove_merch_execute(char *name, database_t *db)
{
    return true;
}

bool backend_edit_merch_execute(char *old_name, char *new_name, char *new_desc, int new_price, database_t *db)
{
    return true;
}
bool backend_show_stock_execute(char *name, database_t *db)
{
    return true;
}
bool backend_replenish_execute(int id, char *merch_name, int items_amount, database_t *db)
{
    return true;
}
void backend_create_cart_execute(database_t *db)
{

}

bool backend_remove_cart_execute(int id, database_t *db)
{
    return true;
}
bool backend_add_to_cart_execute(int id, char *name, int item_amount, database_t *db)
{
    return true;
}
bool backend_remove_from_cart_execute(int id, char *name, int item_amount, database_t *db)
{
    return true;
}

int backend_calculate_cost_execute(int id, database_t *db)
{
    return -100;
}
bool backend_undo_execute(database_t *db)
{
    return true;
}
void backend_quit_execute(database_t *db)
{

}
void backend_persistence_on_file(database_t *db)
{

}