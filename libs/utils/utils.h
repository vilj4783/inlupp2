#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdbool.h>


typedef union { 
  int   int_value;
  float float_value;
  char *string_value;
  char char_value;
} answer_t;

typedef bool check_func(char *);
typedef answer_t convert_func(char*);

int clear_input_buffer(void);

bool is_digit(char c);
bool is_number(char *str);
bool is_float(char *str);

int read_string(char *buf, int buf_siz);

int print(char *str);
int println(char *str);

answer_t ask_question(char *question, check_func *check, convert_func *convert);

bool not_empty(char *str);
answer_t make_float(char *str);

int ask_question_int(char *question);
double ask_question_float(char *question);
char *ask_question_string(char *question);


#endif 
