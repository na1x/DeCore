#ifndef DATAWRITER_H
#define DATAWRITER_H

#include <iterator>

namespace decore
{

/**
 * @brief Abstract data writer
 */
class DataWriter
{
public:
    DataWriter();
    virtual ~DataWriter();

    /**
     * @brief Writes `value`
     * @param value value to write
     */
    template<typename T>
    void write(const T& value)
    {
        write(static_cast<const void*>(&value), sizeof(value));
    }

    template<typename T>
    void write(T begin, T end)
    {
        typename T::difference_type elementsCount = std::distance(begin, end);
        write(&elementsCount, sizeof(elementsCount));
        for (T it = begin; it != end; ++it) {
            write(&*it, sizeof(*it));
        }
    }

protected:
    /**
     * @brief Writes `data` with the `dataSizeBytes`
     * @param data pointer to first data byte
     * @param dataSizeBytes size of the data in bytes
     */
    virtual void write(const void* data, unsigned int dataSizeBytes) = 0;
};

}

#endif /* DATAWRITER_H */

