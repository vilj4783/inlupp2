#include "./db_io.h"
#include "./db_backend.h"
#include "./db_common.h"



void action_handler()
{
    // handles each action
    action_t action = ask_question_action("input action: ");
}

int main()
{
    action_handler();
}
