#pragma once
#include <RHI/RHIDLL.h>

#include <RHI/Adapter/Adapter.h>

class plRHIInstance;

static plSharedPtr<plRHIInstance> g_GraphicsInstance = nullptr;

class PL_RHI_DLL plRHIInstance : public plRefCounted
{
public:
    virtual ~plRHIInstance() = default;
	virtual plDynamicArray<plSharedPtr<plRHIAdapter>> EnumerateAdapters() = 0;

};

struct PL_RHI_DLL plRHIInstanceFactory
{
  using CreatorFunc = plDelegate<plSharedPtr<plRHIInstance>()>;

  static plSharedPtr<plRHIInstance> CreateInstance(plRHIApiType type);

  static void RegisterCreatorFunc(plRHIApiType type, const CreatorFunc& func);
  static void UnregisterCreatorFunc(plRHIApiType type);
};

