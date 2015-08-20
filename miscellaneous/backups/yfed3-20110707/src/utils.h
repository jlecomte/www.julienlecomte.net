#ifndef UTILS_H
#define UTILS_H

#define FOREACH(ht, data)                                           \
    for (zend_hash_internal_pointer_reset(ht);                      \
         zend_hash_get_current_data(ht, (void**)&data) == SUCCESS; \
         zend_hash_move_forward(ht))

#define EMPTY(ht) \
    (zend_hash_num_elements(ht) == 0)

#define FREE_AND_NIL(p) \
    do {                \
        if (p) {        \
            efree(p);   \
            p = NULL;   \
        }               \
    } while(0)

#endif /* UTILS_H */
