#include "php_yahoo_yfed3.h"
#include "WorkflowScheduler.h"
#include "utils.h"
#include "Logger.h"

#include <assert.h>

// -- Functions used by zend_hash_apply_with_arguments ------------------------

static int configure_dispatcher_callback(zval **ppzvDispatcher TSRMLS_DC, int num_args, va_list args, zend_hash_key *key)
{
    DEFINE_LOGGER("yfed3.configure_dispatcher_callback");

    zval *pzvDispatcher = *ppzvDispatcher;
    WorkflowScheduler *sched = va_arg(args, WorkflowScheduler*);
    bool *success = va_arg(args, bool*);
    assert(key->nKeyLength);
    char *name = key->arKey;
    uint nameLen = key->nKeyLength;

    return sched->configureDispatcher(pzvDispatcher, name, nameLen, success);
}

static int run_manipulator_callback(zval **ppzvManipulator TSRMLS_DC, int num_args, va_list args, zend_hash_key *key)
{
    DEFINE_LOGGER("yfed3.run_manipulator_callback");

    zval *pzvManipulator = *ppzvManipulator;
    WorkflowScheduler *sched = va_arg(args, WorkflowScheduler*);
    bool *success = va_arg(args, bool*);
    assert(key->nKeyLength);
    char *name = key->arKey;
    uint nameLen = key->nKeyLength;

    return sched->runManipulator(pzvManipulator, name, nameLen, success);
}

// -- WorkflowScheduler implementation ----------------------------------------

WorkflowScheduler::WorkflowScheduler(HashTable *registry, HashTable *runqueue, zval *datamodel):
    m_registry(registry),
    m_runqueue(runqueue),
    m_datamodel(datamodel),
    m_active(false)
{
    DEFINE_LOGGER("yfed3.WorkflowScheduler.constructor");

    m_endCvar = st_cond_new();
}

WorkflowScheduler::~WorkflowScheduler()
{
    DEFINE_LOGGER("yfed3.WorkflowScheduler.destructor");

    st_cond_destroy(m_endCvar);
}

bool WorkflowScheduler::isActive()
{
    DEFINE_LOGGER("yfed3.WorkflowScheduler.isActive");

    return m_active;
}

bool WorkflowScheduler::checkDependencies(zval *pzvComponent, char *name, uint nameLen)
{
    DEFINE_LOGGER("yfed3.WorkflowScheduler.checkDependencies");

    zval *pzvDependencies = zend_read_property(yfed3_workflow_component_ce_ptr,
        pzvComponent, "dependencies", strlen("dependencies"), 1 TSRMLS_CC);

    if (Z_TYPE_P(pzvDependencies) == IS_NULL) {
        return true;
    } else if (Z_TYPE_P(pzvDependencies) != IS_ARRAY) {
        ERROR_STREAM() << "Component '" << name << "' has a $dependencies attribute that does not seem to be an array" << FSEP_LOGGER_FLUSH;
        return false;
    }

    zval **ppzvDependency, *pzvDependency;
    HashTable *dependencies = Z_ARRVAL_P(pzvDependencies);

    FOREACH(dependencies, ppzvDependency) {

        pzvDependency = *ppzvDependency;

        if (Z_TYPE_P(pzvDependency) != IS_STRING) {
            ERROR_STREAM() << "Component '" << name << "' has a listed dependency that does not appear to be a string" << FSEP_LOGGER_FLUSH;
            return false;
        }

        char *dependency = Z_STRVAL_P(pzvDependency);

        if (m_completed.count(dependency) == 0) {
            return false;
        }
    }

    return true;
}

