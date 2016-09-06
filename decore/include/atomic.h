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

    /**
     * @brief Synchronously sets new value
     * @param data value to set
     * @return previsous value
     */

    T setAndGet(const T& data)
    {
        T res;
        lock();
        res = mData;
        mData = data;
        unlock();
        return res;
    }

    /**
     * @brief Synchronously returns value
     * @return value
     */
    T get() const
    {
        T res;
        lock();
        res = mData;
        unlock();
        return res;
    }
private:
    /**
     * @brief Internal lock
     */
    void lock() const
    {
        pthread_mutex_lock(&mLock);
    }
    /**
     * @brief Internal unlock
     */
    void unlock() const
    {
        pthread_mutex_unlock(&mLock);
    }

};

}




#endif /* ATOMIC_H */

