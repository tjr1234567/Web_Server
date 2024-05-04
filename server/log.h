#ifndef __SERVER_LOG_H__
#define __SERVER_LOG_H__

#include <string>
#include <vector>
#include <stdint.h>
#include <memory>
#include <list>

namespace server
{
    // Log event
    class Logevent
    {
    public:
        typedef std::shared_ptr<Logevent> ptr;
        Logevent();

    private:
        const char *m_file = nullptr; // file name
        int32_t m_line = 0;           // line number
        uint32_t m_threadId = 0;      // thread ID
        uint32_t m_fiberId = 0;       // fiber ID
        uint32_t m_elapse = 0;        // time span since the program started(ms)
        uint64_t m_time = 0;          // timestamp
        std::string m_content;
    };
    class LogLevel
    {
    public:
        enum Level
        {
            DEBUG = 1,
            INFO = 2,
            WARN = 3,
            ERROR = 4,
            FATAL = 5
        };
    };
    // destination of log
    class LogAppender
    {
    public:
        typedef std::shared_ptr<LogAppender> ptr;
        virtual ~LogAppender();
        void log(LogLevel::Level level, Logevent::ptr event);

    private:
        LogLevel::Level m_level;
    };
    // formatter of log
    class LogFormatter
    {
    public:
        typedef std::shared_ptr<LogFormatter> ptr;
        std::string formate(Logevent::ptr event);
    };
    // outputter of log
    class Logger
    {
    public:
        typedef std::shared_ptr<Logger> ptr;

        Logger(const std::string &name = "root");
        void log(LogLevel::Level level, Logevent::ptr event);

        void debug(Logevent::ptr event);
        void info(Logevent::ptr event);
        void warn(Logevent::ptr event);
        void error(Logevent::ptr event);
        void fatal(Logevent::ptr event);

        void addAppender(LogAppender::ptr appender);
        void delAppender(LogAppender::ptr appender);

        LogLevel::Level getLevel() const { return m_level; }
        void setLevel(LogLevel::Level lev) { m_level = lev; }

    private:
        std::string m_name;                      // log name
        LogLevel::Level m_level;                 // log lovel
        std::list<LogAppender::ptr> m_appenders; // the list of appender
    };
    // Appender which present on console
    class StdoutLogAppender : public LogAppender
    {
    };
    // Appender which record on logFile
    class FileLogAppender : public LogAppender
    {
    };

}
#endif