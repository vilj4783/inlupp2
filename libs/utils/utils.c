#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "utils.h"

extern char *strdup(const char *);

int clear_input_buffer(void) {
    int c;
        
    do
    {
        c = getchar();
    } 
    while (c != '\n' && c != EOF);
    return 0;
}

bool is_digit(char c) {

    if('0' <= c && c <= '9') {
        return true;
    }
    
    return false;
    
}

bool is_number(char *str) {
    
    int len = strlen(str);
    int first = str[0];

    if((first == '-' && len > 1) || is_digit(first) == true) {
        
        for(int i = 1; i < len; i++) {
            
            if(is_digit(str[i]) != true) {
                return false;
            }
        
        }
        return true;
    }
    return false;
}

bool is_float(char *str)
{
    int len = strlen(str);
    int points = 0;

    if (len == 0)
    {
        return false;
    }

    for (int i = 0; i < len; i++)
    {
        if (str[i] == '.' && points == 0)
        {
            points++;
        }
        else if (str[i] == '-' && i == 0)
        {

        }
        else if (!is_digit(str[i]))
        {
            return false;
        } 
    }

    return true;
}

int read_string(char *buf, int buf_siz) {
    
    int max = buf_siz - 1;
    int total = 0;
    int c;

    for(int i = 0; i < max; i++) {
        
        c = getchar();
        if (c == '\n') {
            break;
        } 
        buf[total] = c;
        total++;
    }

    if (total == max) {
        clear_input_buffer();
    }
    
    buf[total] = '\0';

    return total;
    
}

int print(char *str) {
    int length = strlen(str);

    for(int i = 0; i < length; i++) {
        putchar(str[i]);
    }
    return 0;
}

int println(char *str) {
    print(str);
    putchar('\n');
    return 0;
}

answer_t ask_question(char *question, check_func *check, convert_func *convert) {

    int buf_siz = 255;
    char buf[buf_siz];

    do
    {
        puts(question);
        read_string(buf, buf_siz);
    } while (!check(buf));

    return convert(buf);
}

bool not_empty(char *str)
{
  return strlen(str) > 0;
}

answer_t make_float(char *str) {
    return (answer_t) { .float_value = atof(str) };
}

int ask_question_int(char *question) {
    answer_t answer = ask_question(question, is_number, (convert_func *) atoi);
    return answer.int_value;
}

double ask_question_float(char *question) {
    return ask_question(question, is_float, make_float).float_value;
}

char *ask_question_string(char *question) {
    return ask_question(question, not_empty, (convert_func *) strdup).string_value;
}
