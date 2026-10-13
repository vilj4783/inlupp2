#pragma once
#include "common.h"

typedef struct node node_t;


struct node
{
  elem_t value;     // holds the value
  node_t *next; // posize_ts to the next entry (possibly NULL)
};

struct list
{
    node_t *head_node;
    size_t size;
};