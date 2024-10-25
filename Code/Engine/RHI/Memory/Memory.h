#pragma once

#include <RHI/RHIDLL.h>

#include <RHI/Types/BasicTypes.h>

class PL_RHI_DLL plRHIMemory : public plRefCounted
{
public:
	virtual ~plRHIMemory() = default;
	virtual plRHIMemoryType GetMemoryType() const = 0;
};