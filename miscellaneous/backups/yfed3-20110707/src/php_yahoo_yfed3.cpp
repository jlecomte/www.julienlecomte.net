/*
 * YFed3
 *
 * A dynamic workflow processor where each component declare its dependencies.
 * The scheduler then tries to run each component in order to respect the
 * declared dependencies and while trying to parallelize as much as possible.
 *
 * @author: Julien Lecomte <jlecomte@yahoo-inc.com>
 */

#include "php_yahoo_yfed3.h"
#include "Logger.h"
#include "WorkflowScheduler.h"

ZEND_DECLARE_MODULE_GLOBALS(yfed3);

// -- arginfo -----------------------------------------------------------------

ZEND_BEGIN_ARG_INFO(arginfo_yfed3_workflow_manipulator_run, 0)
    ZEND_ARG_INFO(0, dataModel)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_yfed3_workflow_dispatcher_configure, 0)
    ZEND_ARG_INFO(0, dataModel)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_yfed3_workflow_dispatcher_extract, 0)
    ZEND_ARG_INFO(0, dataModel)
    ZEND_ARG_INFO(0, data)
ZEND_END_ARG_INFO()

// -- Function table ----------------------------------------------------------

function_entry yfed3_functions[] = {
    PHP_FE(yahoo_yfed3_initialize_session, NULL)
    PHP_FE(yahoo_yfed3_register_component, NULL)
    PHP_FE(yahoo_yfed3_unregister_component, NULL)
    PHP_FE(yahoo_yfed3_get_component, NULL)
    PHP_FE(yahoo_yfed3_enum_components, NULL)
    PHP_FE(yahoo_yfed3_execute, NULL)
    PHP_FE(yahoo_yfed3_release_session, NULL)
    {NULL, NULL, NULL}
};

function_entry yfed3_workflow_component_methods[] = {
    PHP_ME(YFedWorkflowComponent, __construct, NULL, ZEND_ACC_PROTECTED | ZEND_ACC_CTOR)
    {NULL, NULL, NULL}
};

function_entry yfed3_workflow_manipulator_methods[] = {
    PHP_ME(YFedWorkflowManipulator, __construct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ABSTRACT_ME(YFedWorkflowManipulator, run, arginfo_yfed3_workflow_manipulator_run)
    {NULL, NULL, NULL}
};

function_entry yfed3_workflow_dispatcher_methods[] = {
    PHP_ME(YFedWorkflowDispatcher, __construct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ABSTRACT_ME(YFedWorkflowDispatcher, configure, arginfo_yfed3_workflow_dispatcher_configure)
    PHP_ABSTRACT_ME(YFedWorkflowDispatcher, extract, arginfo_yfed3_workflow_dispatcher_extract)
    {NULL, NULL, NULL}
};

// -- Module struct -----------------------------------------------------------

zend_module_entry yfed3_module_entry = {
    STANDARD_MODULE_HEADER,
    "yfed3",
    yfed3_functions,
    PHP_MINIT(yfed3),
    NULL, /* MSHUTDOWN */
    NULL, /* RINIT */
    NULL, /* RSHUTDOWN */
    PHP_MINFO(yfed3),
    NO_VERSION_YET,
    PHP_MODULE_GLOBALS(yfed3),
    PHP_GINIT(yfed3),
    NULL, /* GSHUTDOWN */
    NULL, /* Post deactivate function? */
    STANDARD_MODULE_PROPERTIES_EX
};

#ifdef COMPILE_DL_YAHOO_YFED3
extern "C" {
ZEND_GET_MODULE(yfed3)
}
#endif

// -- Class entry pointers ----------------------------------------------------

zend_class_entry *yfed3_workflow_component_ce_ptr,
                 *yfed3_workflow_manipulator_ce_ptr,
                 *yfed3_workflow_dispatcher_ce_ptr;

// -- Components interfaces ---------------------------------------------------

// proto YFedWorkflowComponent::__construct() [protected]
PHP_METHOD(YFedWorkflowComponent, __construct) {}

// proto YFedWorkflowManipulator::__construct() [public]
PHP_METHOD(YFedWorkflowManipulator, __construct) {}

// proto YFedWorkflowDispatcher::__construct() [public]
PHP_METHOD(YFedWorkflowDispatcher, __construct) {}

// -- Functions available from user space -------------------------------------

PHP_FUNCTION(yahoo_yfed3_initialize_session)
{
    if (YFED3_GLOBAL(initialized)) {
        RETURN_TRUE;
    }

    INIT_LOGGER("/home/y/share/yfed3/Logger.conf");

    DEFINE_LOGGER("yfed3.yahoo_yfed3_initialize_session");

    HashTable *registry, *runqueue;

    ALLOC_HASHTABLE(registry);
    ALLOC_HASHTABLE(runqueue);

    if (!registry || !runqueue) {
        ERROR_STREAM() << "emalloc() failed unexpectedly" << FSEP_LOGGER_FLUSH;
        RETURN_FALSE;
    }

    if (zend_hash_init(registry, 256, NULL, ZVAL_PTR_DTOR, 0) != SUCCESS ||
        zend_hash_init(runqueue, 256, NULL, ZVAL_PTR_DTOR, 0) != SUCCESS) {
        ERROR_STREAM() << "zend_hash_init() failed unexpectedly" << FSEP_LOGGER_FLUSH;
        FREE_HASHTABLE(registry);
        FREE_HASHTABLE(runqueue);
        RETURN_FALSE;
    }

    YFED3_GLOBAL(registry) = registry;
    YFED3_GLOBAL(runqueue) = runqueue;
    YFED3_GLOBAL(initialized) = true;

    RETURN_TRUE;
}

PHP_FUNCTION(yahoo_yfed3_release_session)
{
    DEFINE_LOGGER("yfed3.yahoo_yfed3_release_session");

    if (!YFED3_GLOBAL(initialized)) {
        RETURN_TRUE;
    }

    if (YFED3_GLOBAL(active)) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "yahoo_yfed3_release_session() is invoked while the workflow scheduler is still active");
        RETURN_FALSE;
    }

    HashTable *registry = YFED3_GLOBAL(registry);
    HashTable *runqueue = YFED3_GLOBAL(runqueue);

    zend_hash_destroy(runqueue);
    zend_hash_destroy(registry);

    FREE_HASHTABLE(runqueue);
    FREE_HASHTABLE(registry);

    YFED3_GLOBAL(registry) = NULL;
    YFED3_GLOBAL(runqueue) = NULL;
    YFED3_GLOBAL(initialized) = false;

    RETURN_TRUE;
}

