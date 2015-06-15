#include "ds/darray.h"
#include "php.h"

jco_ds_darray *jco_ds_darray_create(size_t size, size_t capacity) {
    jco_ds_darray *array = emalloc(sizeof(jco_ds_darray));
    if (!array) {
        return NULL;
    }

    array->count = 0;
    array->length = 0;
    array->min_length = size;
    array->capacity = capacity;
    array->elements = NULL;


    return array;
}


void jco_ds_darray_destroy(jco_ds_darray *array) {
    if (!array) {
        return;
    }

    efree(array);
}
