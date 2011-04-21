#ifndef ATIRE_PHP_EXT_H
#define ATIRE_PHP_EXT_H

#define PHP_ATIRE_EXTNAME  "atire"
#define PHP_ATIRE_EXTVER   "0.1"

extern "C" {
#include "php.h"
}

#ifdef PHP_WIN32
#include "zend_config.w32.h"
#endif

extern zend_module_entry atire_module_entry;
#define phpext_atire_ptr &atire_module_entry;

#endif /* ATIRE_PHP_EXT_H */
