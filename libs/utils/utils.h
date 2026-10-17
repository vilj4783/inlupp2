#ifndef __UTILS_H__
#define __UTILS_H__
#include <stdbool.h>
#include "../../db_common.h"

typedef union { 
  int   int_value;
  float float_value;
  char *string_value;
  char char_value;
  action_t action;
  bool bool_value;
} answer_t;

typedef bool check_func(char *);
typedef answer_t convert_func(char *);

extern char *strdup(const char *);

void clear_input_buffer(void);

int read_string(char *buf, int buf_siz);

bool is_number(char *str);

bool not_empty(char *str);

answer_t ask_question(char *question, check_func *check , convert_func *convert);

char *ask_question_string(char *question);

int ask_question_int(char *question);


#endif 
