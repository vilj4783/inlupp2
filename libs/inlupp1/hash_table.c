#include "hash_table.h"
#include "hash_table_utils.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// typedefs


// functions

static entry_t *entry_create(elem_t key, elem_t value, entry_t *next)
{
    entry_t *entry = calloc(1, sizeof(entry_t));
    entry->key = key;
    entry->value = value;
    entry->next = next; 
    return entry;
}

static void entry_destroy(entry_t *entry)
{// NOTE: frees all entries linked to the specified one recursively
    entry_t *next;
    entry_t *cur = entry;
    while (cur != NULL)
    {
        next = cur->next;
        free(cur);
        cur = next;
    }
}

size_t string_knr_hash(elem_t str)
{
  size_t result = 0;
  while (*str.s != '\0')
  {
    result = result * 31 + ((unsigned char) *str.s);
    str.s++;
  }
  return result;
}


/*
size_t string_knr_hash(const char *str)
{
  size_t result = 0;
  while (*str != '\0')
  {
    result = result * 31 + ((unsigned char) *str);
    str++;
  }
  return result;
}
*/

bool string_equal(elem_t str1, elem_t str2) {
  return strcmp(str1.s, str2.s) != 0;
}

entry_t *find_previous_entry(ioopm_hash_table_t *ht, elem_t key)
{
    size_t bucket = ht->hash_fn(key) % No_Buckets;


    entry_t *previous = &(ht->buckets[bucket]);

    while (previous->next != NULL && ht->key_eq_fn(previous->next->key, key))
    {
        previous = previous->next;

    }

    return previous;
}

bool ioopm_hash_table_remove(ioopm_hash_table_t *ht, elem_t key, elem_t *result)
{
  // finds prev and to_be_removed
  entry_t *previous = find_previous_entry(ht, key);
  entry_t *to_be_removed = previous->next;

  if(to_be_removed == NULL) return false;

  *result = to_be_removed->value;

  // sets prev's next to to_be_removed's next
  previous->next = to_be_removed->next;
  free(to_be_removed);
  ht->size--;
  return true;
}


ioopm_hash_table_t *ioopm_hash_table_create(ioopm_hash_function *hash_fn, ioopm_eq_function *key_eq_fn) {
  ioopm_hash_table_t *ht = calloc(1, sizeof(ioopm_hash_table_t));
  ht->hash_fn = hash_fn;
  ht->key_eq_fn = key_eq_fn;
  ht->size = 0;
  return ht;
}

void ioopm_hash_table_destroy(ioopm_hash_table_t *ht) {
    // TODO: Stub
    for (int i = 0; i < No_Buckets; i++)
    {
        entry_destroy(ht->buckets[i].next);
    }
    free(ht);
    return;
}

void ioopm_hash_table_insert(ioopm_hash_table_t *ht, elem_t key, elem_t value)
{
  // find previous entry, or the last entry if the key does not exist
  entry_t *previous = find_previous_entry(ht, key);

  // if the key exists, update the value, otherwise create a new entry
  if (previous->next != NULL)
  {
    previous->next->value = value;
  }
  else
  {
    previous->next = entry_create(key, value, NULL);
    ht->size++;
  }
}

bool ioopm_hash_table_lookup(ioopm_hash_table_t *ht, elem_t key, elem_t *result)
{

  // look for an entry with the key we want
  entry_t *current = find_previous_entry(ht, key)->next;

  // if the key exists, return the value, otherwise, indicate that the lookup failed
  if (current != NULL)
  {
    *result = current->value;
    return true;
  }
  else
  {
    return false;
  }
}


bool ioopm_hash_table_has_key(ioopm_hash_table_t *ht, elem_t key)
{
  elem_t result;
  return ioopm_hash_table_lookup(ht, key, &result);
}

// REDUNDANT: manual calculation for the size of a bucket
/*
int bucket_size(entry_t *cur)
{
  int size = 0;
  while (cur != NULL)
  {
    size++;
    cur = cur->next;
  }
  return size;
}
*/
int ioopm_hash_table_size(ioopm_hash_table_t *ht)
{
 return ht->size;
}


bool ioopm_hash_table_is_empty(ioopm_hash_table_t *ht)
{
 return (ht->size == 0); // relies on size being accurate
  /*
  for (int i = 0; i < No_Buckets; i++)
  {
    if(ht->buckets[i].next != NULL) return false;
  }
  return true;
  */

}
