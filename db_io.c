#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "db_io.h"
#include "./libs/utils/utils.h"

#define INPUT_ITEM "input item name: "
#define INPUT_CART "input cart id: "

#define INPUT_DESC "input item description: "
#define INPUT_PRICE "input item price: "
#define INPUT_AMOUNT "input amount of items: "

#define INPUT_OLD_NAME "input old name: "
#define INPUT_NEW_NAME "input new name: "
#define INPUT_NEW_DESC "input new description: "
#define INPUT_NEW_PRICE "input new item price: " 


bool valid_action(char *str)
{
    // if(str[1] == '\n') return false; // dunno wtf is up w taht

    char c = toupper(str[0]);

    switch (c)
    {
    case 'A': return true;
    case 'L': return true;
    case 'D': return true;
    case 'E': return true;
    case 'S': return true;
    case 'P': return true;
    case 'C': return true;
    case 'R': return true;
    case '+': return true;
    case '-': return true;
    case '=': return true;
    case 'O': return true;
    case 'U': return true;
    case 'Q': return true;
    default: return false; // should never happen so error case
    }
}

action_t convert_action(char *str)
{
    char c = toupper(str[0]);

    switch (c)
    {
    case 'A': return ADD_MERCH;
    case 'L': return LIST_MERCH;
    case 'D': return REMOVE_MERCH;
    case 'E': return EDIT_MERCH;
    case 'S': return SHOW_STOCK;
    case 'P': return REPLENISH;
    case 'C': return CREATE_CART;
    case 'R': return REMOVE_CART;
    case '+': return ADD_TO_CART;
    case '-': return REMOVE_FROM_CART;
    case '=': return CALCULATE_COST;
    case 'O': return CHECKOUT;
    case 'U': return UNDO;
    case 'Q': return QUIT;
    default: return PERSISTENCE_ON_FILE; // should never happen so error case
    }

}

bool bool_valid(char *str)
{
    return str[0] != '\n';
}

bool convert_Y(char *str)
{
    return toupper(str[0]) == 'Y';
}

bool convert_N(char *str)
{
    return !(toupper(str[0]) == 'N');
}


action_t ask_question_action(char *question) {
    return (action_t) ask_question(question, (check_func *) valid_action, (convert_func *) convert_action).int_value;
}

bool ask_question_confirm_Y()
{
    return !((bool) ask_question("input Y or y to continue: ", (check_func *) bool_valid, (convert_func *) convert_Y).int_value);
}
bool ask_question_confirm_N()
{
    return (bool) ask_question("input N or n to list more: ", (check_func *) bool_valid, (convert_func *) convert_N).int_value;
}
void add_merch_input(char **item_name, char **item_desc, int *item_price) {
    *item_name = ask_question_string(INPUT_ITEM);
    *item_desc = ask_question_string(INPUT_DESC);
    *item_price = ask_question_int(INPUT_PRICE);
}

void remove_merch_input(char **item_name) {
    *item_name = ask_question_string(INPUT_ITEM);
}


void edit_merch_input(char **item_name_old, char **item_name_new, char **item_desc_new, int *item_price_new) {
    *item_name_old = ask_question_string(INPUT_OLD_NAME);
    *item_name_new = ask_question_string(INPUT_NEW_NAME);
    *item_desc_new = ask_question_string(INPUT_NEW_DESC);
    *item_price_new= ask_question_int(INPUT_NEW_PRICE);
}

void show_stock_input(char **item_name) {
    *item_name = ask_question_string(INPUT_ITEM);
}
void replenish_input(char **location_name, char **item_name, int *item_amount) {
    *location_name = ask_question_string("input location name: ");
    *item_name = ask_question_string(INPUT_ITEM);
    *item_amount = ask_question_int(INPUT_AMOUNT);
}


void remove_cart_input(int *cart_id) {
    *cart_id = ask_question_int(INPUT_CART);
}

void add_to_cart_input(int *cart_id, char **item_name, int *item_amount) {
    *cart_id = ask_question_int(INPUT_CART);
    *item_name = ask_question_string(INPUT_ITEM);
    *item_amount = ask_question_int(INPUT_AMOUNT);

}

void remove_from_cart_input(int *cart_id, char **item_name, int *item_amount) {
    *cart_id = ask_question_int(INPUT_CART);
    *item_name = ask_question_string(INPUT_ITEM);
    *item_amount = ask_question_int(INPUT_AMOUNT);

}


void calculate_cost_input(int *cart_id) {
    *cart_id = ask_question_int(INPUT_CART);
}

void checkout_input(int *cart_id) {
    *cart_id = ask_question_int(INPUT_CART);
}
