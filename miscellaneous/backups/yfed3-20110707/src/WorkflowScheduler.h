#ifndef WORKFLOW_SCHEDULER_H
#define WORKFLOW_SCHEDULER_H

extern "C" {
#include <php.h>
} /* extern "C" */

#include "AsyncDispatcher.h"

using namespace std;

#include <string>
#include <vector>
#include <set>

class WorkflowScheduler
{
    protected:

        // List of all registered components.
        HashTable *m_registry;

        // List of components that have not yet completed their execution.
        HashTable *m_runqueue;

        // PHP user space variable representing the global data model object.
        zval *m_datamodel;

        // Name of components that have completed their execution.
        set<string> m_completed;

        // List of pending dispatchers.
        vector<AsyncDispatcher*> m_waitqueue;

        // Indicates whether the scheduler is currently running.
        bool m_active;

        // The condition variable that is signalled when a dispatcher completes
        // its execution.
        st_cond_t m_endCvar;

    public:

        WorkflowScheduler(HashTable *registry, HashTable *runqueue, zval *datamodel);
        virtual ~WorkflowScheduler();
        bool isActive();
        bool checkDependencies(zval *pzvComponent, char *name, uint nameLen);
        int configureDispatcher(zval *pzvDispatcher, char *name, uint nameLen, bool *success);
        bool extractDispatcher(AsyncDispatcher *dispatcher);
        int runManipulator(zval *pzvManipulator, char *name, uint nameLen, bool *success);
        bool run();
};

#endif /* WORKFLOW_SCHEDULER_H */