int WorkflowScheduler::configureDispatcher(zval *pzvDispatcher, char *name, uint nameLen, bool *success)
{
    DEFINE_LOGGER("yfed3.WorkflowScheduler.configureDispatcher");

    if (!instanceof_function(Z_OBJCE_P(pzvDispatcher), yfed3_workflow_dispatcher_ce_ptr TSRMLS_CC)) {
        return ZEND_HASH_APPLY_KEEP;
    }

    if (!checkDependencies(pzvDispatcher, name, nameLen)) {
        DEBUG_STREAM() << "'" << name << "' has dependencies that prevent it from running" << FSEP_LOGGER_FLUSH;
        return ZEND_HASH_APPLY_KEEP;
    }

    int ret;
    zval funcname, retval;
    HashTable *config;
    AsyncDispatcher *dispatcher = NULL;

    INIT_ZVAL(funcname);
    INIT_ZVAL(retval);

    ZVAL_STRING(&funcname, "configure", 1);

    DEBUG_STREAM() << "Configuring dispatcher '" << name << "'" << FSEP_LOGGER_FLUSH;
    ret = call_user_function(EG(function_table), &pzvDispatcher, &funcname, &retval, 1, &m_datamodel TSRMLS_CC);

    if (ret != SUCCESS) {
        ERROR_STREAM() << "call_user_function() failed unexpectedly" << FSEP_LOGGER_FLUSH;
        goto finish;
    }

    if (Z_TYPE(retval) != IS_ARRAY) {
        ERROR_STREAM() << "YFedWorkflowDispatcher::configure must return an array" << FSEP_LOGGER_FLUSH;
        goto finish;
    }

    config = Z_ARRVAL(retval);

    // Create a new AsyncDispatcher object...

    dispatcher = new AsyncDispatcher(pzvDispatcher, name, nameLen, config, m_endCvar);

    // ...add it to the wait queue...

    m_waitqueue.push_back(dispatcher);

    // ...and send the request! This method starts a state thread and returns
    // before the request completes. This means that we have to delete the
    // dispatcher object at a later time...

    if (!dispatcher->start()) {
        ERROR_STREAM() << "AsyncDispatcher::start() failed" << FSEP_LOGGER_FLUSH;
        goto finish;
    }

    *success = true;

finish:

    zval_dtor(&funcname);
    zval_dtor(&retval);

    if (!(*success) && dispatcher) {
        m_waitqueue.pop_back();
        delete dispatcher;
    }

    return ZEND_HASH_APPLY_REMOVE;
}

bool WorkflowScheduler::extractDispatcher(AsyncDispatcher *dispatcher)
{
    DEFINE_LOGGER("yfed3.WorkflowScheduler.extractDispatcher");

    // Invoke the extract method on the dispatcher.

    zval funcname, retval, *param[2];
    ResponseBuffer *responseBuffer = dispatcher->getResponseBuffer();
    zval *pzvDispatcher = dispatcher->getDispatcher();
    string name = dispatcher->getDispatcherName();

    INIT_ZVAL(funcname);
    INIT_ZVAL(retval);
    MAKE_STD_ZVAL(param[0]);
    MAKE_STD_ZVAL(param[1]);

    ZVAL_STRING(&funcname, "extract", 1);
    ZVAL_ZVAL(param[0], m_datamodel, 1, 0);
    ZVAL_STRINGL(param[1], responseBuffer->buf, responseBuffer->bufLen, 1);

    DEBUG_STREAM() << "Extracting data for dispatcher '" << name << "'" << FSEP_LOGGER_FLUSH;

    int ret = call_user_function(EG(function_table), &pzvDispatcher, &funcname, &retval, 2, (zval**)&param TSRMLS_CC);

    zval_dtor(&funcname);
    zval_dtor(&retval);
    zval_ptr_dtor(&param[0]);
    zval_ptr_dtor(&param[1]);

    if (ret != SUCCESS) {
        ERROR_STREAM() << "call_user_function() failed unexpectedly" << FSEP_LOGGER_FLUSH;
        return false;
    }

    m_completed.insert(name);
    return true;
}

