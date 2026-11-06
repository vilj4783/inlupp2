#pragma once

enum ACTION {
    ADD_MERCH,
    LIST_MERCH,
    REMOVE_MERCH,
    EDIT_MERCH,
    SHOW_STOCK,
    REPLENISH,
    CREATE_CART,
    REMOVE_CART,
    ADD_TO_CART,
    REMOVE_FROM_CART,
    CALCULATE_COST,
    CHECKOUT,
    UNDO,
    QUIT,
    PERSISTENCE_ON_FILE
};

typedef enum ACTION action_t;

action_t ask_question_action(char *question);

bool ask_question_confirm_Y();

bool ask_question_confirm_N();


void add_merch_input(char **item_name, char **item_desc, int *item_price);

void remove_merch_input(char **item_name);

void edit_merch_input(char **item_name_old, char **item_name_new, char **item_desc_new, int *item_price_new);

void show_stock_input(char **item_name);

void replenish_input(char **location_name, char **item_name, int *item_amount);

void create_cart_input(int *id);

void remove_cart_input(int *cart_id);

void add_to_cart_input(int *cart_id, char **item_name, int *item_amount);

void remove_from_cart_input(int *cart_id, char **item_name, int *item_amount);

void calculate_cost_input(int *cart_id);

void checkout_input(int *cart_id);