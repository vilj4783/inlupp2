#pragma once

#include <stdbool.h>
#include <stddef.h>


/**
* @file common.h
* @author Victor Ljungberg
* @date 2026-09-19 // write the date you started working on this
* @brief an underlying datatype used for elements in hash_table_t and linked_list_t in order to generalize them
*
* Here typically goes a more extensive explanation of what the header
* defines. Doxygens tags are words preceeded by either a backslash @\
* or by an at symbol @@.
*
*/

// macros for casting into elements of some the types
#define int_elem(x)   ((elem_t) { .i = (x) })
#define bool_elem(x)  ((elem_t) { .b = (x) })
#define ptr_elem(x)   ((elem_t) { .p = (x) })
#define string_elem(x) ((elem_t) { .s = (x) })



typedef union elem elem_t;

union elem
{
  int i;
  unsigned int u;
  bool b;
  float f;
  void *p;
  char *s;
};



