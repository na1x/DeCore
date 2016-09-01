#ifndef ATOMIC_H
#define ATOMIC_H

#include <pthread.h>

namespace decore
{

/**
 * @brief Some kind of std::atomic from C++11
 */
template <typename T>
class Atomic
{
    mutable pthread_mutex_t mLock;
    T mData;

public:
    explicit Atomic(T initialValue)
        : mData(initialValue)
    {
        pthread_mutex_init(&mLock, NULL);
    }
    virtual ~Atomic()
    {
        pthread_mutex_destroy(&mLock);
    }

    void set(const T& data)
    {
        lock();
        mData = data;
        unlock();
    }

    T get() const
    {
        T res;
        lock();
        res = mData;
        unlock();
        return res;
    }
private:
    void lock() const
    {
        pthread_mutex_lock(&mLock);
    }

    void unlock() const
    {
        pthread_mutex_unlock(&mLock);
    }

};

}




#endif /* ATOMIC_H */

