#include "log.h"

Log::Log() {
    lineCount_ = 0;
    isAsync_ = false;
    //writeThread_ = nullptr;
    queue_ = nullptr;
    toDay_ = 0;
    fp_ = nullptr;
}
Log::~Log() {
    if(writeThread_ && writeThread_->joinable()) {
        while(!queue_->empty()) {
            queue_->flush();
        };
        queue_->Close();
        writeThread_->join();
    }
    if(fp_) {
        std::lock_guard<std::mutex> locker(mutex_);
        flush();
        fclose(fp_);
    }
}

Log* Log::Instance()
{
    static Log instance;
    return &instance;
}

int Log::GetLevel()
{
    std::lock_guard<std::mutex> locker(mutex_);
    return level_;
}

void Log::SetLevel(int level) {
    std::lock_guard<std::mutex> locker(mutex_);
    level_ = level;
}

void Log::init(int level, std::string path , 
                std::string suffix,
                int maxQueueCapacity)
{
    isOpen_ = true;
    level_ = level;
    if(maxQueueCapacity > 0){
        isAsync_ = true;
        if(!queue_){
            queue_ = std::make_unique<BlockQueue<std::string>>(maxQueueCapacity);
            writeThread_ = std::make_unique<std::thread>(FlushLogThread);
        }
    }else{
        isAsync_ = false;
    }

    lineCount_ = 0;

    time_t timer = time(nullptr);
    struct tm t;
    localtime_r(&timer,&t);
    path_ = path;
    suffix_ = suffix;
    char fileName[LOG_NAME_LEN] = {0};
    snprintf(fileName, LOG_NAME_LEN - 1, "%s/%04d_%02d_%02d%s", 
            path_.c_str(), t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, suffix_.c_str());
    toDay_ = t.tm_mday;
    {
        std::lock_guard<std::mutex> locker(mutex_);
        memset(buff_, 0, sizeof(buff_));
        if(fp_) { 
            flush();
            fclose(fp_); 
        }
        fp_ = fopen(fileName, "a");
        if(fp_ == nullptr){
            mkdir(path_.c_str(), 0777);
            fp_ = fopen(fileName, "a");
        }
        assert(fp_ != nullptr);
    }
}

void Log::write(int level, const char *format, ...)
{
    struct timeval now = {0, 0};
    gettimeofday(&now, nullptr);
    time_t tSec = now.tv_sec;
    struct tm t;
    localtime_r(&tSec, &t);
    va_list vaList;

    /* 日志日期 日志行数 */
    if (toDay_ != t.tm_mday || (lineCount_ && (lineCount_  %  MAX_LINES == 0)))
    {
        std::unique_lock<std::mutex> locker(mutex_);
        locker.unlock();

        char newFile[LOG_NAME_LEN];
        char tail[36] = {0};
        snprintf(tail, 36, "%04d_%02d_%02d", t.tm_year + 1900, t.tm_mon + 1, t.tm_mday);
        if (toDay_ != t.tm_mday)
        {
            snprintf(newFile, LOG_NAME_LEN - 72, "%s/%s%s", path_.c_str(), tail, suffix_.c_str());
            toDay_ = t.tm_mday;
            lineCount_ = 0;
        }
        else{
            snprintf(newFile, LOG_NAME_LEN - 72, "%s/%s-%d%s", path_.c_str(), tail, (lineCount_  / MAX_LINES), suffix_.c_str());
        }
        locker.lock();
        flush();
        fclose(fp_);
        fp_ = fopen(newFile, "a");
        assert(fp_ != nullptr);
    }

    {
        std::unique_lock<std::mutex> locker(mutex_);
        lineCount_++;
        int n = snprintf(buff_, 128, "%d-%02d-%02d %02d:%02d:%02d.%06ld ",
                    t.tm_year + 1900, t.tm_mon + 1, t.tm_mday,
                    t.tm_hour, t.tm_min, t.tm_sec, now.tv_usec);
        AppendLogLevelTitle_(level);
        
        va_start(vaList, format);
        int currentLen = strlen(buff_);
        int m = vsnprintf(buff_ + currentLen, sizeof(buff_) - currentLen - 1, format, vaList);
        va_end(vaList);
        strcat(buff_,"\n\0");

        if(isAsync_ && queue_ && !queue_->full()) {
            queue_->push(std::string(buff_));//push需要的是string变量
        } else {
            fputs(buff_, fp_);
        }
        memset(buff_, 0, sizeof(buff_));
    }
}

void Log::AppendLogLevelTitle_(int level) {
    switch(level) {
    case 0:
        strcat(buff_,"[debug] : ");
        break;
    case 1:
        strcat(buff_,"[info] : ");
        break;
    case 2:
        strcat(buff_,"[warn] : ");
        break;
    case 3:
        strcat(buff_,"[error]: ");
        break;
    default:
        strcat(buff_,"[error]: ");
        break;
    }
}

void Log::FlushLogThread()
{
    Log::Instance()->AsyncWrite_();
}

void Log::flush() {
    if(isAsync_) { 
        queue_->flush(); 
    }
    fflush(fp_);
}

void Log::AsyncWrite_() {
    std::string str = "";
    while(queue_->pop(str)) {
        std::lock_guard<std::mutex> locker(mutex_);
        fputs(str.c_str(), fp_);
        fflush(fp_); // 【新增】手动刷新，确保立马看到日志
    }
}