#ifndef __SERVER_LOG_H__
#define __SERVER_LOG_H__

#include <string>
#include <vector>
#include <stdint.h>
#include <memory>
#include <list>
#include <sstream>
#include <fstream>

namespace server
{
    // Log event
    class Logevent
    {
    public:
        typedef std::shared_ptr<Logevent> ptr;
        Logevent(LogLevel::Level level, const char *file, int32_t m_line, uint32_t elapse, uint32_t thread_id, uint32_t fiber_id, uint64_t time);
        const char *getfile() const { return m_file; }
        int32_t getLine() const { return m_line; }
        uint32_t getElapse() const { return m_elapse; }
        uint32_t getTime() const { return m_time; }
        uint32_t getThreadId() const { return m_threadId; }
        uint32_t getFiberId() const { return m_fiberId; }
        const std::string getContent() const { return m_content; }

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
            UNKNOWN = 0,
            DEBUG = 1,
            INFO = 2,
            WARN = 3,
            ERROR = 4,
            FATAL = 5
        };
        static const char *toString(LogLevel::Level level);
    };
    // destination of log
    class LogAppender
    {
    public:
        typedef std::shared_ptr<LogAppender> ptr;
        virtual ~LogAppender();
        virtual void log(LogLevel::Level level, Logevent::ptr event) = 0;
        void setFormatter(LogFormatter::ptr val) { m_formatter = val; }
        LogFormatter::ptr getFormatter() const { return m_formatter; }

    protected:
        LogLevel::Level m_level;
        LogFormatter::ptr m_formatter;
    };
    // formatter of log
    class LogFormatter
    {
    public:
        typedef std::shared_ptr<LogFormatter> ptr;
        std::string format(LogLevel::Level level, Logevent::ptr event);
        LogFormatter(const std::string &pattern);

    public:
        class formatItem
        {
        public:
            typedef std::shared_ptr<formatItem> ptr;
            virtual ~formatItem() {}
            virtual std::string format(std::ostream &os, LogLevel::Level level, Logevent::ptr event) = 0;
        };
        void init();

    private:
        std::string m_pattern;
        std::vector<formatItem::ptr> m_item;
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
    public:
        typedef std::shared_ptr<StdoutLogAppender> ptr;
        virtual void log(LogLevel::Level level, Logevent::ptr event) override;

    private:
    };

    // Appender which record on logFile
    class FileLogAppender : public LogAppender
    {
    public:
        FileLogAppender(const std::string filename);
        typedef std::shared_ptr<FileLogAppender> ptr;
        virtual void log(LogLevel::Level level, Logevent::ptr event) override;
        bool reopen(); // reopen the logfile, if the file opened successed return true

    private:
        std::string m_filename;
        std::ofstream m_filestream;
    };

}
#endif