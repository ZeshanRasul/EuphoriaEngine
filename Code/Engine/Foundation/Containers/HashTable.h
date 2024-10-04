#pragma once

#include <Foundation/Algorithm/HashingUtils.h>
#include <Foundation/Math/Math.h>
#include <Foundation/Memory/AllocatorWrapper.h>

template <typename KeyType, typename ValueType, typename Hasher>
class plHashTableBase;

/// \brief Const iterator.
template <typename KeyType, typename ValueType, typename Hasher>
struct plHashTableBaseConstIterator
{
  using iterator_category = std::forward_iterator_tag;
  using value_type = plHashTableBaseConstIterator;
  using difference_type = std::ptrdiff_t;
  using pointer = plHashTableBaseConstIterator*;
  using reference = plHashTableBaseConstIterator&;

  PL_DECLARE_POD_TYPE();

  plHashTableBaseConstIterator() = default;

  /// \brief Checks whether this iterator points to a valid element.
  bool IsValid() const; // [tested]

  /// \brief Checks whether the two iterators point to the same element.
  bool operator==(const plHashTableBaseConstIterator& rhs) const;
  PL_ADD_DEFAULT_OPERATOR_NOTEQUAL(const plHashTableBaseConstIterator&);

  /// \brief Returns the 'key' of the element that this iterator points to.
  const KeyType& Key() const; // [tested]

  /// \brief Returns the 'value' of the element that this iterator points to.
  const ValueType& Value() const; // [tested]

  /// \brief Advances the iterator to the next element in the map. The iterator will not be valid anymore, if the end is reached.
  void Next(); // [tested]

  /// \brief Shorthand for 'Next'
  void operator++(); // [tested]

  /// \brief Returns '*this' to enable foreach
  PL_ALWAYS_INLINE plHashTableBaseConstIterator& operator*() { return *this; } // [tested]

protected:
  friend class plHashTableBase<KeyType, ValueType, Hasher>;

  explicit plHashTableBaseConstIterator(const plHashTableBase<KeyType, ValueType, Hasher>& hashTable);
  void SetToBegin();
  void SetToEnd();

  const plHashTableBase<KeyType, ValueType, Hasher>* m_pHashTable = nullptr;
  plUInt32 m_uiCurrentIndex = 0; // current element index that this iterator points to.
  plUInt32 m_uiCurrentCount = 0; // current number of valid elements that this iterator has found so far.

#if PL_ENABLED(PL_USE_CPP20_OPERATORS)
public:
  struct Pointer
  {
    std::pair<const KeyType&, const ValueType&> value;
    const std::pair<const KeyType&, const ValueType&>* operator->() const { return &value; }
  };

  PL_ALWAYS_INLINE Pointer operator->() const
  {
    return Pointer{.value = {Key(), Value()}};
  }

  // These function is used to return the values for structured bindings.
  // The number and type of type of each slot are defined in the inl file.
  template <std::size_t Index>
  std::tuple_element_t<Index, plHashTableBaseConstIterator>& get() const
  {
    if constexpr (Index == 0)
      return Key();
    if constexpr (Index == 1)
      return Value();
  }
#endif
};

/// \brief Iterator with write access.
template <typename KeyType, typename ValueType, typename Hasher>
struct plHashTableBaseIterator : public plHashTableBaseConstIterator<KeyType, ValueType, Hasher>
{
  PL_DECLARE_POD_TYPE();

  /// \brief Creates a new iterator from another.
  PL_ALWAYS_INLINE plHashTableBaseIterator(const plHashTableBaseIterator& rhs); // [tested]

  /// \brief Assigns one iterator no another.
  PL_ALWAYS_INLINE void operator=(const plHashTableBaseIterator& rhs); // [tested]

  // this is required to pull in the const version of this function
  using plHashTableBaseConstIterator<KeyType, ValueType, Hasher>::Value;

  /// \brief Returns the 'value' of the element that this iterator points to.
  PL_FORCE_INLINE ValueType& Value(); // [tested]

  /// \brief Returns the 'value' of the element that this iterator points to.
  PL_FORCE_INLINE ValueType& Value() const;

  /// \brief Returns '*this' to enable foreach
  PL_ALWAYS_INLINE plHashTableBaseIterator& operator*() { return *this; } // [tested]

private:
  friend class plHashTableBase<KeyType, ValueType, Hasher>;

