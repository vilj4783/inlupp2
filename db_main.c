#include "./db_io.h"
#include "./db_backend.h"
#include "./db_common.h"
#include "./libs/inlupp1/linked_list_iterator.h"
#include "./libs/utils/utils.h"
#include <stdio.h>
#include <stdlib.h>

// TODO: all backend functions with a fail state need to return a bool
/*
database_t *database_create(void) // dodge: placeholder function
{
    return calloc(1, sizeof(database_t));
}

bool backend_add_merch_execute(char *location_name, char *name, char *desc, int price, database_t *db)
{
    return true;
}

int backend_create_cart_execute(database_t *db)
{
    return -1;
}

bool backend_checkout_execute(int cart_id, database_t *db)
{
    return true;
}

bool backend_replenish_execute(char *location_name, char *merch_name, int items_amount, database_t *db)
{
    return true;
}
*/


void add_merch_unify(database_t *db)
{
    // char *location_name = ask_question_string("input warehouse name: ");
    char *item_name = ask_question_string("input item name: ");
    char *item_desc = ask_question_string("input item description: ");
    int item_price = ask_question_int("input item price: ");

    bool success = backend_add_merch_execute(item_name, item_desc, item_price, db);
    printf("action: %s merch %s, description %s, price %d\n",
        success ? "added" : "failed to add",
        item_name,
        item_desc,
        item_price
    );

}

void list_merch_unify(database_t *db)
{
    // TODO: fix the implementation of backend_list_merch to specification
    // IO should be handled here maybe create iterator
    ioopm_list_t *list = backend_list_merch_execute(db);
    ioopm_list_iterator_t *iter = ioopm_list_iterator_create(list);
    do {
        for(size_t i = 0    ; i < 20 && !ioopm_list_iterator_at_end(iter); i++)
        {
            merch_t *merch = ioopm_list_iterator_current(iter).p;
            printf("name: %s price: %d desc: %s \n", get_merch_name(merch), get_merch_price(merch), get_merch_desc(merch));
            ioopm_list_iterator_advance(iter);
        }
    } while (!ioopm_list_iterator_at_end(iter) && ask_question_confirm_N("input N or n to list more: "));

    ioopm_list_iterator_destroy(iter);
    ioopm_list_destroy(list);
    

    printf("action: listed merch\n");
}

void remove_merch_unify(database_t *db)
{
    char *item_name = ask_question_string("input item name: ");

    if (!ask_question_confirm_Y("input Y or y to continue: "))
    {
        printf("action: aborted remove merch %s\n", item_name);
        return;
    }

    bool success = backend_remove_merch_execute(item_name, db);

    printf("action: %s merch %s\n", success ? "removed" : "failed to remove", item_name);
    
}

void edit_merch_unify(database_t *db)
{
    char *item_name_old = ask_question_string("input old item name: ");
    char *item_name_new = ask_question_string("input new item name: ");
    char *item_desc_new = ask_question_string("input new item description: ");
    int item_price_new= ask_question_int("input new item price: ");

    if (!ask_question_confirm_Y("input Y or y to continue: "))
    {
        printf("action: aborted remove merch %s\n", item_name_old);
        return;
    }

    bool success = backend_edit_merch_execute(db, item_name_old, item_name_new, item_desc_new, item_price_new);

    printf("action: %s merch %s with new name %s new description %s new price %d\n",
        success ? "edited" : "failed to edit",
        item_name_old,
        item_name_new,
        item_desc_new,
        item_price_new
    );
}

void show_stock_unify(database_t *db)
{
    char *item_name = ask_question_string("input item name: ");

    ioopm_list_t *list = backend_show_stock_execute(item_name, db);
    ioopm_list_iterator_t *iter = ioopm_list_iterator_create(list);

    while(!ioopm_list_iterator_at_end(iter))
    {
        location_t *location = ioopm_list_iterator_current(iter).p;
        printf("name: %s quantity: %d\n", get_location_name(location), get_location_quantity(location));
        ioopm_list_iterator_advance(iter);
    }

    printf("action: %s stock for merch %s\n",
        !ioopm_list_is_empty(list) ? "showed" : "failed to show",
        item_name
    );

    ioopm_list_iterator_destroy(iter);
    ioopm_list_destroy(list);
}

