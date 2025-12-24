#ifndef LOG_H
#define LOG_H

#include "blockqueue.h"
#include <thread>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <assert.h>
#include <stdarg.h>

class Log {
public:

    
    void init(int level = 1, std::string path = "./log", 
                std::string suffix =".log",
                int maxQueueCapacity = 1024);

    static void FlushLogThread();

    void write(int level, const char *format,...);

    void flush();

    int GetLevel();
    void SetLevel(int level);
    bool IsOpen() { return isOpen_; }


    static Log* Instance();

    // 添加析构函数
    virtual ~Log();
private:
    Log();
    void AppendLogLevelTitle_(int level);
    void AsyncWrite_();
    Log(const Log&) = delete;
    Log& operator=(const Log&) =delete;

private:

    static const int LOG_PATH_LEN = 256;
    static const int LOG_NAME_LEN = 256;
    static const int MAX_LINES = 50000;

    int MAX_LINES_;
    int lineCount_;
    int toDay_;

    std::string path_;
    std::string suffix_;

    int level_;
    bool isOpen_;
    bool isAsync_;

    char buff_[4096];
    std::unique_ptr<BlockQueue<std::string>> queue_;
    std::unique_ptr<std::thread> writeThread_;
    std::mutex mutex_;
    FILE* fp_;
};

#define LOG_BASE(level, format, ...)\
    do {\
        Log* log = Log::Instance();\
        if (log->IsOpen() && log->GetLevel() <= level) {\
            log->write(level, format, ##__VA_ARGS__); \
        }\
    } while(0);

#define LOG_DEBUG(format, ...) do {LOG_BASE(0, format, ##__VA_ARGS__)} while(0)
#define LOG_INFO(format, ...) do {LOG_BASE(1, format, ##__VA_ARGS__)} while(0)
#define LOG_WARN(format, ...) do {LOG_BASE(2, format, ##__VA_ARGS__)} while(0)
#define LOG_ERROR(format, ...) do {LOG_BASE(3, format, ##__VA_ARGS__)} while(0)

#endif