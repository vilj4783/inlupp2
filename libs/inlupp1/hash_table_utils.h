#pragma once

#include "common.h"

#define No_Buckets 499

typedef struct entry entry_t;

struct entry
{
  elem_t key;     // holds the key
  elem_t value;     // holds the value
  entry_t *next; // points to the next entry (possibly NULL)
};

typedef struct hash_table ioopm_hash_table_t;

struct hash_table
{
  // DODGE: hard-coding number of buckets as 17.
  // NOTE: addressing this dodge is optional.
  entry_t buckets[No_Buckets];
  int size;
  ioopm_hash_function *hash_fn;
  ioopm_eq_function *key_eq_fn;
};