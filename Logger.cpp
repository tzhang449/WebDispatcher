#include <sys/time.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <fcntl.h>

#include <cassert>
#include <cstdarg>
#include <cstdio>
#include <ctime>
#include <cstring>
#include <memory>

#include "Logger.h"

Logger::Logger() : buf_(),
                   cur_(buf_),
                   size_(SMALL_BUFFER_SIZE)
{
}

void Logger::log(const char *level, const char *filename, const char *func, int line, const char *fmt, ...)
{
    gen_prefix(level);

    va_list args;
    va_start(args, fmt);
    int ret = ::vsnprintf(cur_, size_, fmt, args);
    va_end(args);

    assert(ret < size_);
    size_ -= ret;
    cur_ += ret;

    gen_suffix(filename, func, line);

    appender_.append(buf_, SMALL_BUFFER_SIZE - size_);
}

void Logger::gen_prefix(const char *level)
{
    thread_local static pid_t threadID = 0;

    struct timeval tv;
    time_t nowtime;
    struct tm *nowtm;
    char tmbuf[64];
    int ret;

    gettimeofday(&tv, nullptr);
    nowtime = tv.tv_sec;
    nowtm = localtime(&nowtime);
    strftime(tmbuf, sizeof(tmbuf), "%Y-%m-%d %H:%M:%S", nowtm);
    if (threadID == 0)
    {
        threadID = syscall(SYS_gettid);
    }
    ret = ::snprintf(cur_, size_, "%s.%06ld %d [%s] \"", tmbuf, tv.tv_usec, threadID, level);
    assert(ret < size_);
    size_ -= ret;
    cur_ += ret;
}

void Logger::gen_suffix(const char *filename, const char *func, int line)
{
    int ret = ::snprintf(cur_, size_, "\" - %s(%s):%d\n", filename, func, line);
    assert(ret < size_);
    size_ -= ret;
    cur_ += ret;
}

Logger::Appender::Appender() : buffer_(std::make_unique<Buffer>()),
                               buffers_(std::vector<std::unique_ptr<Buffer>>()),
                               mutex_(),
                               cond_(),
                               running_(true),
                               thread_(std::thread(&Logger::Appender::threadFunc, this)),
                               fd_(),
                               fileSize_()
{
}

Logger::Appender::~Appender()
{
    running_ = false;
    if (thread_.joinable())
        thread_.join();
}

void Logger::Appender::append(const char *buf, int size)
{
    std::lock_guard<std::mutex> guard(mutex_);
    if (!buffer_->add(buf, size))
    {
        buffers_.push_back(std::move(buffer_));
        buffer_ = std::make_unique<Buffer>();
        buffer_->add(buf, size);
        cond_.notify_all();
    }
}

void Logger::Appender::threadFunc()
{
    std::vector<std::unique_ptr<Buffer>> buffersToWrite;
    while (running_)
    {
        {
            std::unique_lock<std::mutex> guard(mutex_);

            if (buffers_.empty())
            {
                cond_.wait_for(guard,
                               std::chrono::microseconds(1000));
            }
            buffersToWrite.swap(buffers_);
        }
        writeToFile(buffersToWrite);
        buffersToWrite.clear();
    }
    buffers_.push_back(std::move(buffer_));
    writeToFile(buffers_);
}

void Logger::Appender::writeToFile(std::vector<std::unique_ptr<Buffer>> &buffer)
{
    rowFile();
    for (auto &buf : buffer)
    {
        fileWrite(buf->c_str(), buf->len());
    }
}

void Logger::Appender::fileWrite(const char *buf, int len)
{
    int num;
    int numToWrite = len;

    while ((num = ::write(fd_, buf, len)) && (num == -1 || num < numToWrite))
    {
        if (num != -1)
            numToWrite -= num;
    }
    fileSize_ += len;
}

void Logger::Appender::rowFile()
{
    if (!fd_ || fileSize_ > GIGABYTES)
    {
        if (fd_)
            ::close(fd_);
        fd_ = ::open(nextFileName(), O_CREAT | O_WRONLY);
        fileSize_ = 0;
    }
}

const char *Logger::Appender::nextFileName()
{
    static char name[256];
    static char hostname[40];
    thread_local static pid_t threadID = 0;

    extern const char *__progname;
    struct timeval tv;
    time_t nowtime;
    struct tm *nowtm;
    char tmbuf[64];

    gethostname(hostname, sizeof(hostname));

    if (threadID == 0)
    {
        threadID = getpid();
    }

    gettimeofday(&tv, nullptr);
    nowtime = tv.tv_sec;
    nowtm = localtime(&nowtime);
    strftime(tmbuf, sizeof(tmbuf), "%Y%m%d-%H%M%S", nowtm);

    ::snprintf(name, sizeof(name), "%s.%s.%s.%d.log", __progname, tmbuf, hostname, threadID);
    return name;
}

Logger::Appender Logger::appender_;

Logger::Appender::Buffer::Buffer() : buf_(),
                                     cur_(buf_),
                                     size_(LARGE_BUFFER_SIZE)
{
}

bool Logger::Appender::Buffer::add(const char *buf, int len)
{
    if (len > size_)
        return false;
    ::memcpy(cur_, buf, len);
    cur_ += len;
    size_ -= len;
    return true;
}

const char *Logger::Appender::Buffer::c_str()
{
    return buf_;
}

int Logger::Appender::Buffer::len()
{
    return cur_ - buf_;
}