#ifndef _LOGGER_H
#define _LOGGER_H

#include "Noncopyable.h"

class Logger : Noncopyable
{
public:
    Logger();

    void log(const char *level, const char *filename, const char *func, int line, const char *fmt, ...);

private:
    class Appender : Noncopyable
    {

    private:
    };

    inline void gen_prefix(const char *level);
    inline void gen_suffix(const char *filename, const char *func, int line);
    inline void gen_time();
    inline void gen_threadID();

    static const int SMALL_BUFFER_SIZE = 1024;
    static const int LARGE_BUFFER_SIZE = 1024 * 1024;

    static Appender appender_;

    char buf_[SMALL_BUFFER_SIZE];
    char *cur_;
    int size_;

    char time_[40];
    char threadID_[10];
};

#define LOG_ERROR(fmt, ...) \
    Logger().log("ERROR",__FILE__, __func__, __LINE__, fmt, __VA_ARGS__ );
#define LOG_INFO(fmt, ...) \
    Logger().log("INFO",__FILE__, __func__, __LINE__, fmt, __VA_ARGS__ );

#endif
