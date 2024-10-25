#pragma once

#include <RHI/RHIDLL.h>

class PL_RHI_DLL plRHICommandQueue : public plRefCounted
{
public:
	virtual ~plRHICommandQueue() = default;
	virtual void Wait(const plSharedPtr<plRHIFence>& fence, plUInt64 value) = 0;
	virtual void Submit(const plSharedPtr<plRHIFence>& fence, plUInt64 value) = 0;
	virtual void ExecuteCommandList(const plDynamicArray<plSharedPtr<plRHICommandList>>& commandLists) = 0;
};