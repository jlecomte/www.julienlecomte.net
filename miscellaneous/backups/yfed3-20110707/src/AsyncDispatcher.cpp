#include "AsyncDispatcher.h"
#include "utils.h"
#include "Logger.h"

#include <assert.h>

PollFdArray::PollFdArray() :
    m_arraySize(0),
    m_fds(NULL),
    m_nfds(0)
{
    DEFINE_LOGGER("yfed3.PollFdArray.constructor");
}

PollFdArray::~PollFdArray()
{
    DEFINE_LOGGER("yfed3.PollFdArray.destructor");

    if (m_fds) {
        delete[] m_fds;
    }
}

int PollFdArray::build(fd_set &fdRead, fd_set &fdWrite, fd_set &fdExcept, int maxFds)
{
    DEFINE_LOGGER("yfed3.PollFdArray.build");

    int count, fd, idx;

    // Find out the needed array size first.

    count = 0;
    for (fd = 0; fd < maxFds; fd++) {
        if (FD_ISSET(fd, &fdRead) ||
            FD_ISSET(fd, &fdWrite) ||
            FD_ISSET(fd, &fdExcept)) {
            count++;
        }
    }

    // Check storage.

    if (!m_fds) {
        // No existing storage. Create it.
        m_fds = new pollfd[count];
        m_arraySize = count;
    } else if (m_arraySize < count) {
        // Increase existing storage.
        delete[] m_fds;
        m_fds = new pollfd[count];
        m_arraySize = count;
    }

    // Fill the array with file descriptors.

    idx = 0;
    for (fd = 0; fd < maxFds; fd++) {
        bool added = false;
        m_fds[idx].events = 0;

        if (FD_ISSET(fd, &fdRead)) {
            m_fds[idx].fd = fd;
            m_fds[idx].events |= POLLIN;
            added = true;
        }

        if (FD_ISSET(fd, &fdWrite)) {
            m_fds[idx].fd = fd;
            m_fds[idx].events |= POLLOUT;
            added = true;
        }

        if (FD_ISSET(fd, &fdExcept)) {
            m_fds[idx].fd = fd;
            m_fds[idx].events |= POLLPRI;
            added = true;
        }

        if (added) {
            idx++;
        }
    }

    assert(idx == count);

    m_nfds = count;

    return count;
}

static size_t httpmulti_write_callback(void *ptr, size_t size, size_t nmemb, ResponseBuffer *response)
{
    DEFINE_LOGGER("yfed3.httpmulti_write_callback");

    size_t len = size * nmemb;

    // Grow the response buffer if needed.
    if ((response->bufLen + len + 1) > response->bufSize) {

        response->bufSize = response->bufLen + len + (1 << 19);
        response->buf = (char*)erealloc(response->buf, response->bufSize);

        if (!response->buf) {
            ERROR_STREAM() << "Buffer reallocation failed" << FSEP_LOGGER_FLUSH;
            response->bufSize = 0;
            return 0;
        }
    }

    memcpy(response->buf + response->bufLen, ptr, len);
    response->bufLen += len;
    return len;
}

static void *async_dispatcher_thread_fn(void *async_dispatcher_thread_args)
{
    DEFINE_LOGGER("yfed3.async_dispatcher_thread_fn");

    AsyncDispatcher *dispatcher = (AsyncDispatcher*)async_dispatcher_thread_args;
    return (void*) dispatcher->run();
}

AsyncDispatcher::AsyncDispatcher(zval *pzvDispatcher, char *name, uint nameLen, HashTable *config, st_cond_t endCvar):
    m_pzvDispatcher(pzvDispatcher),
    m_name(name),
    m_state(STOPPED),
    m_config(config),
    m_tid(NULL),
    m_endCvar(endCvar),
    m_cmh(NULL),
    m_ch(NULL),
    m_httpstatus(0)
{
    DEFINE_LOGGER("yfed3.AsyncDispatcher.constructor");

    memset(&m_response, 0, sizeof(ResponseBuffer));
}

AsyncDispatcher::~AsyncDispatcher()
{
    this->cleanup();
}

void AsyncDispatcher::cleanup()
{
    if (m_ch) {
        curl_multi_remove_handle(m_cmh, m_ch);
        curl_easy_cleanup(m_ch);
        m_ch = NULL;
    }

    if (m_cmh) {
        curl_multi_cleanup(m_cmh);
        m_cmh = NULL;
    }

    FREE_AND_NIL(m_response.buf);
    memset(&m_response, 0, sizeof(ResponseBuffer));
}