PHP_FUNCTION(yahoo_yfed3_register_component)
{
    DEFINE_LOGGER("yfed3.yahoo_yfed3_register_component");

    if (!YFED3_GLOBAL(initialized)) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "yahoo_yfed3_register_component() is invoked but the yfed3 extension is not initialized");
        RETURN_FALSE;
    }

    zval *object;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O", &object, yfed3_workflow_component_ce_ptr) != SUCCESS) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "yahoo_yfed3_register_component() expects an object of type YFedWorkflowComponent as parameter");
        RETURN_FALSE;
    }

    zval *pzName = zend_read_property(Z_OBJCE_P(object), object, "name", strlen("name"), 1 TSRMLS_CC);

    if (Z_TYPE_P(pzName) != IS_STRING) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "The 'name' attribute is expected to be a string");
        RETURN_FALSE;
    }

    char *name = Z_STRVAL_P(pzName);
    uint name_length = Z_STRLEN_P(pzName);
    HashTable *registry = YFED3_GLOBAL(registry);
    HashTable *runqueue = YFED3_GLOBAL(runqueue);
    ulong hashval = zend_get_hash_value(name, name_length+1);

    if (zend_hash_quick_exists(registry, name, name_length+1, hashval)) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "An attempt is made to re-register a component (%s)", name);
        RETURN_FALSE;
    }

    if (zend_hash_quick_add(registry, name, name_length+1, hashval, &object, sizeof(zval*), NULL) != SUCCESS) {
        php_error_docref(NULL TSRMLS_CC, E_ERROR, "yahoo_yfed3_register_component() failed unexpectedly");
        RETURN_FALSE;
    }

    Z_ADDREF_P(object);

    if (zend_hash_quick_add(runqueue, name, name_length+1, hashval, &object, sizeof(zval*), NULL) != SUCCESS) {
        php_error_docref(NULL TSRMLS_CC, E_ERROR, "yahoo_yfed3_register_component() failed unexpectedly");
        RETURN_FALSE;
    }

    Z_ADDREF_P(object);

    RETURN_TRUE;
}

PHP_FUNCTION(yahoo_yfed3_unregister_component)
{
    DEFINE_LOGGER("yfed3.yahoo_yfed3_unregister_component");

    if (!YFED3_GLOBAL(initialized)) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "yahoo_yfed3_unregister_component() is invoked but the yfed3 extension is not initialized");
        RETURN_FALSE;
    }

    char *name;
    uint name_length;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &name_length) != SUCCESS) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "yahoo_yfed3_unregister_component() expects a string as parameter");
        RETURN_FALSE;
    }

    zval *object;

    HashTable *registry = YFED3_GLOBAL(registry);
    HashTable *runqueue = YFED3_GLOBAL(runqueue);
    ulong hashval = zend_get_hash_value(name, name_length+1);

    if (zend_hash_quick_find(registry, name, name_length+1, hashval, (void**)&object) != SUCCESS) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "An attempt is made to unregister a component that apparently is never registered");
        RETURN_FALSE;
    }

    if (zend_hash_quick_del(registry, name, name_length+1, hashval) != SUCCESS) {
        php_error_docref(NULL TSRMLS_CC, E_ERROR, "yahoo_yfed3_unregister_component() failed unexpectedly");
        RETURN_FALSE;
    }

    if (zend_hash_quick_del(runqueue, name, name_length+1, hashval) != SUCCESS) {
        php_error_docref(NULL TSRMLS_CC, E_ERROR, "yahoo_yfed3_unregister_component() failed unexpectedly");
        RETURN_FALSE;
    }

    RETURN_TRUE;
}

