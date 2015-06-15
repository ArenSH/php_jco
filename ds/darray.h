#ifndef PHP_JCO_DS_DARRAY_H
#define PHP_JCO_DS_DARRAY_H 1

#include "php.h"

typedef  struct jco_ds_darray {
	size_t count;
	size_t length;
	size_t min_length;
	size_t capacity;
    void *elements;
} jco_ds_darray;

jco_ds_darray *jco_ds_darray_create(size_t size, size_t capacity);
void jco_ds_darray_destroy(jco_ds_darray *array);

#define jco_ds_darray_length(array) ((array)->length)
#define jco_ds_darray_min_length(array) ((array)->min_length)
#define jco_ds_darray_capacity(array) ((array)->capacity)
#define jco_ds_darray_count(array) ((array)->count)
#define jco_ds_darray_first(array) ((zval *)(array)->elements)

#endif
