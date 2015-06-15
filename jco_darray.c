#include "php.h"
#include "zend_interfaces.h"
#include "jco_darray.h"
#include "ds/darray.h"

zend_class_entry *jco_darray_ce;
zend_object_handlers jco_darray_handlers;

typedef struct jco_darray {
    zend_object std;
    jco_ds_darray *array;
} jco_darray;


static inline long zval_to_long(zval *zv) {
    if (Z_TYPE_P(zv) == IS_LONG) {
        return Z_LVAL_P(zv);
    } else {
        zval tmp = *zv;
        zval_copy_ctor(&tmp);
        convert_to_long(&tmp);
        return Z_LVAL(tmp);
    }
}

static void jco_darray_free_object_storage(jco_darray *intern TSRMLS_DC) {
    zend_object_std_dtor(&intern->std TSRMLS_CC);

    if (intern->array) {
        jco_ds_darray_destroy(intern->array);
    }

    efree(intern);
}

zend_object_value jco_darray_create_object(zend_class_entry *class_type TSRMLS_DC) {
    zend_object_value retval;

    jco_darray *intern = emalloc(sizeof(jco_darray));
    memset(intern, 0, sizeof(jco_darray));

    zend_object_std_init(&intern->std, class_type TSRMLS_CC);
    object_properties_init(&intern->std, class_type);

    retval.handle = zend_objects_store_put(
        intern,
        (zend_objects_store_dtor_t) zend_objects_destroy_object,
        (zend_objects_free_object_storage_t) jco_darray_free_object_storage,
        NULL TSRMLS_CC
    );

    retval.handlers = &jco_darray_handlers;

    return retval;
}


static zend_object_value jco_darray_clone(zval *object TSRMLS_DC) {
    jco_darray *old_object = zend_object_store_get_object(object TSRMLS_CC);

    zend_object_value new_object_val = jco_darray_create_object(Z_OBJCE_P(object) TSRMLS_CC);
    jco_darray *new_object = zend_object_store_get_object_by_handle(new_object_val.handle TSRMLS_CC);

    zend_objects_clone_members(
        &new_object->std, new_object_val,
        &old_object->std, Z_OBJ_HANDLE_P(object) TSRMLS_CC
    );

    new_object->array = jco_ds_darray_clone(old_object->array);

    if (!new_object->array) {
        zend_throw_exception(NULL, "Failed to clone jco_darray", 0 TSRMLS_CC);
    }

    return new_object_val;
}

static zval *jco_darray_read_dimension(zval *object, zval *zv_offset, int type TSRMLS_DC) {
    jco_darray *intern = zend_object_store_get_object(object TSRMLS_CC);
    
    if (intern->std.ce->parent) {
        return zend_get_std_object_handlers()->read_dimension(object, zv_offset, type TSRMLS_CC);
    }

    if (!zv_offset) {
        zend_throw_exception(NULL, "Cannot append to a jco_darray", 0 TSRMLS_CC);
        return NULL;
    }

    long offset = zval_to_long(zv_offset);
    if (offset < 0 || offset > jco_ds_darray_length(intern->array)) {
        zend_throw_exception(NULL, "Offset out of range", 0 TSRMLS_CC);
        return NULL;
    }

    zval *return_value;
    zval *value = jco_ds_darray_get(intern->array, offset);

    if (value) {

        if (type == BP_VAR_W) {
            return_value = value;
            Z_SET_ISREF_P(return_value);
        } else {
            MAKE_STD_ZVAL(return_value);
            ZVAL_ZVAL(return_value, value, 1, 0);
            Z_DELREF_P(return_value);
        }
    } else {
        MAKE_STD_ZVAL(return_value);
        ZVAL_NULL(return_value);
        Z_DELREF_P(return_value);
    }

    return return_value;
}


