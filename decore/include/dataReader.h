#ifndef DATAREADER_H
#define DATAREADER_H

namespace decore
{

/**
 * @brief Abstract data reader
 *
 * Implementation must handle sequential calls to `read`.
 *
 * DataReader/DataWriter used together to save some state and allow to restore any data from the saved state.
 *
 * Exact implementation of the abstractions is a platform specific and out of decore's scope.
 *
 * Following use case is assumed:
 *
 *      int a = 0, b = 1, c = 2;
 *      writer.write(a);
 *      writer.write(b);
 *      writer.write(c);
 *
 *      int d, e, f;
 *      reader.read(d);
 *      reader.read(e);
 *      reader.read(f);
 * `d`, `e` and `f` should contain 0, 1 and 2 respectively
 * @see DataWriter
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
        read(static_cast<void*>(&value), sizeof(value));
    }

    template <typename T, typename V>
    void read(T& container, const V& defaultValue)
    {
        typename T::size_type amount;
        read(amount);
        while (amount--) {
            V value(defaultValue);
            read(value);
            container.insert(container.begin(), value);
        }
    }

    virtual unsigned int position() const = 0;
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

