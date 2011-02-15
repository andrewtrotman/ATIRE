#include "php_atire.h"

#include "../source/atire_api_remote.h"

zend_object_handlers atire_api_remote_object_handlers;

struct atire_api_remote_object {
    zend_object std;
    ATIRE_API_remote *atire;
};

zend_class_entry *atire_api_remote_ce;

void atire_api_remote_free_storage(void *object TSRMLS_DC)
{
    atire_api_remote_object *obj = (atire_api_remote_object *)object;

    zend_hash_destroy(obj->std.properties);
    FREE_HASHTABLE(obj->std.properties);
    delete obj->atire;

    efree(obj);
}

zend_object_value atire_api_remote_create_handler(zend_class_entry *type TSRMLS_DC)
{
    zval *tmp;
    zend_object_value retval;

    atire_api_remote_object *obj = (atire_api_remote_object *)emalloc(sizeof(atire_api_remote_object));
    memset(obj, 0, sizeof(atire_api_remote_object));
    obj->std.ce = type;

    ALLOC_HASHTABLE(obj->std.properties);
    zend_hash_init(obj->std.properties, 0, NULL, ZVAL_PTR_DTOR, 0);
    zend_hash_copy(obj->std.properties, &type->default_properties,
        (copy_ctor_func_t)zval_add_ref, (void *)&tmp, sizeof(zval *));

    retval.handle = zend_objects_store_put(obj, NULL,
        atire_api_remote_free_storage, NULL TSRMLS_CC);
    retval.handlers = &atire_api_remote_object_handlers;

    return retval;
}


PHP_METHOD(atire_api_remote, __construct)
{
    ATIRE_API_remote *atire =  NULL;
    zval *object = getThis();

    atire = new ATIRE_API_remote();
    if(atire == NULL){
        php_printf("Object is not  made!!\n");
    }
    atire_api_remote_object *obj =(atire_api_remote_object *)zend_object_store_get_object(object TSRMLS_CC);
    obj->atire = atire;
}

PHP_METHOD(atire_api_remote, open)
{
    char *connect_string;
    int len;

    ATIRE_API_remote *atire = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &connect_string, &len) == FAILURE) {
        RETURN_NULL();
    }

    atire_api_remote_object *obj =(atire_api_remote_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    atire = obj->atire;
    if (atire != NULL){
        atire->open(connect_string);
    }
    RETURN_TRUE;
}

PHP_METHOD(atire_api_remote, close)
{
    ATIRE_API_remote *atire = NULL;
    atire_api_remote_object *obj =(atire_api_remote_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    atire = obj->atire;
    if (atire != NULL){
        atire->close();
        RETURN_TRUE;
    }else
    {
        RETURN_FALSE;
    }
}

PHP_METHOD(atire_api_remote, search)
{
    char *search_string, *res;
    int len;

    ATIRE_API_remote *atire = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &search_string, &len) == FAILURE) {
        RETURN_NULL();
    }

    atire_api_remote_object *obj =(atire_api_remote_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    atire = obj->atire;
    if (atire != NULL){
        res = atire->search(search_string,1,10);
        RETURN_STRING(res,1)
    }

}

function_entry atire_api_remote_methods[] = {
    PHP_ME(atire_api_remote,  __construct,     NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(atire_api_remote,  open,           NULL, ZEND_ACC_PUBLIC)
    PHP_ME(atire_api_remote,  close,           NULL, ZEND_ACC_PUBLIC)
    PHP_ME(atire_api_remote,  search,           NULL, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};


PHP_MINIT_FUNCTION(atire)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "atire_api_remote", atire_api_remote_methods);
    atire_api_remote_ce = zend_register_internal_class(&ce TSRMLS_CC);
    atire_api_remote_ce->create_object = atire_api_remote_create_handler;
    memcpy(&atire_api_remote_object_handlers,
        zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    atire_api_remote_object_handlers.clone_obj = NULL;
    return SUCCESS;
}

zend_module_entry atire_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
    STANDARD_MODULE_HEADER,
#endif
    PHP_ATIRE_EXTNAME,
    NULL,                  /* Functions */
    PHP_MINIT(atire),
    NULL,                  /* MSHUTDOWN */
    NULL,                  /* RINIT */
    NULL,                  /* RSHUTDOWN */
    NULL,                  /* MINFO */
#if ZEND_MODULE_API_NO >= 20010901
    PHP_ATIRE_EXTVER,
#endif
    STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_ATIRE

extern "C" {
ZEND_GET_MODULE(atire)
}
#endif