  explicit plHashTableBaseIterator(const plHashTableBase<KeyType, ValueType, Hasher>& hashTable);

#if PL_ENABLED(PL_USE_CPP20_OPERATORS)
public:
  struct Pointer
  {
    std::pair<const KeyType&, ValueType&> value;
    const std::pair<const KeyType&, ValueType&>* operator->() const { return &value; }
  };

  PL_ALWAYS_INLINE Pointer operator->() const
  {
    return Pointer{.value = {plHashTableBaseConstIterator<KeyType, ValueType, Hasher>::Key(), Value()}};
  }

  // These functions are used to return the values for structured bindings.
  // The number and type of type of each slot are defined in the inl file.
  template <std::size_t Index>
  std::tuple_element_t<Index, plHashTableBaseIterator>& get()
  {
    if constexpr (Index == 0)
      return plHashTableBaseConstIterator<KeyType, ValueType, Hasher>::Key();
    if constexpr (Index == 1)
      return Value();
  }

  template <std::size_t Index>
  std::tuple_element_t<Index, plHashTableBaseIterator>& get() const
  {
    if constexpr (Index == 0)
      return plHashTableBaseConstIterator<KeyType, ValueType, Hasher>::Key();
    if constexpr (Index == 1)
      return Value();
  }
#endif
};

/// \brief Implementation of a hashtable which stores key/value pairs.
///
/// The hashtable maps keys to values by using the hash of the key as an index into the table.
/// This implementation uses linear-probing to resolve hash collisions which means all key/value pairs are stored
/// in a linear array.
/// All insertion/erasure/lookup functions take O(1) time if the table does not need to be expanded,
/// which happens when the load gets greater than 60%.
/// The hash function can be customized by providing a Hasher helper class like plHashHelper.

/// \see plHashHelper
template <typename KeyType, typename ValueType, typename Hasher>
class plHashTableBase
{
public:
  using Iterator = plHashTableBaseIterator<KeyType, ValueType, Hasher>;
  using ConstIterator = plHashTableBaseConstIterator<KeyType, ValueType, Hasher>;

protected:
  /// \brief Creates an empty hashtable. Does not allocate any data yet.
  explicit plHashTableBase(plAllocator* pAllocator); // [tested]

  /// \brief Creates a copy of the given hashtable.
  plHashTableBase(const plHashTableBase<KeyType, ValueType, Hasher>& rhs, plAllocator* pAllocator); // [tested]

  /// \brief Moves data from an existing hashtable into this one.
  plHashTableBase(plHashTableBase<KeyType, ValueType, Hasher>&& rhs, plAllocator* pAllocator); // [tested]

  /// \brief Destructor.
  ~plHashTableBase(); // [tested]

  /// \brief Copies the data from another hashtable into this one.
  void operator=(const plHashTableBase<KeyType, ValueType, Hasher>& rhs); // [tested]

  /// \brief Moves data from an existing hashtable into this one.
  void operator=(plHashTableBase<KeyType, ValueType, Hasher>&& rhs); // [tested]

public:
  /// \brief Compares this table to another table.
  bool operator==(const plHashTableBase<KeyType, ValueType, Hasher>& rhs) const; // [tested]
  PL_ADD_DEFAULT_OPERATOR_NOTEQUAL(const plHashTableBase<KeyType, ValueType, Hasher>&);

  /// \brief Expands the hashtable by over-allocating the internal storage so that the load factor is lower or equal to 60% when inserting the given
  /// number of entries.
  void Reserve(plUInt32 uiCapacity); // [tested]

  /// \brief Tries to compact the hashtable to avoid wasting memory.
  ///
  /// The resulting capacity is at least 'GetCount' (no elements get removed).
  /// Will deallocate all data, if the hashtable is empty.
  void Compact(); // [tested]

  /// \brief Returns the number of active entries in the table.
  plUInt32 GetCount() const; // [tested]

  /// \brief Returns true, if the hashtable does not contain any elements.
  bool IsEmpty() const; // [tested]

  /// \brief Clears the table.
  void Clear(); // [tested]

  /// \brief Inserts the key value pair or replaces value if an entry with the given key already exists.
  ///
  /// Returns true if an existing value was replaced and optionally writes out the old value to out_oldValue.
  template <typename CompatibleKeyType, typename CompatibleValueType>
  bool Insert(CompatibleKeyType&& key, CompatibleValueType&& value, ValueType* out_pOldValue = nullptr); // [tested]

