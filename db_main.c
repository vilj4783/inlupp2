#include "./db_io.h"
#include "./db_backend.h"
#include "./db_common.h"
#include "./libs/inlupp1/linked_list_iterator.h"
#include "./libs/utils/utils.h"
#include <stdio.h>
#include <stdlib.h>

#define INPUT_ITEM "input item name: "
#define INPUT_CART "input cart id: "

#define INPUT_DESC "input item description: "
#define INPUT_PRICE "input item price: "
#define INPUT_AMOUNT "input amount of items: "

#define INPUT_OLD_NAME "input old name: "
#define INPUT_NEW_NAME "input new name: "
#define INPUT_NEW_DESC "input new description: "
#define INPUT_NEW_PRICE "input new item price: " 

#define INPUT_ACTION "input action: "

#define UNAVAILABLE_ACTION "action unavailable"
#define ACTION_MSG "action:"

void add_merch_unify(database_t *db)
{
    char *item_name; 
    char *item_desc;
    int item_price; 
    
    add_merch_input(&item_name, &item_desc, &item_price);

    bool success = backend_add_merch_execute(item_name, item_desc, item_price, db);
    printf("%s %s merch %s, description %s, price %d\n",
        ACTION_MSG,
        success ? "added" : "failed to add",
        item_name,
        item_desc,
        item_price
    );

    free(item_name);
    free(item_desc);
}

void list_merch_unify(database_t *db)
{
    // IO should be handled here maybe create iterator
    ioopm_list_t *list = backend_list_merch_execute(db);
    ioopm_list_iterator_t *iter = ioopm_list_iterator_create(list);
    do {
        for(size_t i = 0    ; i < 20 && !ioopm_list_iterator_at_end(iter); i++)
        {
            char *merch_name = ioopm_list_iterator_current(iter).s;
            printf("name: %s\n", merch_name);
            // printf("name: %s price: %d desc: %s \n", get_merch_name(merch), get_merch_price(merch), get_merch_desc(merch));
            ioopm_list_iterator_advance(iter);
        }
    } while (!ioopm_list_iterator_at_end(iter) && ask_question_confirm_N());

    ioopm_list_iterator_destroy(iter);
    ioopm_list_destroy(list);
    

    printf("%s listed merch\n", ACTION_MSG);
}

void remove_merch_unify(database_t *db)
{
    char *item_name;
    remove_merch_input(&item_name);

    if (ask_question_confirm_Y())
    {
        printf("%s aborted remove merch %s\n", ACTION_MSG, item_name);
        return;
    }

    bool success = backend_remove_merch_execute(item_name, db);

    printf("%s %s merch %s\n", ACTION_MSG, success ? "removed" : "failed to remove", item_name);
    free(item_name);   
}

void edit_merch_unify(database_t *db)
{
    char *item_name_old;
    char *item_name_new;
    char *item_desc_new;
    int item_price_new;

    edit_merch_input(&item_name_old, &item_name_new, &item_desc_new, &item_price_new);

    if (ask_question_confirm_Y())
    {
        printf("%s aborted remove merch %s\n", ACTION_MSG, item_name_old);
        return;
    }

    bool success = backend_edit_merch_execute(db, item_name_old, item_name_new, item_desc_new, item_price_new);

    printf("%s %s merch %s with new name %s new description %s new price %d\n",
        ACTION_MSG,
        success ? "edited" : "failed to edit",
        item_name_old,
        item_name_new,
        item_desc_new,
        item_price_new
    );
    free(item_name_old);
    free(item_name_new);
    free(item_desc_new);

}

