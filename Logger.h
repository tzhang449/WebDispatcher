#ifndef _LOGGER_H
#define _LOGGER_H

#include <vector>
#include <mutex>
#include <memory>
#include <thread>
#include <condition_variable>
#include <atomic>

#include "Noncopyable.h"

class Logger : Noncopyable
{
public:
    Logger();
    ~Logger();
    void log(const char *level,
             const char *filename,
             const char *func,
             int line,
             bool toAbort,
             const char *fmt,
             ...);

private:
    static const int SMALL_BUFFER_SIZE = 512;
    static const int LARGE_BUFFER_SIZE = 512 * 1024;

    class Appender : Noncopyable
    {
    public:
        Appender();
        ~Appender();
        void append(const char *buf, int len);
        void stop();

    private:
        static const int GIGABYTES = 1024 * 1024 * 1024;

        class Buffer : Noncopyable
        {
        public:
            Buffer();
            bool add(const char *buf, int len);
            const char *c_str();
            int len();

        private:
            char buf_[LARGE_BUFFER_SIZE];
            char *cur_;
            int size_;
        };

        void threadFunc();
        void writeToFile(std::vector<std::unique_ptr<Buffer>> &buffer);
        inline void fileWrite(const char *buf, int len);
        inline void rowFile();
        inline const char *nextFileName();

        std::unique_ptr<Buffer> buffer_;
        std::vector<std::unique_ptr<Buffer>> buffers_;

        std::mutex mutex_;
        std::condition_variable cond_;

        std::atomic<bool> running_;
        std::thread thread_;

        int fd_;
        int fileSize_;
    };

    static Appender &getAppender()
    {
        static Appender appender_;
        return appender_;
    }

    inline void gen_prefix(const char *level);
    inline void gen_suffix(const char *filename, const char *func, int line);

    char buf_[SMALL_BUFFER_SIZE];
    char *cur_;
    int size_;
    bool toAbort_;
};

#define LOG_TRACE(fmt, ...) \
    Logger().log("TRACE", __FILE__, __func__, __LINE__, false, fmt, __VA_ARGS__);
#define LOG_INFO(fmt, ...) \
    Logger().log("INFO", __FILE__, __func__, __LINE__, false, fmt, __VA_ARGS__);
#define LOG_ERROR(fmt, ...) \
    Logger().log("ERROR", __FILE__, __func__, __LINE__, false, fmt, __VA_ARGS__);
#define LOG_SYSERROR(fmt, ...) \
    Logger().log("SYSERROR", __FILE__, __func__, __LINE__, false, fmt, __VA_ARGS__);
#define LOG_FATAL(fmt, ...) \
    Logger().log("FATAL", __FILE__, __func__, __LINE__, true, fmt, __VA_ARGS__);
#define LOG_SYSFATAL(fmt, ...) \
    Logger().log("SYSFATAL", __FILE__, __func__, __LINE__, true, fmt, __VA_ARGS__);
#endif
