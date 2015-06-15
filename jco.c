
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_jco.h"

PHP_FUNCTION(hello_from_jco)
{
	RETURN_STRING("JCO ENABLED! YEY!", 1);
}

const zend_function_entry jco_functions[] = {
	PHP_FE(hello_from_jco,	NULL)	
    PHP_FE_END
};

PHP_MINIT_FUNCTION(jco_init)
{
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