  /// \brief Removes the entry with the given key. Returns whether an entry was removed and optionally writes out the old value to out_oldValue.
  template <typename CompatibleKeyType>
  bool Remove(const CompatibleKeyType& key, ValueType* out_pOldValue = nullptr); // [tested]

  /// \brief Erases the key/value pair at the given Iterator. Returns an iterator to the element after the given iterator.
  Iterator Remove(const Iterator& pos); // [tested]

  /// \brief Cannot remove an element with just a plHashTableBaseConstIterator
  void Remove(const ConstIterator& pos) = delete;

  /// \brief Returns whether an entry with the given key was found and if found writes out the corresponding value to out_value.
  template <typename CompatibleKeyType>
  bool TryGetValue(const CompatibleKeyType& key, ValueType& out_value) const; // [tested]

  /// \brief Returns whether an entry with the given key was found and if found writes out the pointer to the corresponding value to out_pValue.
  template <typename CompatibleKeyType>
  bool TryGetValue(const CompatibleKeyType& key, const ValueType*& out_pValue) const; // [tested]

  /// \brief Returns whether an entry with the given key was found and if found writes out the pointer to the corresponding value to out_pValue.
  template <typename CompatibleKeyType>
  bool TryGetValue(const CompatibleKeyType& key, ValueType*& out_pValue) const; // [tested]

  /// \brief Searches for key, returns a plHashTableBaseConstIterator to it or an invalid iterator, if no such key is found. O(1) operation.
  template <typename CompatibleKeyType>
  ConstIterator Find(const CompatibleKeyType& key) const;

  /// \brief Searches for key, returns an Iterator to it or an invalid iterator, if no such key is found. O(1) operation.
  template <typename CompatibleKeyType>
  Iterator Find(const CompatibleKeyType& key);

  /// \brief Returns a pointer to the value of the entry with the given key if found, otherwise returns nullptr.
  template <typename CompatibleKeyType>
  const ValueType* GetValue(const CompatibleKeyType& key) const; // [tested]

  /// \brief Returns a pointer to the value of the entry with the given key if found, otherwise returns nullptr.
  template <typename CompatibleKeyType>
  ValueType* GetValue(const CompatibleKeyType& key); // [tested]

  /// \brief Returns the value to the given key if found or creates a new entry with the given key and a default constructed value.
  ValueType& operator[](const KeyType& key); // [tested]

  /// \brief Returns the value stored at the given key. If none exists, one is created. \a bExisted indicates whether an element needed to be created.
  ValueType& FindOrAdd(const KeyType& key, bool* out_pExisted = nullptr); // [tested]

  /// \brief Returns if an entry with given key exists in the table.
  template <typename CompatibleKeyType>
  bool Contains(const CompatibleKeyType& key) const; // [tested]

  /// \brief Returns an Iterator to the very first element.
  Iterator GetIterator(); // [tested]

  /// \brief Returns an Iterator to the first element that is not part of the hash-table. Needed to support range based for loops.
  Iterator GetEndIterator(); // [tested]

  /// \brief Returns a constant Iterator to the very first element.
  ConstIterator GetIterator() const; // [tested]

  /// \brief Returns a plHashTableBaseConstIterator to the first element that is not part of the hash-table. Needed to support range based for loops.
  ConstIterator GetEndIterator() const; // [tested]

  /// \brief Returns the allocator that is used by this instance.
  plAllocator* GetAllocator() const;

  /// \brief Returns the amount of bytes that are currently allocated on the heap.
  plUInt64 GetHeapMemoryUsage() const; // [tested]

  /// \brief Swaps this map with the other one.
  void Swap(plHashTableBase<KeyType, ValueType, Hasher>& other); // [tested]

private:
  friend struct plHashTableBaseConstIterator<KeyType, ValueType, Hasher>;
  friend struct plHashTableBaseIterator<KeyType, ValueType, Hasher>;

  struct Entry
  {
    KeyType key;
    ValueType value;
  };

  Entry* m_pEntries = nullptr;
  plUInt32* m_pEntryFlags = nullptr;

  plUInt32 m_uiCount = 0;
  plUInt32 m_uiCapacity = 0;

