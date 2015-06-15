#include "php.h"
#include "jco_darray.h"
#include "ds/darray.h"

zend_class_entry *jco_darray_ce;
zend_object_handlers jco_darray_handlers;

typedef struct jco_darray {
    zend_object std;
    jco_ds_darray *array;
} jco_darray;


static void jco_darray_free_object_storage(jco_darray *intern TSRMLS_DC)
{
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

const zend_function_entry jco_darray_functions[] = {
    PHP_ME(jco_darray, __construct, arginfo_construct, ZEND_ACC_PUBLIC)
    PHP_ME(jco_darray, sayHello, arginfo_void, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

void jco_darray_init(TSRMLS_D)
{
    zend_class_entry tmp_ce;
    INIT_CLASS_ENTRY(tmp_ce, "JCO\\DArray", jco_darray_functions);

    jco_darray_ce = zend_register_internal_class(&tmp_ce TSRMLS_CC);
    jco_darray_ce->create_object = jco_darray_create_object;
    memcpy(&jco_darray_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));

    return;
}