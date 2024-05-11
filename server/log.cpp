#include "log.h"
#include <iostream>
namespace server
{
    // Log level
    const char *LogLevel::toString(LogLevel::Level level)
    {
        switch (level)
        {
#define XX(name)         \
    case LogLevel::name: \
        return #name;    \
        break;
            XX(DEBUG)
            XX(INFO)
            XX(WARN)
            XX(ERROR)
            XX(FATAL)
#undef XX
        default:
            return "UNKNOWN";
        }
        return "UNKNOWN";
    }
    // Logger
    Logger::Logger(const std::string &name)
        : m_name(name) {}

    void Logger::log(LogLevel::Level level, Logevent::ptr event)
    {
        if (level >= m_level)
        {
            for (auto &i : m_appenders)
            {
                i->log(level, event);
            }
        }
    }

    void Logger::debug(Logevent::ptr event)
    {
        log(LogLevel::DEBUG, event);
    }
    void Logger::info(Logevent::ptr event)
    {
        log(LogLevel::INFO, event);
    }
    void Logger::warn(Logevent::ptr event)
    {
        log(LogLevel::WARN, event);
    }
    void Logger::error(Logevent::ptr event)
    {
        log(LogLevel::ERROR, event);
    }
    void Logger::fatal(Logevent::ptr event)
    {
        log(LogLevel::FATAL, event);
    }
    void Logger::addAppender(LogAppender::ptr appender)
    {
        m_appenders.push_back(appender);
    }
    void Logger::delAppender(LogAppender::ptr appender)
    {
        for (auto i = m_appenders.begin(); i != m_appenders.end(); i++)
        {
            if (*i == appender)
            {
                m_appenders.erase(i);
                break;
            }
        }
    }

    // LogFormatter
    LogFormatter::LogFormatter(const std::string &pattern) : m_pattern(pattern) {}
    std::string LogFormatter::format(LogLevel::Level level, Logevent::ptr event)
    {
        std::stringstream s;
        for (auto &i : m_item)
        {
            i->format(s, level, event);
        }
        return s.str();
    }
    // Log tempalte: %xxx  OR  %xxx{xxx}  OR  %%
    void LogFormatter::init()
    {
        // str formate type
        std::vector<std::tuple<std::string, std::string, int>> vec;
        std::string gstr;
        size_t last_pos;

        for (size_t i; i < m_pattern.size(); i++)
        {
            if (m_pattern[i] != '%')
            {
                gstr.append(1, m_pattern[i]);
                continue;
            }
            if ((i + 1) < m_pattern.size())
            {
                if (m_pattern[i + 1] = '%')
                {
                    gstr.append(1, '%');
                    continue;
                }
            }
            size_t n = i + 1;
            int fmt_status = 0;
            int fmt_begin = 0;
            std::string fmt;
            std::string s;
            while (n < m_pattern.size())
            {
                if (isspace(m_pattern[i]))
                {
                    break;
                }
                if (fmt_status == 0)
                {
                    if (m_pattern[n] == '{')
                    {
                        s = m_pattern.substr(i + 1, n - i - 1);
                        fmt_status = 1; // decode the format
                        n++;
                        fmt_begin = n;
                        continue;
                    }
                }
                if (fmt_status = 1)
                {
                    if (m_pattern[n] == '}')
                    {
                        fmt = m_pattern.substr(fmt_begin + 1, n - fmt_begin - 1);
                        fmt_status = 2;
                        n++;
                        continue;
                    }
                }
            }
            if (fmt_status == 0)
            {
                if (!gstr.empty())
                {
                    vec.push_back(std::make_tuple(gstr, "", 0));
                }
                s = m_pattern.substr(i + 1, n - i - 1);
                vec.push_back(std::make_tuple(s, fmt, 1));
                i = n;
            }
            else if (fmt_status == 1)
            {
                std::cout << "pattern parse error:" << m_pattern << "-" << m_pattern.substr(i) << std::endl;
                vec.push_back(std::make_tuple("pattern parse error", fmt, 0));
            }
            else if (fmt_status == 2)
            {
                vec.push_back(std::make_tuple(s, fmt, 1));
                i = n;
            }
        }
        if (!gstr.empty())
        {
            vec.push_back(std::make_tuple(gstr, "", 0));
        }
        //%m -- message
        //%p -- level
        //%r -- time span from the event starts to be output
        //%c -- log name
        //%t -- thread id
        //%n -- enter
        //%d -- timestamp
        //%f -- filename
        //%l -- line number
    }
    class messageFormat : public LogFormatter::formatItem
    {
    public:
        std::string format(std::ostream &os, LogLevel::Level level, Logevent::ptr event) override
        {
            os << event->getContent();
        }
    };
    class levelFormat : public LogFormatter::formatItem
    {
    public:
        std::string format(std::ostream &os, LogLevel::Level level, Logevent::ptr event) override
        {
            os << LogLevel::toString(level);
        }
    };

    // LogAppender
    // FileLogAppender
    FileLogAppender::FileLogAppender(const std::string filename) : m_filename(filename) {}
    void FileLogAppender::log(LogLevel::Level level, Logevent::ptr event)
    {
        if (level >= m_level)
        {
            m_filestream << m_formatter->format(level, event);
        }
    }
    bool FileLogAppender::reopen()
    {
        if (m_filestream)
        {
            m_filestream.close();
        }
        m_filestream.open(m_filename);
        return !!m_filestream;
    }
    // StdoutLogAppender
    void StdoutLogAppender::log(LogLevel::Level level, Logevent::ptr event)
    {
        if (level >= m_level)
        {
            std::cout << m_formatter->format(level, event);
        }
    }
};