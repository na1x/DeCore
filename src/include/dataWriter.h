#ifndef DATAWRITER_H
#define DATAWRITER_H

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

