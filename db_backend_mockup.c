#pragma once
#include "./libs/inlupp1/linked_list.h"
#include "./libs/inlupp1/hash_table.h"


struct database
{

};
typedef struct database database_t;

struct merch {
    char *name;
    char *desc;
    int price;
};

typedef struct merch merch_t;

struct location
{
    int quantity;
    char *shelf_name
};

typedef struct location location_t;

database_t *create_database(void) {
    return NULL;
}


 bool backend_add_merch_execute(char *name, char *desc, int price, database_t *db) {
    return true;
}


ioopm_list_t *backend_list_merch_execute(database_t *database) {
    return NULL;
}


 bool backend_remove_merch_execute(char *name, database_t *db) {
    return true;
}


 bool backend_edit_merch_execute(database_t *database, char *old_name, char *new_name, char *new_desc, int new_price) {
    return true;
}


 bool backend_show_stock_execute(char *name, database_t *db) {
    return true;
}


 bool backend_replenish_execute(database_t *database, char *merch, char *shelf, int add) {
    return true;
}


int backend_create_cart_execute(database_t *db) {
    return 1;
}



 bool backend_remove_cart_execute(size_t id, database_t *db) {
    return true;
}


 bool backend_add_to_cart_execute(size_t id, char *name, int item_amount, database_t *db) {
    return true;
}


 bool backend_remove_from_cart_execute(size_t id, char *name, int item_amount, database_t *db) {
    return true;
}


int backend_calculate_cost_execute(size_t id, database_t *db) {
    return 1;
}


 bool backend_undo_execute(database_t *db) {
    return true;
}


void backend_quit_execute(database_t *db) {

}


void backend_persistence_on_file(database_t *db) {

}

bool backend_checkout_execute(size_t id, database_t *db)
{
    return true;
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



char *get_location_name(location_t *location)
{
    return location->shelf_name;
}

int get_location_quantity(location_t *location)
{
    return location->quantity;
}