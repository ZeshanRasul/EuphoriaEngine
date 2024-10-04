template <plAllocatorTrackingMode TrackingMode, bool OverwriteMemoryOnReset>
plLinearAllocator<TrackingMode, OverwriteMemoryOnReset>::plLinearAllocator(plStringView sName, plAllocator* pParent)
  : plAllocatorWithPolicy<typename plLinearAllocator<TrackingMode, OverwriteMemoryOnReset>::PolicyStack, TrackingMode>(sName, pParent)
  , m_DestructData(pParent)
  , m_PtrToDestructDataIndexTable(pParent)
{
}

template <plAllocatorTrackingMode TrackingMode, bool OverwriteMemoryOnReset>
plLinearAllocator<TrackingMode, OverwriteMemoryOnReset>::~plLinearAllocator()
{
  Reset();
}

template <plAllocatorTrackingMode TrackingMode, bool OverwriteMemoryOnReset>
void* plLinearAllocator<TrackingMode, OverwriteMemoryOnReset>::Allocate(size_t uiSize, size_t uiAlign, plMemoryUtils::DestructorFunction destructorFunc)
{
  PL_LOCK(m_Mutex);

  void* ptr = plAllocatorWithPolicy<typename plLinearAllocator<TrackingMode, OverwriteMemoryOnReset>::PolicyStack, TrackingMode>::Allocate(uiSize, uiAlign, destructorFunc);

  if (destructorFunc != nullptr)
  {
    plUInt32 uiIndex = m_DestructData.GetCount();
    m_PtrToDestructDataIndexTable.Insert(ptr, uiIndex);

    auto& data = m_DestructData.ExpandAndGetRef();
    data.m_Func = destructorFunc;
    data.m_Ptr = ptr;
  }

  return ptr;
}

template <plAllocatorTrackingMode TrackingMode, bool OverwriteMemoryOnReset>
void plLinearAllocator<TrackingMode, OverwriteMemoryOnReset>::Deallocate(void* pPtr)
{
  PL_LOCK(m_Mutex);

  plUInt32 uiIndex;
  if (m_PtrToDestructDataIndexTable.Remove(pPtr, &uiIndex))
  {
    auto& data = m_DestructData[uiIndex];
    data.m_Func = nullptr;
    data.m_Ptr = nullptr;
  }

  plAllocatorWithPolicy<typename plLinearAllocator<TrackingMode, OverwriteMemoryOnReset>::PolicyStack, TrackingMode>::Deallocate(pPtr);
}

PL_MSVC_ANALYSIS_WARNING_PUSH

// Disable warning for incorrect operator (compiler complains about the TrackingMode bitwise and in the case that flags = None)
// even with the added guard of a check that it can't be 0.
PL_MSVC_ANALYSIS_WARNING_DISABLE(6313)

template <plAllocatorTrackingMode TrackingMode, bool OverwriteMemoryOnReset>
void plLinearAllocator<TrackingMode, OverwriteMemoryOnReset>::Reset()
{
  PL_LOCK(m_Mutex);

  for (plUInt32 i = m_DestructData.GetCount(); i-- > 0;)
  {
    auto& data = m_DestructData[i];
    if (data.m_Func != nullptr)
      data.m_Func(data.m_Ptr);
  }

  m_DestructData.Clear();
  m_PtrToDestructDataIndexTable.Clear();

  this->m_allocator.Reset();
  if constexpr (TrackingMode >= plAllocatorTrackingMode::AllocationStats)
  {
    plMemoryTracker::RemoveAllAllocations(this->m_Id);
  }
  else if constexpr (TrackingMode >= plAllocatorTrackingMode::Basics)
  {
    plAllocator::Stats stats;
    this->m_allocator.FillStats(stats);

    plMemoryTracker::SetAllocatorStats(this->m_Id, stats);
  }
}
PL_MSVC_ANALYSIS_WARNING_POP