  plAllocator* m_pAllocator = nullptr;

  enum
  {
    FREE_ENTRY = 0,
    VALID_ENTRY = 1,
    DELETED_ENTRY = 2,
    FLAGS_MASK = 3,
    CAPACITY_ALIGNMENT = 32
  };

  void SetCapacity(plUInt32 uiCapacity);

  void RemoveInternal(plUInt32 uiIndex);

  template <typename CompatibleKeyType>
  plUInt32 FindEntry(const CompatibleKeyType& key) const;

  template <typename CompatibleKeyType>
  plUInt32 FindEntry(plUInt32 uiHash, const CompatibleKeyType& key) const;

  plUInt32 GetFlagsCapacity() const;
  plUInt32 GetFlags(plUInt32* pFlags, plUInt32 uiEntryIndex) const;
  void SetFlags(plUInt32 uiEntryIndex, plUInt32 uiFlags);

  bool IsFreeEntry(plUInt32 uiEntryIndex) const;
  bool IsValidEntry(plUInt32 uiEntryIndex) const;
  bool IsDeletedEntry(plUInt32 uiEntryIndex) const;

  void MarkEntryAsFree(plUInt32 uiEntryIndex);
  void MarkEntryAsValid(plUInt32 uiEntryIndex);
  void MarkEntryAsDeleted(plUInt32 uiEntryIndex);
};

/// \brief \see plHashTableBase
template <typename KeyType, typename ValueType, typename Hasher = plHashHelper<KeyType>, typename AllocatorWrapper = plDefaultAllocatorWrapper>
class plHashTable : public plHashTableBase<KeyType, ValueType, Hasher>
{
public:
  plHashTable();
  explicit plHashTable(plAllocator* pAllocator);

  plHashTable(const plHashTable<KeyType, ValueType, Hasher, AllocatorWrapper>& other);
  plHashTable(const plHashTableBase<KeyType, ValueType, Hasher>& other);

  plHashTable(plHashTable<KeyType, ValueType, Hasher, AllocatorWrapper>&& other);
  plHashTable(plHashTableBase<KeyType, ValueType, Hasher>&& other);


  void operator=(const plHashTable<KeyType, ValueType, Hasher, AllocatorWrapper>& rhs);
  void operator=(const plHashTableBase<KeyType, ValueType, Hasher>& rhs);

  void operator=(plHashTable<KeyType, ValueType, Hasher, AllocatorWrapper>&& rhs);
  void operator=(plHashTableBase<KeyType, ValueType, Hasher>&& rhs);
};

//////////////////////////////////////////////////////////////////////////
// begin() /end() for range-based for-loop support

template <typename KeyType, typename ValueType, typename Hasher>
typename plHashTableBase<KeyType, ValueType, Hasher>::Iterator begin(plHashTableBase<KeyType, ValueType, Hasher>& ref_container)
{
  return ref_container.GetIterator();
}

template <typename KeyType, typename ValueType, typename Hasher>
typename plHashTableBase<KeyType, ValueType, Hasher>::ConstIterator begin(const plHashTableBase<KeyType, ValueType, Hasher>& container)
{
  return container.GetIterator();
}

template <typename KeyType, typename ValueType, typename Hasher>
typename plHashTableBase<KeyType, ValueType, Hasher>::ConstIterator cbegin(const plHashTableBase<KeyType, ValueType, Hasher>& container)
{
  return container.GetIterator();
}

template <typename KeyType, typename ValueType, typename Hasher>
typename plHashTableBase<KeyType, ValueType, Hasher>::Iterator end(plHashTableBase<KeyType, ValueType, Hasher>& ref_container)
{
  return ref_container.GetEndIterator();
}

template <typename KeyType, typename ValueType, typename Hasher>
typename plHashTableBase<KeyType, ValueType, Hasher>::ConstIterator end(const plHashTableBase<KeyType, ValueType, Hasher>& container)
{
  return container.GetEndIterator();
}

template <typename KeyType, typename ValueType, typename Hasher>
typename plHashTableBase<KeyType, ValueType, Hasher>::ConstIterator cend(const plHashTableBase<KeyType, ValueType, Hasher>& container)
{
  return container.GetEndIterator();
}

#include <Foundation/Containers/Implementation/HashTable_inl.h>
