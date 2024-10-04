#pragma once

#include <Foundation/Memory/Allocator.h>

struct plNullAllocatorWrapper
{
  PL_FORCE_INLINE static plAllocator* GetAllocator()
  {
    PL_REPORT_FAILURE("This method should never be called");
    return nullptr;
  }
};

struct plDefaultAllocatorWrapper
{
  PL_ALWAYS_INLINE static plAllocator* GetAllocator() { return plFoundation::GetDefaultAllocator(); }
};

struct plStaticsAllocatorWrapper
{
  PL_ALWAYS_INLINE static plAllocator* GetAllocator() { return plFoundation::GetStaticsAllocator(); }
};

struct plAlignedAllocatorWrapper
{
  PL_ALWAYS_INLINE static plAllocator* GetAllocator() { return plFoundation::GetAlignedAllocator(); }
};

/// \brief Helper function to facilitate setting the allocator on member containers of a class
/// Allocators can be either template arguments or a ctor parameter. Using the ctor parameter requires the class ctor to reference each member container in the initialization list. This can be very tedious. On the other hand, the template variant only support template parameter so you can't simply pass in a member allocator.
/// This class solves this problem provided the following rules are followed:
/// 1. The `plAllocator` must be the declared at the earliest in the class, before any container.
/// 2. The `plLocalAllocatorWrapper` should be declared right afterwards.
/// 3. Any container needs to be declared below these two and must include the `plLocalAllocatorWrapper` as a template argument to the allocator.
/// 4. In the ctor initializer list, init the plAllocator first, then the plLocalAllocatorWrapper. With this approach all containers can be omitted.
/// \code{.cpp}
///   class MyClass
///   {
///     plAllocator m_SpecialAlloc;
///     plLocalAllocatorWrapper m_Wrapper;
///
///     plDynamicArray<int, plLocalAllocatorWrapper> m_Data;
///
///     MyClass()
///       : m_SpecialAlloc("MySpecialAlloc")
///       , m_Wrapper(&m_SpecialAlloc)
///     {
///     }
///   }
/// \endcode
struct PL_FOUNDATION_DLL plLocalAllocatorWrapper
{
  plLocalAllocatorWrapper(plAllocator* pAllocator);

  void Reset();

  static plAllocator* GetAllocator();
};
