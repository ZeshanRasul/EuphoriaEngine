#pragma once
#include <RHIVulkan/RHIVulkanDLL.h>

#include <RHI/Memory/Memory.h>

class plVKDevice;

class plVKMemory : public plRHIMemory
{
public:
	plVKMemory(plVKDevice& device, plUInt64 size, plRHIMemoryType memoryType, plUInt32 memoryTypeBits, const vk::MemoryDedicatedAllocateInfoKHR* dedicatedAllocateInfo);
	plRHIMemoryType GetMemoryType() const override;
	vk::DeviceMemory GetMemory() const;

private:
	plRHIMemoryType m_MemoryType;
	vk::UniqueDeviceMemory m_Memory;
};