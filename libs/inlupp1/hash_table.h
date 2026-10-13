#pragma once

#include "common.h"

/**
* @file hash_table.h
* @author Victor Ljungberg // write both your names here
* @date 2026-09-19 // write the date you started working on this
* @brief Simple hash table that maps string keys to integer values.
*
* Here typically goes a more extensive explanation of what the header
* defines. Doxygens tags are words preceeded by either a backslash @\
* or by an at symbol @@.
*
*/


typedef bool ioopm_eq_function(elem_t a, elem_t b);
typedef size_t ioopm_hash_function(elem_t key);

typedef struct hash_table ioopm_hash_table_t;

/// @brief Create a new hash table
/// @return A new empty hash table
ioopm_hash_table_t *ioopm_hash_table_create(ioopm_hash_function *hash_fn, ioopm_eq_function *key_eq_fn);

/// @brief Delete a hash table and free its memory
/// @param ht a hash table to be deleted
void ioopm_hash_table_destroy(ioopm_hash_table_t *ht);

/// @note if key exists then it only changes value
/// @brief add key => value mapping in hash table ht
/// @param ht hash table operated upon
/// @param key key to insert
/// @param value value to insert
void ioopm_hash_table_insert(ioopm_hash_table_t *ht, elem_t key, elem_t value);

/// @brief lookup value for key in hash table ht
/// @param ht hash table operated upon
/// @param key key to lookup
/// @param result the address were the value of the specified entry is written to
/// @return the value mapped to by key (FIXME: what if the key does not exist?)
bool ioopm_hash_table_lookup(ioopm_hash_table_t *ht, elem_t key, elem_t *result);

/// @note this only frees the elem_t not what it points to
/// @brief remove any mapping of a key if t exists
/// @param ht hash table operated upon
/// @param key key to remove
/// @param result the address were the value mapped to the specified key is written to
/// @return  whether the mapping could be removed from the hash table
bool ioopm_hash_table_remove(ioopm_hash_table_t *ht, elem_t key, elem_t *result);


///@brief checks whether a hash table has an entry with the given key
///@param ht hash table operated upon
///@param key key to check
///@return whether the hash table has an entry with the given key
bool ioopm_hash_table_has_key(ioopm_hash_table_t *ht, elem_t key);

///@note relies on ht->size being accurate
///@brief checks whether a has table is empty
///@param ht hash table operated upon
///@return if empty or not
bool ioopm_hash_table_is_empty(ioopm_hash_table_t *ht);

///@note relies on ht->size being accurate
///@brief returns the size of the given hash table
///@param ht hash table operated upon
///@return the size of the hash table
int ioopm_hash_table_size(ioopm_hash_table_t *ht);

/// @note edgecases are not assured
/// @brief a basic function of the type ioopm_eq_function
/// @param str1 a elem_t containing first string (must contain a string)
/// @param str2 a elem_t containing first string (must contain a string)
/// @return wether the strings given are equal (contain the same sequence of char(s))
bool string_equal(elem_t str1, elem_t str2);

/// @note edgecases are not assured
/// @brief a basic function of the type ioopm_hash_function
/// @param str the elem_t containing the string to hash (must contain a string)
/// @return the hash value of the input given as a size_t
size_t string_knr_hash(const elem_t str);






