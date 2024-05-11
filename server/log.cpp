#include "log.h"
#include <iostream>
#include <map>
#include <functional>
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
    class messageFormat : public LogFormatter::formatItem
    {
    public:
        messageFormat(const std::string &str = "") {}
        std::string format(std::ostream &os, std::shared_ptr<Logger> logger, LogLevel::Level level, Logevent::ptr event) override
        {
            os << event->getContent();
        }
    };
    class levelFormat : public LogFormatter::formatItem
    {
    public:
        levelFormat(const std::string &str = "") {}
        std::string format(std::ostream &os, std::shared_ptr<Logger> logger, LogLevel::Level level, Logevent::ptr event) override
        {
            os << LogLevel::toString(level);
        }
    };
    class elapseFormat : public LogFormatter::formatItem
    {
    public:
        elapseFormat(const std::string &str = "") {}
        std::string format(std::ostream &os, std::shared_ptr<Logger> logger, LogLevel::Level level, Logevent::ptr event) override
        {
            os << event->getElapse();
        }
    };
    class nameFormat : public LogFormatter::formatItem
    {
    public:
        nameFormat(const std::string &str = "") {}
        std::string format(std::ostream &os, std::shared_ptr<Logger> logger, LogLevel::Level level, Logevent::ptr event) override
        {
            os << logger->getName();
        }
    };
    class threadIdFormat : public LogFormatter::formatItem
    {
    public:
        threadIdFormat(const std::string &str = "") {}
        std::string format(std::ostream &os, std::shared_ptr<Logger> logger, LogLevel::Level level, Logevent::ptr event) override
        {
            os << event->getThreadId();
        }
    };
    class fiberIdFormat : public LogFormatter::formatItem
    {
    public:
        fiberIdFormat(const std::string &str = "") {}
        std::string format(std::ostream &os, std::shared_ptr<Logger> logger, LogLevel::Level level, Logevent::ptr event) override
        {
            os << event->getFiberId();
        }
    };
    class dateTimeFormat : public LogFormatter::formatItem
    {
    public:
        dateTimeFormat(const std::string &format = "%Y%m%d %H%M%S") : m_format(format) {}
        std::string format(std::ostream &os, std::shared_ptr<Logger> logger, LogLevel::Level level, Logevent::ptr event) override
        {
            os << event->getTime();
        }

    private:
        std::string m_format;
    };
    class fileNameFormat : public LogFormatter::formatItem
    {
    public:
        fileNameFormat(const std::string &str = "") {}
        std::string format(std::ostream &os, std::shared_ptr<Logger> logger, LogLevel::Level level, Logevent::ptr event) override
        {
            os << event->getfile();
        }
    };
    class lineFormat : public LogFormatter::formatItem
    {
    public:
        lineFormat(const std::string &str = "") {}
        std::string format(std::ostream &os, std::shared_ptr<Logger> logger, LogLevel::Level level, Logevent::ptr event) override
        {
            os << event->getLine();
        }
    };
    class newLineFormat : public LogFormatter::formatItem
    {
    public:
        newLineFormat(const std::string &str = "") {}
        std::string format(std::ostream &os, std::shared_ptr<Logger> logger, LogLevel::Level level, Logevent::ptr event) override
        {
            os << std::endl;
        }
    };

    class stringFormat : public LogFormatter::formatItem
    {
    public:
        stringFormat(const std::string &str)
            : m_string(str) {}
        std::string format(std::ostream &os, std::shared_ptr<Logger> logger, LogLevel::Level level, Logevent::ptr event) override
        {
            os << m_string;
        }

    private:
        std::string m_string;
    };
    class tabFormat : public LogFormatter::formatItem
    {
    public:
        tabFormat(const std::string &str = "") {}
        std::string format(std::ostream &os, std::shared_ptr<Logger> logger, LogLevel::Level level, Logevent::ptr event) override
        {
            os << "\t";
        }

    private:
        std::string m_string;
    };

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
    std::string LogFormatter::format(std::shared_ptr<Logger> logger, LogLevel::Level level, Logevent::ptr event)
    {
        std::stringstream s;
        for (auto &i : m_item)
        {
            i->format(s, logger, level, event);
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
                        break;
                    }
                }
                n++;
            }
            if (fmt_status == 0)
            {
                if (!gstr.empty())
                {
                    vec.push_back(std::make_tuple(gstr, "", 0));
                    gstr.clear();
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
        static std::map<std::string, std::function<formatItem::ptr(const std::string &str)>> s_format_items = {
#define XX(str, C) \
    {#str, [](const std::string &fmt) { return formatItem::ptr(new C(fmt)); }}

            XX(m, messageFormat),
            XX(p, levelFormat),
            XX(r, elapseFormat),
            XX(c, nameFormat),
            XX(t, threadIdFormat),
            XX(n, newLineFormat),
            XX(d, dateTimeFormat),
            XX(f, fileNameFormat),
            XX(l, lineFormat),
            XX(T, tabFormat),
            XX(F, fiberIdFormat),
#undef XX
        };
        //%m -- message
        //%p -- level
        //%r -- time span from the event starts to be output
        //%c -- log name
        //%t -- thread id
        //%n -- enter
        //%d -- timestamp
        //%f -- filename
        //%l -- line number
        for (auto &i : vec)
        {
            if (std::get<2>(i) == 0)
            {
                m_item.push_back(formatItem::ptr(new stringFormat(std::get<0>(i))));
            }
            else
            {
                auto it = s_format_items.find(std::get<0>(i));
                if (it == s_format_items.end())
                {
                    m_item.push_back(formatItem::ptr(new stringFormat("<<error_format %" + std::get<0>(i) + ">>")));
                }
                else
                {
                    m_item.push_back(it->second(std::get<1>(i)));
                }
            }
            std::cout << std::get<0>(i) << "-" << std::get<1>(i) << "-" << std::get<2>(i) << std::endl;
        }
    }

    // LogAppender
    // FileLogAppender
    FileLogAppender::FileLogAppender(const std::string filename) : m_filename(filename) {}
    void FileLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, Logevent::ptr event)
    {
        if (level >= m_level)
        {
            m_filestream << m_formatter->format(logger, level, event);
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
    void StdoutLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, Logevent::ptr event)
    {
        if (level >= m_level)
        {
            std::cout << m_formatter->format(logger, level, event);
        }
    }
};