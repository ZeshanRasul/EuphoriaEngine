#pragma once

#include <Foundation/Math/Math.h>

/// Chooses either plUInt32 or plUInt64 as the storage type for a given type T depending on its size. Required as plMath::FirstBitLow only supports plUInt32 or plUInt64.
/// \tparam T Type for which the storage should be inferred.
template <typename T, typename = std::void_t<>>
struct plBitIteratorStorage;
template <typename T>
struct plBitIteratorStorage<T, std::enable_if_t<sizeof(T) <= 4>>
{
  using Type = plUInt32;
};
template <typename T>
struct plBitIteratorStorage<T, std::enable_if_t<sizeof(T) >= 5>>
{
  using Type = plUInt64;
};

/// Configurable bit iterator. Allows for iterating over the bits in an integer, returning either the bit index or value.
/// \tparam DataType The type of data that is being iterated over.
/// \tparam ReturnsIndex If set, returns the index of the bit. Otherwise returns the value of the bit, i.e. PL_BIT(value).
/// \tparam ReturnType Returned value type of the iterator. Defaults to same as DataType.
/// \tparam StorageType The storage type that the bit operations are performed on (either plUInt32 or plUInt64). Auto-computed.
template <typename DataType, bool ReturnsIndex = true, typename ReturnType = DataType, typename StorageType = typename plBitIteratorStorage<DataType>::Type>
struct plBitIterator
{
  using iterator_category = std::forward_iterator_tag;
  using value_type = DataType;
  static_assert(sizeof(DataType) <= 8);

  // Invalid iterator (end)
  PL_ALWAYS_INLINE plBitIterator() = default;

  // Start iterator.
  PL_ALWAYS_INLINE explicit plBitIterator(DataType data)
  {
    m_uiMask = static_cast<StorageType>(data);
  }

  PL_ALWAYS_INLINE bool IsValid() const
  {
    return m_uiMask != 0;
  }

  PL_ALWAYS_INLINE ReturnType Value() const
  {
    if constexpr (ReturnsIndex)
    {
      return static_cast<ReturnType>(plMath::FirstBitLow(m_uiMask));
    }
    else
    {
      return static_cast<ReturnType>(PL_BIT(plMath::FirstBitLow(m_uiMask)));
    }
  }

  PL_ALWAYS_INLINE void Next()
  {
    // Clear the lowest set bit. Why this works: https://www.geeksforgeeks.org/turn-off-the-rightmost-set-bit/
    m_uiMask = m_uiMask & (m_uiMask - 1);
  }

  PL_ALWAYS_INLINE bool operator==(const plBitIterator& other) const
  {
    return m_uiMask == other.m_uiMask;
  }

  PL_ALWAYS_INLINE bool operator!=(const plBitIterator& other) const
  {
    return m_uiMask != other.m_uiMask;
  }

  PL_ALWAYS_INLINE ReturnType operator*() const
  {
    return Value();
  }

  PL_ALWAYS_INLINE void operator++()
  {
    Next();
  }

  StorageType m_uiMask = 0;
};