PHP_FUNCTION(yahoo_yfed3_get_component)
{
    DEFINE_LOGGER("yfed3.yahoo_yfed3_get_component");

    if (!YFED3_GLOBAL(initialized)) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "yahoo_yfed3_get_component() is invoked but the yfed3 extension is not initialized");
        RETURN_FALSE;
    }

    char *name;
    uint name_length;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &name_length) != SUCCESS) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "yahoo_yfed3_get_component() expects a string as parameter");
        RETURN_FALSE;
    }

    zval *object, **object_ptr;
    HashTable *registry = YFED3_GLOBAL(registry);

    if (zend_hash_find(registry, name, name_length+1, (void**)&object_ptr) != SUCCESS) {
        DEBUG_STREAM() << "The component named '" << name << "' is not currently registered" << FSEP_LOGGER_FLUSH;
        RETURN_NULL();
    }

    object = *object_ptr;

    RETURN_ZVAL(object, 1, 0);
}

PHP_FUNCTION(yahoo_yfed3_enum_components)
{
    DEFINE_LOGGER("yfed3.yahoo_yfed3_enum_components");

    if (!YFED3_GLOBAL(initialized)) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "yahoo_yfed3_enum_components() is invoked but the yfed3 extension is not initialized");
        RETURN_NULL();
    }

    HashTable *registry = YFED3_GLOBAL(registry);

    array_init(return_value);
    zend_hash_copy(Z_ARRVAL_P(return_value), registry, (copy_ctor_func_t) zval_add_ref, NULL, sizeof(zval*));
}

PHP_FUNCTION(yahoo_yfed3_execute)
{
    DEFINE_LOGGER("yfed3.yahoo_yfed3_execute");

    if (!YFED3_GLOBAL(initialized)) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "yahoo_yfed3_execute() is invoked but the yfed3 extension is not initialized");
        RETURN_FALSE;
    }

    if (YFED3_GLOBAL(active)) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "yahoo_yfed3_execute() is invoked while the scheduler is already running");
        RETURN_FALSE;
    }

    zval *datamodel;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o", &datamodel) != SUCCESS) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "yahoo_yfed3_initialize_session() expects an object as parameter");
        RETURN_FALSE;
    }

    HashTable *registry = YFED3_GLOBAL(registry);
    HashTable *runqueue = YFED3_GLOBAL(runqueue);

    YFED3_GLOBAL(active) = true;

    WorkflowScheduler *sched = new WorkflowScheduler(registry, runqueue, datamodel);
    bool ret = sched->run();
    delete sched;

    YFED3_GLOBAL(active) = false;

    RETURN_BOOL(ret);
}

// -- Extension entry points --------------------------------------------------

PHP_MINIT_FUNCTION(yfed3)
{
    zend_class_entry ce;

    // Create class YFedWorkflowComponent.

    INIT_CLASS_ENTRY(ce, "YFedWorkflowComponent", yfed3_workflow_component_methods);
    yfed3_workflow_component_ce_ptr = zend_register_internal_class(&ce TSRMLS_CC);

    // Add properties to the newly created YFedWorkflowComponent class.

    // public $name = '';
    zend_declare_property_string(yfed3_workflow_component_ce_ptr,
        "name", strlen("name"), "", ZEND_ACC_PUBLIC TSRMLS_CC);

    // public $dependencies = null;
    zend_declare_property_null(yfed3_workflow_component_ce_ptr,
        "dependencies", strlen("dependencies"), ZEND_ACC_PUBLIC TSRMLS_CC);

    // public $hasRun = null;
    zend_declare_property_bool(yfed3_workflow_component_ce_ptr,
        "hasRun", strlen("hasRun"), 0, ZEND_ACC_PUBLIC TSRMLS_CC);

    // Create class YFedWorkflowManipulator as inheriting from YFedWorkflowComponent.

    INIT_CLASS_ENTRY(ce, "YFedWorkflowManipulator", yfed3_workflow_manipulator_methods);
    yfed3_workflow_manipulator_ce_ptr = zend_register_internal_class_ex(&ce,
        yfed3_workflow_component_ce_ptr, NULL TSRMLS_CC);

    // Create class YFedWorkflowDispatcher as inheriting from YFedWorkflowComponent.

    INIT_CLASS_ENTRY(ce, "YFedWorkflowDispatcher", yfed3_workflow_dispatcher_methods);
    yfed3_workflow_dispatcher_ce_ptr = zend_register_internal_class_ex(&ce,
        yfed3_workflow_component_ce_ptr, NULL TSRMLS_CC);

    return SUCCESS;
}

PHP_MINFO_FUNCTION(yfed3)
{
    php_info_print_table_start();
    php_info_print_table_header(2, "yahoo_yfed3 support", "enabled");
    php_info_print_table_end();
}

PHP_GINIT_FUNCTION(yfed3)
{
    // Note: For a reason I can't quite grasp, the compiler barfs
    // if I use the YFED3_GLOBAL macro from within this function...
    yfed3_globals->initialized = false;
    yfed3_globals->active = false;
    yfed3_globals->registry = NULL;
    yfed3_globals->runqueue = NULL;
}