int WorkflowScheduler::runManipulator(zval *pzvManipulator, char *name, uint nameLen, bool *success)
{
    DEFINE_LOGGER("yfed3.WorkflowScheduler.runManipulator");

    if (!instanceof_function(Z_OBJCE_P(pzvManipulator), yfed3_workflow_manipulator_ce_ptr TSRMLS_CC)) {
        return ZEND_HASH_APPLY_KEEP;
    }

    if (!checkDependencies(pzvManipulator, name, nameLen)) {
        DEBUG_STREAM() << "'" << name << "' has dependencies that prevent it from running" << FSEP_LOGGER_FLUSH;
        return ZEND_HASH_APPLY_KEEP;
    }

    zval funcname, retval;
    int ret;

    INIT_ZVAL(funcname);
    INIT_ZVAL(retval);
    ZVAL_STRING(&funcname, "run", 1);

    DEBUG_STREAM() << "Running manipulator '" << name << "'" << FSEP_LOGGER_FLUSH;
    ret = call_user_function(EG(function_table), &pzvManipulator, &funcname, &retval, 1, &m_datamodel TSRMLS_CC);

    zval_dtor(&funcname);
    zval_dtor(&retval);

    if (ret != SUCCESS) {
        ERROR_STREAM() << "call_user_function() failed unexpectedly" << FSEP_LOGGER_FLUSH;
        return ZEND_HASH_APPLY_REMOVE;
    }

    *success = true;
    m_completed.insert(name);
    return ZEND_HASH_APPLY_REMOVE | ZEND_HASH_APPLY_STOP;
}

bool WorkflowScheduler::run()
{
    DEFINE_LOGGER("yfed3.WorkflowScheduler.run");

    if (m_active) {
        WARN_STREAM() << "WorkflowScheduler::run() is not reentrant" << FSEP_LOGGER_FLUSH;
        return false;
    }

    if (st_init() < 0) {
        ERROR_STREAM() << "st_init() failed unexpectedly (" << strerror(errno) << ")" << FSEP_LOGGER_FLUSH;
        return false;
    }

    bool b;

start:

    do {

        b = false;

        // If there are any dispatchers w/o any dependencies left, remove them
        // from the run queue, configure them and start a thread to handle the
        // HTTP request asynchronously.

        zend_hash_apply_with_arguments(m_runqueue TSRMLS_CC, (apply_func_args_t) &configure_dispatcher_callback, 2, this, &b);

        // If there are any manipulators w/o any dependencies left, remove them
        // from the run queue, execute them and adjust the dependencies on all
        // remaining workflow components.

        zend_hash_apply_with_arguments(m_runqueue TSRMLS_CC, (apply_func_args_t) &run_manipulator_callback, 2, this, &b);

        if (b) {
            // Since dispatchers were configured, give them a chance to run.
            st_sleep(0);
        }

    } while (b);

    // We've executed everything we could at this point. Are we done?

    if (m_waitqueue.empty()) {

        if (EMPTY(m_runqueue)) {
            DEBUG_STREAM() << "Workflow execution complete" << FSEP_LOGGER_FLUSH;
            return true;
        }

        ERROR_STREAM() << "Unable the complete the workflow execution. Check for circular dependencies or dependencies that cannot be satisfied." << FSEP_LOGGER_FLUSH;

        // TODO: dump the runqueue...

        return false;

    } else {

        // Wait until a request completes.
        // TODO: Should this be a timed wait?
        st_cond_wait(m_endCvar);

        // Figure out which dispatcher just completed.
        vector<AsyncDispatcher*>::iterator it;
        AsyncDispatcher *dispatcher;
        for (it = m_waitqueue.begin(); it < m_waitqueue.end(); it++) {
            dispatcher = (AsyncDispatcher*)*it;
            if (dispatcher->getState() == STOPPED) {
                this->extractDispatcher(dispatcher);
                m_waitqueue.erase(it);
                delete dispatcher;
            }
        }

        // Since the recently completed dispatcher might have unblocked other
        // components, jump back to the beginning.
        goto start;
    }

    /* NOT REACHED */
    return false;
}
