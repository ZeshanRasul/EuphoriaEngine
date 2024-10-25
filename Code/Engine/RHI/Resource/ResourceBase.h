#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/Resource/Resource.h>

class PL_RHI_DLL plRHIResourceBase : public plRHIResource
{
public:
	plRHIResourceBase();

	plRHIResourceType GetResourceType() const override final;
	plRHIResourceFormat::Enum GetFormat() const override final;
	plRHIMemoryType GetMemoryType() const override final;

	virtual void UpdateUploadBuffer(plUInt64 bufferOffset, const void* data, plUInt64 numBytes) override final;
	virtual void UpdateUploadBufferWithTextureData(plUInt64 bufferOffset, plUInt32 bufferRowPitch, plUInt32 bufferDepthPitch,
		const void* srcData, plUInt32 srcRowPitch, plUInt32 srcDepthPitch, plUInt32 numRows, plUInt32 numSlices) override final;
	virtual plRHIResourceState GetInitialState() const override final;
	virtual bool IsBackBuffer() const override final;
	void SetInitialState(plRHIResourceState state);
	plRHIResourceStateTracker& GetGlobalResourceStateTracker();
	const plRHIResourceStateTracker& GetGlobalResourceStateTracker() const;

	plRHIResourceFormat::Enum m_Format = plRHIResourceFormat::UNKNOWN;
	plRHIResourceType m_ResourceType = plRHIResourceType::kUnknown;
	plSharedPtr<plRHIResource> m_AccelerationStructureMemory;
	bool m_bBackBuffer = false;

protected:
	plSharedPtr<plRHIMemory> m_Memory;
	plRHIMemoryType m_MemoryType = plRHIMemoryType::kDefault;

private:
	plRHIResourceStateTracker m_ResourceStateTracker;
	plRHIResourceState m_InitialState = plRHIResourceState::kUnknown;
};