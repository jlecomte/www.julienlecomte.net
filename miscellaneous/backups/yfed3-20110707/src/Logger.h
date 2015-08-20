#ifndef FSEP_LOGGER_H
#define FSEP_LOGGER_H

#include <iostream>
#ifdef LINUX
# include <ostream>
#else
# include <ostream.h>
#endif
#include <sstream>
#include <log4cpp/Category.hh>
#include <log4cpp/PropertyConfigurator.hh>

#define DEFAULT_LOGGER_CONFIG "/home/y/conf/fsep_logger/Logger.conf"

#ifdef endl
# define FSEP_LOGGER_FLUSH endl
#else
# define FSEP_LOGGER_FLUSH "\n"
#endif

using namespace std;

namespace fsep_logger
{
    inline void init(string filename = "")
    {
        if (filename.empty()) {
            log4cpp::PropertyConfigurator::configure(DEFAULT_LOGGER_CONFIG);
        } else {
            struct stat sb;
            int retVal = stat(filename.c_str(), &sb);
            if (retVal == 0) {
                log4cpp::PropertyConfigurator::configure(filename);
            } else {
                cerr<<"Logger configuration file ["<<filename<<"] cannot be opened; using the default Logger configuration file at ["<<DEFAULT_LOGGER_CONFIG<<"]"<<FSEP_LOGGER_FLUSH;
                log4cpp::PropertyConfigurator::configure(DEFAULT_LOGGER_CONFIG);
            }
        }
    }

    class Logger
    {
        private:
            log4cpp::Category& m_logger;

        public:
            Logger(const string& name, const string& file, const int line) : m_logger(log4cpp::Category::getInstance(name))
            {
                if (m_logger.isDebugEnabled()) {
                    m_logger.debugStream() << "[" << file << ":" << line << "] " << " Entering..." << FSEP_LOGGER_FLUSH;
                }
            }

            ~Logger()
            {
                if (m_logger.isDebugEnabled()) {
                    m_logger.debugStream() << "Exiting..." << FSEP_LOGGER_FLUSH;
                }
            }

            log4cpp::Category& operator()()
            {
                return m_logger;
            }
    };
}

#define INIT_LOGGER(args...) fsep_logger::init(args)

#define DEFINE_LOGGER(name) fsep_logger::Logger logger(name, __FILE__, __LINE__)

// Macros for logging at DEBUG priority
#define DEBUG_STREAM() if (logger().isDebugEnabled()) logger().debugStream()<<"["<<__FILE__<<":"<<__LINE__<<"] "
#define IS_DEBUG_ENABLED() logger().isDebugEnabled()
#define DEBUG_FMTSTR(format, args...) \
       if (logger().isDebugEnabled()) \
         { \
           stringstream strm; \
           strm<<"["<<__FILE__<<":"<<__LINE__<<"] "<<format; \
           logger().debug(strm.str().c_str(), args); \
         }
#define DEBUG_STLSTR(message) DEBUG_STREAM()<<message

// Macros for logging at WARN priority
#define WARN_STREAM() if (logger().isWarnEnabled()) logger().warnStream()<<"["<<__FILE__<<":"<<__LINE__<<"] "
#define IS_WARN_ENABLED() logger().isWarnEnabled()
#define WARN_FMTSTR(format, args...) \
       if (logger().isWarnEnabled()) \
         { \
           stringstream strm; \
           strm<<"["<<__FILE__<<":"<<__LINE__<<"] "<<format; \
           logger().warn(strm.str().c_str(), args); \
         }
#define WARN_STLSTR(message) WARN_STREAM()<<message

// Macros for logging at ERROR priority
#define ERROR_STREAM() if (logger().isErrorEnabled()) logger().errorStream()<<"["<<__FILE__<<":"<<__LINE__<<"] "
#define IS_ERROR_ENABLED() logger().isErrorEnabled()
#define ERROR_FMTSTR(format, args...) \
       if (logger().isErrorEnabled()) \
         { \
           stringstream strm; \
           strm<<"["<<__FILE__<<":"<<__LINE__<<"] "<<format; \
           logger().error(strm.str().c_str(), args); \
         }
#define ERROR_STLSTR(message) ERROR_STREAM()<<message

// Macros for logging at ALERT priority
#define ALERT_STREAM() if (logger().isAlertEnabled()) logger().alertStream()<<"["<<__FILE__<<":"<<__LINE__<<"] "
#define IS_ALERT_ENABLED() logger().isAlertEnabled()
#define ALERT_FMTSTR(format, args...) \
       if (logger().isAlertEnabled()) \
         { \
           stringstream strm; \
           strm<<"["<<__FILE__<<":"<<__LINE__<<"] "<<format; \
           logger().alert(strm.str().c_str(), args); \
         }
#define ALERT_STLSTR(message) ALERT_STREAM()<<message

#endif /* FSEP_LOGGER_H */
