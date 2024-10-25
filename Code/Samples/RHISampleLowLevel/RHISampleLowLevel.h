#pragma once

#include <memory>
#include <Foundation/Application/Application.h>
#include <Foundation/Types/UniquePtr.h>

#include <RHI/Types/BasicTypes.h>

class plRHISampleWindow;
class plRHIInstance;
class plRHIAdapter;

constexpr plUInt32 FRAME_COUNT = 3;

class plRHISampleApp : public plApplication
{
public:
  typedef plApplication SUPER;

  plRHISampleApp();

  virtual Execution Run() override;

  virtual void AfterCoreSystemsStartup() override;

  virtual void BeforeHighLevelSystemsShutdown() override;

  void OnResize(plUInt32 width, plUInt32 height);

private:
  plRHISampleWindow* m_pWindow = nullptr;

  plSharedPtr<plRHIInstance> m_pInstance;
  plSharedPtr<plRHIAdapter> m_pAdapter;
  plSharedPtr<plRHIDevice> m_pDevice;

  plRHIRenderDeviceDesc m_RenderDeviceDesc;
};