static void jco_darray_write_dimension(zval *object, zval *zv_offset, zval *value TSRMLS_DC) {
    jco_darray *intern = zend_object_store_get_object(object TSRMLS_CC);

    if (intern->std.ce->parent) {
        return zend_get_std_object_handlers()->write_dimension(object, zv_offset, value TSRMLS_CC);
    }


    if (!zv_offset) {
        zend_throw_exception(NULL, "Cannot append to a jco_darray", 0 TSRMLS_CC);
    }

    long offset = zval_to_long(zv_offset);
    if (offset < 0) {
        zend_throw_exception(NULL, "Offset out of range", 0 TSRMLS_CC);
    }

    zval *saved_val = jco_ds_darray_set(intern->array, (size_t)offset, value);
    if (saved_val == NULL) {
        zend_throw_exception(NULL, "Error occured during dimension write", 0 TSRMLS_CC);
    }
}


static int jco_darray_has_dimension(zval *object, zval *zv_offset, int check_empty TSRMLS_DC) {
    jco_darray *intern = zend_object_store_get_object(object TSRMLS_CC);

    if (intern->std.ce->parent) {
        return zend_get_std_object_handlers()->has_dimension(object, zv_offset, check_empty TSRMLS_CC);
    }

    long offset = zval_to_long(zv_offset);
    if (offset < 0 || offset > jco_ds_darray_length(intern->array)) {
        return 0;
    }

    if (check_empty) {
        zval *value = jco_ds_darray_get(intern->array, offset);
        if (value == NULL) {
            return 0;
        }

        return zend_is_true(value);
    }

}

static void jco_darray_unset_dimension(zval *object, zval *zv_offset TSRMLS_DC) {
    jco_darray *intern = zend_object_store_get_object(object TSRMLS_CC);

    if (intern->std.ce->parent) {
        return zend_get_std_object_handlers()->unset_dimension(object, zv_offset TSRMLS_CC);
    }

    long offset = zval_to_long(zv_offset);
    if (offset < 0 || offset > jco_ds_darray_length(intern->array)) {
        zend_throw_exception(NULL, "Offset out of range", 0 TSRMLS_CC);
    }

    jco_ds_darray_unset(intern->array, offset);
}

int jco_darray_count_elements(zval *object, long *count TSRMLS_DC) {
    jco_darray *intern = zend_object_store_get_object(object TSRMLS_CC);

    if (intern->std.ce->parent) {
        return zend_get_std_object_handlers()->count_elements(object, count TSRMLS_CC);
    }

    if (intern && intern->array) {
        *count = (long)jco_ds_darray_count(intern->array);
        return SUCCESS;
    } else {
        *count = 0;
        return FAILURE;
    }
}


PHP_METHOD(jco_darray, __construct)
{
    jco_darray *intern;
    long size = 0;
    long capacity = 0;

    zend_error_handling error_handling;

    zend_replace_error_handling(EH_THROW, NULL, &error_handling TSRMLS_CC);
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &size, &capacity) == FAILURE) {
        zend_restore_error_handling(&error_handling TSRMLS_CC);
        return;
    }
    zend_restore_error_handling(&error_handling TSRMLS_CC);

    if (size <= 0) {
        zend_throw_exception(NULL, "Array size must be positive", 0 TSRMLS_CC);
        return;
    }

    if (capacity < 0) {
        zend_throw_exception(NULL, "Array capacity must be positive or 0", 0 TSRMLS_CC);
        return;
    }

    intern = zend_object_store_get_object(getThis() TSRMLS_CC);

    intern->array = jco_ds_darray_create((size_t)size, (size_t)capacity);
    if (!intern->array) {
        zend_throw_exception(NULL, "Failed to allocate array", 0 TSRMLS_CC);
    }

    return;
}


PHP_METHOD(jco_darray, count)
{
    jco_darray *intern;
    long count;

    intern = zend_object_store_get_object(getThis() TSRMLS_CC);
    count = (long)jco_ds_darray_count(intern->array);

    ZVAL_LONG(return_value, count);
}

PHP_METHOD(jco_darray, length)
{
    jco_darray *intern;
    long length;

    intern = zend_object_store_get_object(getThis() TSRMLS_CC);
    length = (long) jco_ds_darray_length(intern->array);

    ZVAL_LONG(return_value, length);
}

