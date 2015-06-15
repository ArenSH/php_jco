
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_jco.h"
#include "jco_darray.h"

const zend_function_entry jco_functions[] = {
    PHP_FE_END
};

PHP_MINIT_FUNCTION(jco_init)
{
	jco_darray_init(TSRMLS_C);
    return SUCCESS;
}


zend_module_entry jco_module_entry = {
    STANDARD_MODULE_HEADER,
    PHP_JCO_EXTNAME,
    jco_functions,
    PHP_MINIT(jco_init),
    NULL, //MSHUTDOWN
    NULL, //RINIT
    NULL, //RSHUTDOWN
    NULL, //MINFO
    PHP_JCO_VERSION,
    STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_JCO
ZEND_GET_MODULE(jco)
#endif