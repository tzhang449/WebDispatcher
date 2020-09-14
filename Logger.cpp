#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>

#include <cassert>
#include <cstdarg>
#include <cstdio>
#include <time.h>


#include "Logger.h"

Logger::Logger()
{
    cur_ = buf_;
    size_ = SMALL_BUFFER_SIZE;
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

    ::printf("%s",buf_);
    //appender_.append(buf_);
}

void Logger::gen_prefix(const char *level)
{
    gen_time();
    gen_threadID();
    int ret = ::snprintf(cur_, size_, "%s %s [%s] \"" , time_, threadID_, level);
    assert(ret < size_);
    size_ -= ret;
    cur_ += ret;
}

void Logger::gen_time()
{
    time_t rawtime;
    struct tm *timeinfo;

    ::time(&rawtime);
    timeinfo = ::localtime(&rawtime);

    int ret=::strftime(time_, sizeof(time_),"%F %T", timeinfo);

    struct timeval current_time;
    gettimeofday(&current_time,nullptr);
    
    snprintf(time_+ret,5,".%ld", current_time.tv_usec);
}

void Logger::gen_threadID(){
    thread_local pid_t threadID=0;

    if(threadID==0){
        threadID=::getpid();
    }
    ::snprintf(threadID_,sizeof(threadID_),"%d",threadID);
}

void Logger::gen_suffix(const char *filename, const char *func, int line)
{
    int ret = ::snprintf(cur_, size_, "\" - %s(%s):%d\n", filename, func, line);
    assert(ret < size_);
    size_ -= ret;
    cur_ += ret;
}