void show_stock_unify(database_t *db)
{
    char *item_name;
    show_stock_input(&item_name);

    ioopm_list_t *list = backend_show_stock_execute(db, item_name);
    ioopm_list_iterator_t *iter = ioopm_list_iterator_create(list);

    while(!ioopm_list_iterator_at_end(iter))
    {
        shelf_t *location = ioopm_list_iterator_current(iter).p;
        printf("name: %s quantity: %d\n", get_location_name(location), get_location_quantity(location));
        ioopm_list_iterator_advance(iter);
    }

    printf("%s %s stock for merch %s\n",
        ACTION_MSG,
        !ioopm_list_is_empty(list) ? "showed" : "failed to show",
        item_name
    );

    ioopm_list_iterator_destroy(iter);
    ioopm_list_destroy(list);
    free(item_name);
}

void replenish_unify(database_t *db)
{
    char *location_name;
    char *item_name;
    int item_amount;

    replenish_input(&location_name, &item_name, &item_amount);

    void replenish_input(char **location_name, char **item_name, int *item_amount);
    // implementation depends on whether backend_replenish_execute takes int id or char *location name
    // CHEAT: assumes for location_name for now
    bool success = backend_replenish_execute(db, item_name, location_name, item_amount);
    printf("%s %s merch %s at location %s by %d\n",
        ACTION_MSG,
        success ? "replenished" : "failed to replenish",
        item_name,
        location_name,
        item_amount
    );
    free(location_name);
    free(item_name);
}

void create_cart_unify(database_t *db)
{
    // FIXME: backend_create_cart_execute needs to return an int for id
    int cart_id = backend_create_cart_execute(db);
    printf("%s created cart with id: %d\n", ACTION_MSG, cart_id);
}

void remove_cart_unify(database_t *db)
{
    int cart_id;
    remove_cart_input(&cart_id);
    
    if (ask_question_confirm_Y())
    {
        printf("%s aborted remove cart for cart %d\n", ACTION_MSG, cart_id);
        return;
    }

    bool success = backend_remove_cart_execute(cart_id, db);
    printf("%s %s cart with id %d\n",
        ACTION_MSG,
        success ? "removed" : "failed to remove",
        cart_id
    );
}

void add_to_cart_unify(database_t *db)
{
    int cart_id;
    char *item_name;
    int item_amount;
    add_to_cart_input(&cart_id, &item_name, &item_amount);

    bool success = backend_add_to_cart_execute(cart_id, item_name, item_amount, db);
    printf("%s %s %d merch %s to cart %d\n",
        ACTION_MSG,
        success ? "added" : "failed to add",
        item_amount,
        item_name,
        cart_id
        );
    free(item_name);
}

void remove_from_cart_unify(database_t *db)
{
    
    int cart_id;
    char *item_name;
    int item_amount;
    remove_from_cart_input(&cart_id, &item_name, &item_amount);
    
    bool success = backend_remove_from_cart_execute(cart_id, item_name, item_amount, db);
    printf("%s %s %d merch %s from cart %d\n",
        ACTION_MSG,
        success ? "removed" : "failed to remove",
        item_amount,
        item_name,
        cart_id
        );
    free(item_name);
}

void calculate_cost_unify(database_t *db)
{
    int cart_id;
    calculate_cost_input(&cart_id);

    int cost = backend_calculate_cost_execute(cart_id, db);
    printf("%s cart %d total cost is %d\n", ACTION_MSG, cart_id, cost);
}

void checkout_unify(database_t *db)
{
    int cart_id;
    checkout_input(&cart_id);
    int success = backend_checkout_execute(cart_id, db);
    printf("%s %s cart %d\n", ACTION_MSG, success ? "checked out" : "failed to check out", cart_id);

}

void undo_unify(database_t *db) {return;} // DODGE: is a stub until implementation

bool quit_unify(database_t *db)
{
    if(ask_question_confirm_Y())
    {
        printf("%s aborted quit\n", ACTION_MSG);
        return false;
    }
    backend_quit_execute(db);
    printf("%s quit program\n", ACTION_MSG);
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
        default: printf(UNAVAILABLE_ACTION); break;
    }
    return false;
}

int main()
{
    database_t *db = create_database(); 
    
    while(true)
    {
        action_t action = ask_question_action(INPUT_ACTION);
        if (action_handler(action, db)) {break;}
    }
}