PHP_METHOD(jco_darray, offsetSet)
{
    jco_darray *intern;
    zval *val;
    long index;


    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lz", &index, &val) == FAILURE) {
        zend_throw_exception(NULL, "Failed to parse arguments", 0 TSRMLS_CC);
        return;
    }

    intern = zend_object_store_get_object(getThis() TSRMLS_CC);
    jco_ds_darray_set(intern->array, (size_t)index, val);

}

PHP_METHOD(jco_darray, offsetUnset)
{
    jco_darray *intern;
    long index;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &index) == FAILURE) {
        zend_throw_exception(NULL, "Invalid index passed", 0 TSRMLS_CC);
        return;
    }


    intern = zend_object_store_get_object(getThis() TSRMLS_CC);
    jco_ds_darray_unset(intern->array, (size_t)index);
}

PHP_METHOD(jco_darray, offsetGet)
{
    jco_darray *intern;
    long index;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &index) == FAILURE) {
        zend_throw_exception(NULL, "Invalid index passed", 0 TSRMLS_CC);
        return;
    }


    intern = zend_object_store_get_object(getThis() TSRMLS_CC);
    zval *val = jco_ds_darray_get(intern->array, (size_t)index);

    if (val) {
        ZVAL_ZVAL(return_value, val, 1, 0);
    } else {
        ZVAL_NULL(return_value);
    }
}

PHP_METHOD(jco_darray, offsetExists)
{
    jco_darray *intern;
    long index;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &index) == FAILURE) {
        zend_throw_exception(NULL, "Invalid index passed", 0 TSRMLS_CC);
        return;
    }


    intern = zend_object_store_get_object(getThis() TSRMLS_CC);
    zval *val = jco_ds_darray_get(intern->array, (size_t)index);
    if (val) {
        ZVAL_TRUE(return_value);
    } else {
        ZVAL_FALSE(return_value);
    }
}

PHP_METHOD(jco_darray, sayHello)
{
    RETURN_STRING("Hello from darray!", 1);
}


ZEND_BEGIN_ARG_INFO_EX(arginfo_void, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_construct, 0, 0, 1)
ZEND_ARG_INFO(0, size)
ZEND_ARG_INFO(0, capacity)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_jco_darray_offset, 0, 0, 1)
ZEND_ARG_INFO(0, offset)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_jco_darray_offset_value, 0, 0, 2)
ZEND_ARG_INFO(0, offset)
ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

const zend_function_entry jco_darray_functions[] = {
    PHP_ME(jco_darray, __construct, arginfo_construct, ZEND_ACC_PUBLIC)
    PHP_ME(jco_darray, offsetSet, arginfo_jco_darray_offset_value, ZEND_ACC_PUBLIC)
    PHP_ME(jco_darray, offsetGet, arginfo_jco_darray_offset, ZEND_ACC_PUBLIC)
    PHP_ME(jco_darray, offsetUnset, arginfo_jco_darray_offset, ZEND_ACC_PUBLIC)
    PHP_ME(jco_darray, offsetExists, arginfo_jco_darray_offset, ZEND_ACC_PUBLIC)
    PHP_ME(jco_darray, count, arginfo_void, ZEND_ACC_PUBLIC)
    PHP_ME(jco_darray, length, arginfo_void, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

void jco_darray_init(TSRMLS_D)
{
    zend_class_entry tmp_ce;
    INIT_CLASS_ENTRY(tmp_ce, "JCO\\DArray", jco_darray_functions);

    jco_darray_ce = zend_register_internal_class(&tmp_ce TSRMLS_CC);
    jco_darray_ce->create_object = jco_darray_create_object;
    memcpy(&jco_darray_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));

    zend_class_implements(jco_darray_ce TSRMLS_CC, 1, zend_ce_arrayaccess);

    jco_darray_handlers.has_dimension   = jco_darray_has_dimension;
    jco_darray_handlers.read_dimension  = jco_darray_read_dimension;
    jco_darray_handlers.write_dimension = jco_darray_write_dimension;
    jco_darray_handlers.unset_dimension = jco_darray_unset_dimension;
    jco_darray_handlers.count_elements  = jco_darray_count_elements;
    jco_darray_handlers.clone_obj = jco_darray_clone;

    return;
}