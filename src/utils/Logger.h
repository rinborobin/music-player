#ifndef LOGGER_H
#define LOGGER_H

#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

// Logger level can be controlled at compile time.
// Define LOG_LEVEL before including this header, or edit the default below.
// Levels: 0 = Debug, 1 = Info, 2 = Warning, 3 = Error
#ifndef LOG_LEVEL
#define LOG_LEVEL 0
#endif

namespace Logger
{

enum class Level
{
    Debug,
    Info,
    Warning,
    Error
};

inline std::string levelToString(Level level)
{
    switch (level)
    {
    case Level::Debug:
        return "DEBUG";
    case Level::Info:
        return "INFO";
    case Level::Warning:
        return "WARN";
    case Level::Error:
        return "ERROR";
    default:
        return "UNKNOWN";
    }
}

inline std::string currentTimestamp()
{
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");

    return ss.str();
}

inline void log(Level level, const std::string &message)
{
    int levelValue = static_cast<int>(level);

    if (levelValue < LOG_LEVEL)
    {
        return;
    }

    std::cerr << "[" << currentTimestamp() << "]"
              << "[" << levelToString(level) << "] "
              << message
              << std::endl;
}

} // namespace Logger

#define LOG_DEBUG(message) Logger::log(Logger::Level::Debug, message)
#define LOG_INFO(message) Logger::log(Logger::Level::Info, message)
#define LOG_WARN(message) Logger::log(Logger::Level::Warning, message)
#define LOG_ERROR(message) Logger::log(Logger::Level::Error, message)

#endif // LOGGER_H
