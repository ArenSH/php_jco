#include "php.h"
#include "jco_darray.h"

zend_class_entry *jco_darray_ce;

PHP_METHOD(jco_darray, sayHello)
{
    RETURN_STRING("Hello from darray!", 1);
}


ZEND_BEGIN_ARG_INFO_EX(arginfo_void, 0, 0, 0)
ZEND_END_ARG_INFO()

const zend_function_entry jco_darray_functions[] = {
    PHP_ME(jco_darray, sayHello, arginfo_void, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

void jco_darray_init(TSRMLS_D)
{
    zend_class_entry tmp_ce;
    INIT_CLASS_ENTRY(tmp_ce, "JCO\\DArray", jco_darray_functions);

    jco_darray_ce = zend_register_internal_class(&tmp_ce TSRMLS_CC);

    return;
}