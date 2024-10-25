#pragma once

#include <RHI/RHIDLL.h>

class plRHIAdapter;

static plSharedPtr<plRHIAdapter> g_GraphicsAdapter = nullptr;

class PL_RHI_DLL plRHIAdapter : public plRefCounted
{
public:
	virtual ~plRHIAdapter();
	virtual const plString& GetName() const = 0;
	virtual plSharedPtr<plRHIDevice> CreateDevice() = 0;
};