#ifndef ASYNC_DISPATCHER_H
#define ASYNC_DISPATCHER_H

extern "C" {
#include <php.h>
} /* extern "C" */

#include <curl/curl.h>
#include <st.h>

using namespace std;

#include <string>

typedef enum {
    RUNNING,
    STOPPED
} DispatcherState;

typedef struct {
    char  *buf;
    size_t bufLen;
    size_t bufSize;
} ResponseBuffer;

class PollFdArray
{
    protected:
        int m_arraySize;

    public:
        pollfd *m_fds;
        int m_nfds;

        PollFdArray();
        ~PollFdArray();

        int build(fd_set &fdRead, fd_set &fdWrite, fd_set &fdExcep, int maxFds);
};

class AsyncDispatcher
{
    protected:

        // The corresponding PHP user-space dispatcher.
        zval *m_pzvDispatcher;
        string m_name;

        // Current state.
        DispatcherState m_state;

        // The dispatcher's configuration.
        HashTable *m_config;

        // State thread and synchronization variable.
        st_thread_t m_tid;
        st_cond_t m_endCvar;

        // Our cUrl handles.
        CURLM *m_cmh;
        CURL  *m_ch;

        // Request configuration.
        string m_url;
        // ...

        // The response buffer.
        ResponseBuffer m_response;

        // Response status.
        long m_httpstatus;

        void cleanup();

    public:

        AsyncDispatcher(zval *pzvDispatcher, char *name, uint nameLen, HashTable *config, st_cond_t endCvar);
        virtual ~AsyncDispatcher();

        bool start();
        bool run();

        //-- Accessors --------------------------------------------------------

        zval *getDispatcher()
        {
            return m_pzvDispatcher;
        }

        string getDispatcherName()
        {
            return m_name;
        }

        DispatcherState getState()
        {
            return m_state;
        }

        long getResponseStatus()
        {
            return m_httpstatus;
        }

        ResponseBuffer *getResponseBuffer()
        {
            return &m_response;
        }
};

#endif /* ASYNC_DISPATCHER_H */
