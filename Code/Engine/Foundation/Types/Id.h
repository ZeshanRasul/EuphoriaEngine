#pragma once

/// \file

#include <Foundation/Basics.h>

/// \brief Declares an id type, see generic id below how to use this
#define PL_DECLARE_ID_TYPE(name, instanceIndexBits, generationBits)                                                     \
  static const StorageType MAX_INSTANCES = (1ULL << instanceIndexBits);                                                 \
  static const StorageType INVALID_INSTANCE_INDEX = MAX_INSTANCES - 1;                                                  \
  static const StorageType INDEX_AND_GENERATION_MASK = ((plUInt64) - 1) >> (64 - (instanceIndexBits + generationBits)); \
  PL_DECLARE_POD_TYPE();                                                                                                \
  PL_ALWAYS_INLINE name()                                                                                               \
  {                                                                                                                     \
    m_Data = INVALID_INSTANCE_INDEX;                                                                                    \
  }                                                                                                                     \
  PL_ALWAYS_INLINE explicit name(StorageType internalData)                                                              \
  {                                                                                                                     \
    m_Data = internalData;                                                                                              \
  }                                                                                                                     \
  PL_ALWAYS_INLINE bool operator==(const name other) const                                                              \
  {                                                                                                                     \
    return m_Data == other.m_Data;                                                                                      \
  }                                                                                                                     \
  PL_ALWAYS_INLINE bool operator!=(const name other) const                                                              \
  {                                                                                                                     \
    return m_Data != other.m_Data;                                                                                      \
  }                                                                                                                     \
  PL_ALWAYS_INLINE bool operator<(const name other) const                                                               \
  {                                                                                                                     \
    return m_Data < other.m_Data;                                                                                       \
  }                                                                                                                     \
  PL_ALWAYS_INLINE void Invalidate()                                                                                    \
  {                                                                                                                     \
    m_Data = INVALID_INSTANCE_INDEX;                                                                                    \
  }                                                                                                                     \
  PL_ALWAYS_INLINE bool IsInvalidated() const                                                                           \
  {                                                                                                                     \
    return m_Data == INVALID_INSTANCE_INDEX;                                                                            \
  }                                                                                                                     \
  PL_ALWAYS_INLINE bool IsIndexAndGenerationEqual(const name other) const                                               \
  {                                                                                                                     \
    return (m_Data & INDEX_AND_GENERATION_MASK) == (other.m_Data & INDEX_AND_GENERATION_MASK);                          \
  }


/// \brief A generic id class that holds an id combined of an instance index and a generation counter.
///
/// \todo Document this better.
template <plUInt32 InstanceIndexBits, plUInt32 GenerationBits>
struct plGenericId
{
  enum
  {
    STORAGE_SIZE = ((InstanceIndexBits + GenerationBits - 1) / 8) + 1
  };
  using StorageType = typename plSizeToType<STORAGE_SIZE>::Type;

  PL_DECLARE_ID_TYPE(plGenericId, InstanceIndexBits, GenerationBits);

  PL_ALWAYS_INLINE plGenericId(StorageType instanceIndex, StorageType generation)
  {
    m_Data = 0;
    m_InstanceIndex = instanceIndex;
    m_Generation = generation;
  }

  union
  {
    StorageType m_Data;
    struct
    {
      StorageType m_InstanceIndex : InstanceIndexBits;
      StorageType m_Generation : GenerationBits;
    };
  };
};

#define PL_DECLARE_HANDLE_TYPE(name, idType)               \
public:                                                    \
  PL_DECLARE_POD_TYPE();                                   \
  PL_ALWAYS_INLINE name() {}                               \
  PL_ALWAYS_INLINE explicit name(idType internalId)        \
    : m_InternalId(internalId)                             \
  {                                                        \
  }                                                        \
  PL_ALWAYS_INLINE bool operator==(const name other) const \
  {                                                        \
    return m_InternalId == other.m_InternalId;             \
  }                                                        \
  PL_ALWAYS_INLINE bool operator!=(const name other) const \
  {                                                        \
    return m_InternalId != other.m_InternalId;             \
  }                                                        \
  PL_ALWAYS_INLINE bool operator<(const name other) const  \
  {                                                        \
    return m_InternalId < other.m_InternalId;              \
  }                                                        \
  PL_ALWAYS_INLINE void Invalidate()                       \
  {                                                        \
    m_InternalId.Invalidate();                             \
  }                                                        \
  PL_ALWAYS_INLINE bool IsInvalidated() const              \
  {                                                        \
    return m_InternalId.IsInvalidated();                   \
  }                                                        \
  PL_ALWAYS_INLINE idType GetInternalID() const            \
  {                                                        \
    return m_InternalId;                                   \
  }                                                        \
  using IdType = idType;                                   \
                                                           \
protected:                                                 \
  idType m_InternalId;                                     \
  operator idType()                                        \
  {                                                        \
    return m_InternalId;                                   \
  }                                                        \
  operator const idType() const                            \
  {                                                        \
    return m_InternalId;                                   \
  }