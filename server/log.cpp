#include "log.h"
namespace server
{
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

};