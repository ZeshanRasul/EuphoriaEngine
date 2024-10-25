#include <RHIVulkan/RHIVulkanPCH.h>

#include <RHIVulkan/Memory/VKMemory.h>
#include <RHIVulkan/Device/VKDevice.h>

plVKMemory::plVKMemory(plVKDevice& device, plUInt64 size, plRHIMemoryType memoryType, plUInt32 memoryTypeBits, const vk::MemoryDedicatedAllocateInfoKHR* dedicatedAllocateInfo)
	: m_MemoryType(memoryType)
{
	vk::MemoryAllocateFlagsInfo allocFlagInfo = {};
	allocFlagInfo.pNext = dedicatedAllocateInfo;
	allocFlagInfo.flags = vk::MemoryAllocateFlagBits::eDeviceAddress;

	vk::MemoryPropertyFlags properties = {};
	if (memoryType == plRHIMemoryType::kDefault)
	{
		properties = vk::MemoryPropertyFlagBits::eDeviceLocal;
	}
	else if (memoryType == plRHIMemoryType::kUpload)
	{
		properties = vk::MemoryPropertyFlagBits::eHostVisible;
	}
	else if (memoryType == plRHIMemoryType::kReadback)
	{
		properties = vk::MemoryPropertyFlagBits::eHostVisible;
	}

	vk::MemoryAllocateInfo allocInfo = {};
	allocInfo.pNext = &allocFlagInfo;
	allocInfo.allocationSize = size;
	allocInfo.memoryTypeIndex = device.FindMemoryType(memoryTypeBits, properties);
	m_Memory = device.GetDevice().allocateMemoryUnique(allocInfo);
}

plRHIMemoryType plVKMemory::GetMemoryType() const
{
	return m_MemoryType;
}

vk::DeviceMemory plVKMemory::GetMemory() const
{
	return m_Memory.get();
}


