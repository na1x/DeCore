#ifndef DATAREADER_H
#define DATAREADER_H

namespace decore
{

/**
 * @brief Abstract data reader
 */
class DataReader
{
public:
    DataReader();
    virtual ~DataReader();

    /**
     * @brief Reads value
     * @param value value to read
     */
    template <typename T>
    void read(T& value)
    {
        read(static_cast<void*>(value), sizeof(value));
    }

protected:
    /**
     * @brief Reads value
     * @param data destination data pointer
     * @param dataSizeBytes size of data to read in bytes
     */
    virtual void read(void* data, unsigned int dataSizeBytes) = 0;
};

}
#endif /* DATAREADER_H */

