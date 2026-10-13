#include "linked_list.h"
#include "common.h"
#include "linked_list_utils.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <assert.h>


// helper functions

static node_t *node_create_last(elem_t value)
{
    node_t *node = calloc(1,sizeof(node_t));
    node->value = value;
    node->next = NULL;
    return node;
}

static node_t *node_create(node_t *next, elem_t value)
{
    node_t *node = node_create_last(value);
    node->next = next;
    return node;
}

static node_t **node_at_index(ioopm_list_t *list, size_t index)
{
    assert(index <= list->size && "index out of boundds");
    node_t **prev = &(list->head_node);

    for (int i = 0; i < index; i++)
    {
        assert(*prev != NULL && "an entry didnt link it's next");
        prev = &((*prev)->next);
    }
    return prev;
}

// public functions

ioopm_list_t *ioopm_list_create(void)
{
    return calloc(1,sizeof(ioopm_list_t));
}



void ioopm_list_destroy(ioopm_list_t *list)
{
    node_t *next;
    node_t *cur = list->head_node;
    while (cur != NULL)
    {
        next = cur->next;
        free(cur);
        cur = next;
    }
    free(list);
    
}


void ioopm_list_append(ioopm_list_t *list, elem_t value)
{
    ioopm_list_insert(list, list->size, value);
}


void ioopm_list_prepend(ioopm_list_t *list, elem_t value)
{
    ioopm_list_insert(list, 0, value);
}



elem_t ioopm_list_head(ioopm_list_t *list)
{
    return !ioopm_list_is_empty(list) ? list->head_node->value :  (elem_t)NULL; // ternary for if list empty, returns NULL if it is
}




elem_t ioopm_list_last(ioopm_list_t *list)
{
    return ioopm_list_is_empty(list) ? (elem_t)NULL : ioopm_list_get(list, list->size-1); // ternary for if list empty, returns NULL if it is
}




void ioopm_list_insert(ioopm_list_t *list, size_t index, elem_t value)
{
    
        node_t **prev = node_at_index(list, index);
        *prev = node_create(*prev, value); // conditional stmt
        list->size++;
    
}


elem_t ioopm_list_remove(ioopm_list_t *list, size_t index)
{
    node_t **indirect = node_at_index(list, index);
    node_t *cur = *indirect;

    *indirect = cur->next;
    elem_t value = cur->value;
    free(cur);
    list->size--;
    return value;
}



elem_t ioopm_list_get(ioopm_list_t *list, size_t index)
{
    return (*node_at_index(list, index))->value;
}




size_t ioopm_list_size(ioopm_list_t *list)
{
    return list->size;
}




bool ioopm_list_is_empty(ioopm_list_t *list)
{
    return list->size == 0 && list->head_node == NULL;
}

