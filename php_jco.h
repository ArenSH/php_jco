#ifndef PHP_JCOLLECTIONS_H
#define PHP_JCOLLECTIONS_H 1

#define PHP_JCO_VERSION "0.1"
#define PHP_JCO_EXTNAME "jco"

extern zend_module_entry jco_module_entry;
#define phpext_jco_ptr &jco_module_entry

#ifdef PHP_WIN32
# define PHP_DARRAY_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
# define PHP_DARRAY_API __attribute__ ((visibility("default")))
#else
# define PHP_DARRAY_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

#endif

