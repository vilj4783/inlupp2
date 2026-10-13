#include <stdio.h>
#include "./utils.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

extern char *strdup(const char *);

void clear_input_buffer(void)
{
    int c;
    do
    {
        c = getchar();
    }
    while (c != '\n' && c != EOF);
}

int read_string(char *buf, int buf_siz)
{
    for (int i = 0; i < buf_siz - 1; i++)
    {
        int c = getchar();
        if (c == EOF || c == '\n')
        {
            buf[i] = '\0';
            return i;
        }
        buf[i] = c;
    }
    buf[buf_siz - 1] = '\0';
    clear_input_buffer();
    return buf_siz - 1;
}

bool is_number(char *str)
{
    for (size_t i = 0; i < strlen(str); i++)
    {
        if (i == 0 && str[i] == '-' && strlen(str) > 1)
        {
            continue;
        }
        if (!isdigit(str[i]))
        {
            return false;
        }
    }
    return true;
}

bool not_empty(char *str)
{
  return strlen(str) > 0;
}

answer_t ask_question(char *question, check_func *check , convert_func *convert)
{
    char answer[255];
    bool valid_answer = false;
    while (!valid_answer)
    {
        printf("%s\n", question);
        read_string(answer, sizeof(answer));
        valid_answer = check(answer);
    }
    return convert(answer);
}

char *ask_question_string(char *question)
{
  return ask_question(question, not_empty, (convert_func *) strdup).string_value;
}

int ask_question_int(char *question)
{ 
  answer_t answer = ask_question(question, is_number, (convert_func *) atoi);
  return answer.int_value; // svaret som ett heltal
}