void replenish_unify(database_t *db)
{
    char *location_name = ask_question_string("input location name: ");
    char *item_name = ask_question_string("input item name: ");
    int item_amount = ask_question_int("input amount of items: ");

    // implementation depends on whether backend_replenish_execute takes int id or char *location name
    // CHEAT: assumes for location_name for now
    bool success = backend_replenish_execute(db, location_name, item_name, item_amount);
    printf("action: %s merch %s at location %s by %d\n",
        success ? "replenished" : "failed to replenish",
        item_name,
        location_name,
        item_amount
    );
}

void create_cart_unify(database_t *db)
{
    // FIXME: backend_create_cart_execute needs to return an int for id
    int cart_id = backend_create_cart_execute(db);
    printf("action: created cart with id: %d\n", cart_id);
}

void remove_cart_unify(database_t *db)
{
    int cart_id = ask_question_int("input cart id: ");
    
    if (!ask_question_confirm_Y("input Y or y to continue: "))
    {
        printf("action: aborted remove cart for cart %d\n", cart_id);
        return;
    }

    bool success = backend_remove_cart_execute(cart_id, db);
    printf("action: %s cart with id %d\n",
        success ? "removed" : "failed to remove",
        cart_id
    );
}

void add_to_cart_unify(database_t *db)
{
    int cart_id = ask_question_int("input cart id: ");
    char *item_name = ask_question_string("input item name: ");
    int item_amount = ask_question_int("input amount of items: ");

    bool success = backend_add_to_cart_execute(cart_id, item_name, item_amount, db);
    printf("action: %s %d merch %s to cart %d\n",
        success ? "added" : "failed to add",
        item_amount,
        item_name,
        cart_id
        );
}

void remove_from_cart_unify(database_t *db)
{
    
    int cart_id = ask_question_int("input cart id: ");
    char *item_name = ask_question_string("input item name: ");
    int item_amount = ask_question_int("input amount of items: ");
    
    bool success = backend_remove_from_cart_execute(cart_id, item_name, item_amount, db);
    printf("action: %s %d merch %s to cart %d\n",
        success ? "removed" : "failed to remove",
        item_amount,
        item_name,
        cart_id
        );
}

void calculate_cost_unify(database_t *db)
{
    int cart_id = ask_question_int("input cart id: ");

    int cost = backend_calculate_cost_execute(cart_id, db);
    printf("action: cart %d total cost is %d\n", cart_id, cost);
}

void checkout_unify(database_t *db)
{
    int cart_id = ask_question_int("input cart id: ");
    int success = backend_checkout_execute(cart_id, db);
    printf("action: %s cart %d\n", success ? "checked out" : "failed to check out", cart_id);

}

void undo_unify(database_t *db) {return;} // DODGE: is a stub until implementation

bool quit_unify(database_t *db)
{
    if(!ask_question_confirm_Y("input Y or y to continue: "))
    {
        printf("action: aborted quit\n");
        return false;
    }
    backend_quit_execute(db);
    printf("action: quit program\n");
    return true;
}



bool action_handler(action_t action, database_t *db) // returns true if should quit
{
    switch(action)
    {
        case ADD_MERCH: add_merch_unify(db); break;
        case LIST_MERCH: list_merch_unify(db); break;
        case REMOVE_MERCH: remove_merch_unify(db); break;
        case EDIT_MERCH: edit_merch_unify(db); break;
        case SHOW_STOCK: show_stock_unify(db); break;
        case REPLENISH: replenish_unify(db); break;
        case CREATE_CART: create_cart_unify(db); break;
        case REMOVE_CART: remove_cart_unify(db); break;
        case ADD_TO_CART: add_to_cart_unify(db); break;
        case REMOVE_FROM_CART: remove_from_cart_unify(db); break;
        case CALCULATE_COST: calculate_cost_unify(db); break;
        case CHECKOUT: checkout_unify(db); break;
        case UNDO: undo_unify(db); break;
        case QUIT: return quit_unify(db);
        default: printf("action unavailable"); break;
    }
    return false;
}

int main()
{
    database_t *db = create_database(); 
    
    while(true)
    {
        action_t action = ask_question_action("input action: ");
        if (action_handler(action, db)) {break;}
    }
}
