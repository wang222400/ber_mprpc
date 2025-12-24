#ifndef BLOCKQUEUE_H
#define BLOCKQUEUE_H

#include <mutex>
#include <deque>
#include <condition_variable>
#include <sys/time.h>
#include <assert.h>

template<class T>
class BlockQueue{
public:
    BlockQueue(size_t MaxCapacity = 1024);
    ~BlockQueue();

    bool push(const T& item);

    bool pop(T& item);

    bool empty();

    bool full();

    size_t capacity();

    void Close();

    void flush();

private:
    std::mutex mutex_;
    std::deque<T> deq_;
    std::condition_variable conConsumer_;
    std::condition_variable conProducer_;
    size_t capacity_;
    bool isClose_;
};

template<class T>
bool BlockQueue<T>::full(){
    std::lock_guard<std::mutex> locker(mutex_);
    return deq_.size() >= capacity_;
}

template<class T>
BlockQueue<T>::BlockQueue(size_t MaxCapacity) : capacity_(MaxCapacity)
{
    assert(MaxCapacity > 0);
    isClose_ = false;
}

template<class T>
BlockQueue<T>::~BlockQueue()
{
    Close();
}

template<class T>
bool BlockQueue<T>::push(const T& item)
{
    std::unique_lock<std::mutex> locker(mutex_);
    while(deq_.size() >= capacity_){
        conProducer_.wait(locker);
        if(isClose_){
            return false;
        }
    }
    deq_.push_back(item);
    conConsumer_.notify_one();
    return true;
}

template<class T>
bool BlockQueue<T>::pop(T& item)
{
    std::unique_lock<std::mutex> locker(mutex_);
    while(deq_.empty()){
        conConsumer_.wait(locker);
        if(isClose_){
            return false;
        }
    }
    item = deq_.front();
    deq_.pop_front();
    conProducer_.notify_one();
    return true;
}

template<class T>
bool BlockQueue<T>::empty()
{
    std::lock_guard<std::mutex> locker(mutex_);
    return deq_.empty();
}

template<class T>
size_t BlockQueue<T>::capacity()
{
    std::lock_guard<std::mutex> locker(mutex_);
    return capacity_;
}

template<class T>
void BlockQueue<T>::Close()
{
    {
        std::lock_guard<std::mutex> locker(mutex_);
        deq_.clear();
        isClose_ = true;
    }
    conProducer_.notify_all();
    conConsumer_.notify_all();
}

template<class T>
void BlockQueue<T>::flush()
{
    conConsumer_.notify_one();
}

#endif