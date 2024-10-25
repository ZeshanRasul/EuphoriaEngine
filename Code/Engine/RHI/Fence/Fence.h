#pragma once
#include <RHI/RHIDLL.h>

class PL_RHI_DLL plRHIFence : public plRefCounted
{
public:
	virtual ~plRHIFence() = default;
	virtual plUInt64 GetCompletedValue() = 0;
	virtual void Wait(plUInt64 value) = 0;
	virtual void Signal(plUInt64 value) = 0;
};