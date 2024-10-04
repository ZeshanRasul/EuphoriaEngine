#include <Foundation/Containers/Implementation/BitIterator.h>

/// Helper base class to iterate over the bit indices or bit values of an integer.
/// \tparam DataType The type of data that is being iterated over.
/// \tparam ReturnsIndex If set, returns the index of the bit. Otherwise returns the value of the bit, i.e. PL_BIT(value).
/// \tparam ReturnType Returned value type of the iterator.
/// \sa plIterateBitValues, plIterateBitIndices
template <typename DataType, bool ReturnsIndex, typename ReturnType = DataType>
struct plIterateBits
{
  explicit plIterateBits(DataType data)
  {
    m_Data = data;
  }

  plBitIterator<DataType, ReturnsIndex, ReturnType> begin() const
  {
    return plBitIterator<DataType, ReturnsIndex, ReturnType>(m_Data);
  };

  plBitIterator<DataType, ReturnsIndex, ReturnType> end() const
  {
    return plBitIterator<DataType, ReturnsIndex, ReturnType>();
  };

  DataType m_Data = {};
};

/// \brief Helper class to iterate over the bit values of an integer.
/// The class can iterate over the bits of any unsigned integer type that is equal to or smaller than plUInt64.
/// \code{.cpp}
///    plUInt64 bits = 0b1101;
///    for (auto bit : plIterateBitValues(bits))
///    {
///      plLog::Info("{}", bit); // Outputs 1, 4, 8
///    }
/// \endcode
/// \tparam DataType The type of data that is being iterated over.
/// \tparam ReturnType Returned value type of the iterator. Defaults to same as DataType.
template <typename DataType, typename ReturnType = DataType>
struct plIterateBitValues : public plIterateBits<DataType, false, ReturnType>
{
  explicit plIterateBitValues(DataType data)
    : plIterateBits<DataType, false, ReturnType>(data)
  {
  }
};

/// \brief Helper class to iterate over the bit indices of an integer.
/// The class can iterate over the bits of any unsigned integer type that is equal to or smaller than plUInt64.
/// \code{.cpp}
///    plUInt64 bits = 0b1101;
///    for (auto bit : plIterateBitIndices(bits))
///    {
///      plLog::Info("{}", bit); // Outputs 0, 2, 3
///    }
/// \endcode
/// \tparam DataType The type of data that is being iterated over.
/// \tparam ReturnType Returned value type of the iterator. Defaults to same as DataType.
template <typename DataType, typename ReturnType = DataType>
struct plIterateBitIndices : public plIterateBits<DataType, true, ReturnType>
{
  explicit plIterateBitIndices(DataType data)
    : plIterateBits<DataType, true, ReturnType>(data)
  {
  }
};