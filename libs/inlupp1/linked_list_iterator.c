#include "linked_list_iterator.h"
#include "common.h"
#include "linked_list_utils.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>


typedef struct node node_t;


struct list_iterator {
    node_t *node;
    size_t index;
    ioopm_list_t *list;
};



ioopm_list_iterator_t *ioopm_list_iterator_create(ioopm_list_t *l)
{
    ioopm_list_iterator_t *it = calloc(1, sizeof(ioopm_list_iterator_t));
    it->node = l->head_node;
    it->index = 0;
    it->list = l;
    return it;
}



void ioopm_list_iterator_destroy(ioopm_list_iterator_t *iter)
{
    // ioopm_list_destroy(iter->list);
    free(iter);
}




bool ioopm_list_iterator_at_end(ioopm_list_iterator_t *iter)
{
    return (iter->node == NULL);
}



void ioopm_list_iterator_advance(ioopm_list_iterator_t *iter)
{
    iter->node = iter->node->next;
    iter->index++;
}




elem_t ioopm_list_iterator_current(ioopm_list_iterator_t *iter)
{
    return iter->node->value;
}





elem_t ioopm_list_iterator_remove(ioopm_list_iterator_t *iter)
{
    ioopm_list_iterator_advance(iter);
    
    if(ioopm_list_iterator_at_end(iter)) {
        return ioopm_list_remove(iter->list, iter->index-1);

    } else {
        iter->index = iter->index-1;
        return ioopm_list_remove(iter->list, iter->index);
    }
}





void ioopm_list_iterator_insert(ioopm_list_iterator_t *iter, elem_t element)
{
    ioopm_list_insert(iter->list, iter->index, element);
    iter->index++;
}
