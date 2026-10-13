#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "db_io.h"
#include "./libs/utils/utils.h"


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

bool convert_Y(char *str)
{
    return toupper(str[0]) == 'Y';
}

bool convert_N(char *str)
{
    return !(toupper(str[0]) == 'N');
}


action_t ask_question_action(char *question) {
    return ask_question(question, (check_func *) valid_action, (convert_func *) convert_action).action;
}

bool ask_question_confirm_Y(char *question)
{
    // TODO: returns true on 'Y' or 'y' and false on all other
}

bool ask_question_confirm_N(char *question)
{
    // TODO: returns false on 'N' or 'n' and true on all other
}
