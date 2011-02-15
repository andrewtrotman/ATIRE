#ifndef PHP_ATIRE_H
#define PHP_ATIRE_H

#define PHP_ATIRE_EXTNAME  "atire"
#define PHP_ATIRE_EXTVER   "0.1"

extern "C" {
#include "php.h"
}

extern zend_module_entry atire_module_entry;
#define phpext_atire_ptr &atire_module_entry;

#endif /* PHP_ATIRE_H */
