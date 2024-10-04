#pragma once

#include <memory>
#include <Foundation/Application/Application.h>
#include <Foundation/Types/UniquePtr.h>

class plRHISampleWindow;

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
};
