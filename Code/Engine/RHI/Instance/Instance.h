#pragma once
#include <RHI/RHIDLL.h>

enum class plRHIApiType
{
  kVulkan,
  kDX12,
  kMetal,
  kWebGPU
};

class plRHIInstance;

static plSharedPtr<plRHIInstance> g_GraphicsInstance = nullptr;

class PL_RHI_DLL plRHIInstance : public plRefCounted
{
public:
    virtual ~plRHIInstance() = default;
};

struct PL_RHI_DLL plRHIInstanceFactory
{
  using CreatorFunc = plDelegate<plSharedPtr<plRHIInstance>()>;

  static plSharedPtr<plRHIInstance> CreateInstance(plRHIApiType type);

  static void RegisterCreatorFunc(plRHIApiType type, const CreatorFunc& func);
  static void UnregisterCreatorFunc(plRHIApiType type);
};

