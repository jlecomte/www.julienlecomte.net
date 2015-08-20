#ifndef PHP_YAHOO_YFED3_H
#define PHP_YAHOO_YFED3_H

extern "C" {

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <php.h>
#include <ext/standard/info.h>

} /* extern "C" */

extern zend_module_entry yfed3_module_entry;
#define phpext_yfed3_ptr &yfed3_module_entry;

#ifdef ZTS
#include "TSRM.h"
#endif

ZEND_BEGIN_MODULE_GLOBALS(yfed3)
    bool initialized;
    bool active;
    HashTable *registry;
    HashTable *runqueue;
ZEND_END_MODULE_GLOBALS(yfed3)

#ifdef ZTS
#define YFED3_GLOBAL(v) TSRMG(yfed3_globals_id, zend_yfed3_globals*, v)
#else
#define YFED3_GLOBAL(v) (yfed3_globals.v)
#endif

PHP_METHOD(YFedWorkflowComponent, __construct);
PHP_METHOD(YFedWorkflowManipulator, __construct);
PHP_METHOD(YFedWorkflowDispatcher, __construct);

PHP_FUNCTION(yahoo_yfed3_initialize_session);
PHP_FUNCTION(yahoo_yfed3_release_session);
PHP_FUNCTION(yahoo_yfed3_register_component);
PHP_FUNCTION(yahoo_yfed3_unregister_component);
PHP_FUNCTION(yahoo_yfed3_get_component);
PHP_FUNCTION(yahoo_yfed3_enum_components);
PHP_FUNCTION(yahoo_yfed3_execute);

PHP_MINIT_FUNCTION(yfed3);
PHP_MINFO_FUNCTION(yfed3);
PHP_GINIT_FUNCTION(yfed3);

extern zend_class_entry *yfed3_workflow_component_ce_ptr,
                        *yfed3_workflow_manipulator_ce_ptr,
                        *yfed3_workflow_dispatcher_ce_ptr;

// These are apparently needed to compile with PHP 5.2...

#ifndef Z_ADDREF_P
#define Z_ADDREF_P(pz) (pz)->refcount++
#define Z_ADDREF_PP(ppz) Z_ADDREF_P(*(ppz))
#endif

#ifndef Z_DELREF_P
#define Z_DELREF_P(pz) (pz)->refcount--
#define Z_DELREF_PP(ppz) Z_DELREF_P(*(ppz))
#endif

#ifndef zend_hash_quick_del
#define zend_hash_quick_del(ht, arKey, nKeyLength, h) \
        zend_hash_del(ht, arKey, nKeyLength)
#endif

#endif /* PHP_YAHOO_YFED3_H */
