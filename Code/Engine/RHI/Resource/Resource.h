#pragma once

#include <RHI/RHIDLL.h>

#include <RHI/Memory/Memory.h>

struct PL_RHI_DLL plRHIMemoryRequirements
{
	plUInt64 m_uiSize;
	plUInt64 m_uiAlignment;
	plUInt32 m_uiMemoryTypeBits;
};

class PL_RHI_DLL plRHIResource : public plRefCounted
{
public:
	virtual ~plRHIResource() = default;
	virtual void CommitMemory(plRHIMemoryType memoryType) = 0;
	virtual void BindMemory(const plSharedPtr<plRHIMemory>& memory, plUInt64 offset) = 0;
	virtual plRHIResourceType GetResourceType() const = 0;
	virtual plRHIResourceFormat::Enum GetFormat() const = 0;
	virtual plRHIMemoryType GetMemoryType() const = 0;
	virtual plUInt64 GetWidth() const = 0;
	virtual plUInt64 GetHeight() const = 0;
	virtual plUInt16 GetLayerCount() const = 0;
	virtual plUInt16 GetLevelCount() const = 0;
	virtual plUInt32 GetSampleCount() const = 0;
	virtual plUInt64 GetAccelerationStructureHandle() const = 0;
	virtual void SetName(plStringView name) = 0;
	virtual plUInt8* Map() = 0;
	virtual void Unmap() = 0;
	virtual void UpdateUploadBuffer(plUInt64 bufferOffset, const void* data, plUInt64 numBytes) = 0;
	virtual void UpdateUploadBufferWithTextureData(plUInt64 bufferOffset, plUInt32 bufferRowPitch, plUInt32 bufferDepthPitch,
		const void* srcData, plUInt32 srcRowPitch, plUInt32 srcDepthPitch, plUInt32 numRows, plUInt32 numSlices) = 0;
	virtual void AllowCommonStatePromotion(plRHIResourceState stateAfter) = 0;
	virtual plRHIResourceState GetInitialState() const = 0;
	virtual plRHIMemoryRequirements GetMemoryRequirements() const = 0;
	virtual bool IsBackBuffer() const = 0;
};