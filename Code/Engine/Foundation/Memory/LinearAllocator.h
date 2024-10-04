#pragma once

#include <Foundation/Containers/DynamicArray.h>
#include <Foundation/Containers/HashTable.h>
#include <Foundation/Memory/AllocatorWithPolicy.h>
#include <Foundation/Memory/Policies/AllocPolicyLinear.h>
#include <Foundation/Threading/Lock.h>
#include <Foundation/Threading/Mutex.h>

template <plAllocatorTrackingMode TrackingMode = plAllocatorTrackingMode::Default, bool OverwriteMemoryOnReset = false>
class plLinearAllocator : public plAllocatorWithPolicy<plAllocPolicyLinear<OverwriteMemoryOnReset>, TrackingMode>
{
  using PolicyStack = plAllocPolicyLinear<OverwriteMemoryOnReset>;

public:
  plLinearAllocator(plStringView sName, plAllocator* pParent);
  ~plLinearAllocator();

  virtual void* Allocate(size_t uiSize, size_t uiAlign, plMemoryUtils::DestructorFunction destructorFunc) override;
  virtual void Deallocate(void* pPtr) override;

  /// \brief
  ///   Resets the allocator freeing all memory.
  void Reset();

private:
  struct DestructData
  {
    PL_DECLARE_POD_TYPE();

    plMemoryUtils::DestructorFunction m_Func;
    void* m_Ptr;
  };

  plMutex m_Mutex;
  plDynamicArray<DestructData> m_DestructData;
  plHashTable<void*, plUInt32> m_PtrToDestructDataIndexTable;
};

#include <Foundation/Memory/Implementation/LinearAllocator_inl.h>