bool AsyncDispatcher::start()
{
    DEFINE_LOGGER("yfed3.AsyncDispatcher.start");

    if (m_tid) {
        WARN_STREAM() << "AsyncDispatcher::start() was apparently invoked multiple times" << FSEP_LOGGER_FLUSH;
        return false;
    }

    // Prepare the response buffer.

    m_response.bufLen = 0;
    m_response.bufSize = 1 << 20; // 1MB
    m_response.buf = (char*)emalloc(m_response.bufSize);
    if (!m_response.buf) {
        ERROR_STREAM() << "Buffer allocation failed" << FSEP_LOGGER_FLUSH;
        goto failure;
    }

    // Prepare the cUrl handles.

    m_cmh = curl_multi_init();
    if (!m_cmh) {
        ERROR_STREAM() << "curl_multi_init() failed unexpectedly" << FSEP_LOGGER_FLUSH;
        goto failure;
    }

    m_ch = curl_easy_init();
    if (!m_ch) {
        ERROR_STREAM() << "curl_easy_init() failed unexpectedly" << FSEP_LOGGER_FLUSH;
        goto failure;
    }

    if (curl_multi_add_handle(m_cmh, m_ch) != CURLM_OK) {
        ERROR_STREAM() << "curl_multi_add_handle() failed unexpectedly" << FSEP_LOGGER_FLUSH;
        goto failure;
    }

    // Custom cUrl configuration.
    // The code below is trivially simple. A LOT OF WORK IS NEEDED HERE!
    // It probably should move to a separate function, eventually)

    zval **ppzval;

    FOREACH(m_config, ppzval) {

        zval *zvalue = *ppzval;
        char *key;
        uint keyLen;
        ulong dummy;

        if (zend_hash_get_current_key_ex(m_config, &key, &keyLen, &dummy, 0, NULL) != HASH_KEY_IS_STRING) {
            WARN_STREAM() << "Array keys must be strings" << FSEP_LOGGER_FLUSH;
            continue;
        }

        if (strncmp("url", key, keyLen) == 0) {
            convert_to_string_ex(&zvalue);
            DEBUG_STREAM() << "Setting CURLOPT_URL to '" << Z_STRVAL_P(zvalue) << "'" << FSEP_LOGGER_FLUSH;
            // Note: strings passed to libcurl as 'char *' arguments are
            // copied by the library... NOTE: before 7.17.0, strings were
            // not copied! With this code, we handle that case.
            m_url = estrndup(Z_STRVAL_P(zvalue), Z_STRLEN_P(zvalue));
            curl_easy_setopt(m_ch, CURLOPT_URL, m_url.c_str());
        }
    }

    // Basic cUrl configuration.

    // Disable header for now. Soon, we should enable it, parse it and send it
    // to the extract method as an array parameter...
    // curl_easy_setopt(m_ch, CURLOPT_HEADER, 1);

    // What does this do?
    // curl_easy_setopt(m_ch, CURLOPT_NOPROGRESS, 1);

    curl_easy_setopt(m_ch, CURLOPT_WRITEFUNCTION, httpmulti_write_callback);
    curl_easy_setopt(m_ch, CURLOPT_WRITEDATA, &m_response);

    // Finally, create a new state thread with a 1MB stack.
    // TODO: investigate the impact of this value on performance.
    m_tid = st_thread_create(async_dispatcher_thread_fn, this, 0, (1 << 20));

    if (!m_tid) {
        ERROR_STREAM() << "st_thread_create() failed unexpectedly (" << strerror(errno) << ")" << FSEP_LOGGER_FLUSH;
        goto failure;
    }

    return true;

failure:

    this->cleanup();
    return false;
}

bool AsyncDispatcher::run()
{
    DEFINE_LOGGER("yfed3.AsyncDispatcher.run");

    bool success = false;
    CURLMcode code;
    PollFdArray fds;
    fd_set fdRead, fdWrite, fdExcept;
    st_utime_t pollTimeOut;
    int active, maxFds, nfds, rc;

    m_state = RUNNING;

    for (;;) {

        do {
            code = curl_multi_perform(m_cmh, &active);
        } while (code == CURLM_CALL_MULTI_PERFORM);

        if (code != CURLM_OK) {
            ERROR_STREAM() << "curl_multi_perform() failed unexpectedly" << FSEP_LOGGER_FLUSH;
            goto finish;
        } else if (!active) {
            break;
        }

        FD_ZERO(&fdRead);
        FD_ZERO(&fdWrite);
        FD_ZERO(&fdExcept);

        if (curl_multi_fdset(m_cmh, &fdRead, &fdWrite, &fdExcept, &maxFds) != CURLM_OK) {
            ERROR_STREAM() << "curl_multi_fdset() failed unexpectedly" << FSEP_LOGGER_FLUSH;
            goto finish;
        }

        nfds = fds.build(fdRead, fdWrite, fdExcept, maxFds+1);

        // For now, use a static 100 msec timeout. Eventually, this timeout should
        // decrease over time based on the various timeout values set in the
        // dispatcher's configuration.
        pollTimeOut = 1000000;

        rc = st_poll(fds.m_fds, fds.m_nfds, pollTimeOut);

        if (rc < 0) {
            ERROR_STREAM() << "st_poll() failed unexpectedly (" << strerror(errno) << ")" << FSEP_LOGGER_FLUSH;
            goto finish;
        }
    }

    // The transfer is complete. Check status.

    curl_easy_getinfo(m_ch, CURLINFO_RESPONSE_CODE, &m_httpstatus);
    if (m_httpstatus < 200 || m_httpstatus >= 400) {
        ERROR_STREAM() << "Dispatcher [" << m_name << "] received HTTP response code [" << m_httpstatus << "] (url = '" << m_url << "')" << FSEP_LOGGER_FLUSH;
        goto finish;
    }

    success = true;

finish:

    m_state = STOPPED;
    st_cond_signal(m_endCvar);
    return success;
}
