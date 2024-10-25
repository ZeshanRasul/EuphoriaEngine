#include <RHIVulkan/RHIVulkanPCH.h>

#include <RHIVulkan/Device/VKDevice.h>
#include <RHIVulkan/Memory/VKMemory.h>
#include <RHIVulkan/Resource/VKResource.h>

plVKResource::plVKResource(plVKDevice& device)
	: m_Device(device)
{
}

void plVKResource::CommitMemory(plRHIMemoryType memoryType)
{
	plRHIMemoryRequirements mem_requirements = GetMemoryRequirements();
	vk::MemoryDedicatedAllocateInfoKHR dedicated_allocate_info = {};
	vk::MemoryDedicatedAllocateInfoKHR* p_dedicated_allocate_info = nullptr;
	if (m_ResourceType == plRHIResourceType::kBuffer)
	{
		dedicated_allocate_info.buffer = buffer.m_Res.get();
		p_dedicated_allocate_info = &dedicated_allocate_info;
	}
	else if (m_ResourceType == plRHIResourceType::kTexture)
	{
		dedicated_allocate_info.image = image.m_Res;
		p_dedicated_allocate_info = &dedicated_allocate_info;
	}
	auto memory = PL_DEFAULT_NEW(plVKMemory, m_Device, mem_requirements.m_uiSize, memoryType, mem_requirements.m_uiMemoryTypeBits, p_dedicated_allocate_info);
	BindMemory(memory, 0);
}

void plVKResource::BindMemory(const plSharedPtr<plRHIMemory>& memory, plUInt64 offset)
{
	m_Memory = memory;
	m_MemoryType = m_Memory->GetMemoryType();
	m_VKMemory = m_Memory.Downcast<plVKMemory>()->GetMemory();

	if (m_ResourceType == plRHIResourceType::kBuffer)
	{
		m_Device.GetDevice().bindBufferMemory(buffer.m_Res.get(), m_VKMemory, offset);
	}
	else if (m_ResourceType == plRHIResourceType::kTexture)
	{
		m_Device.GetDevice().bindImageMemory(image.m_Res, m_VKMemory, offset);
	}
}

plUInt64 plVKResource::GetWidth() const
{
	if (m_ResourceType == plRHIResourceType::kTexture)
		return image.m_Size.width;
	return buffer.m_Size;
}

plUInt64 plVKResource::GetHeight() const
{
	return image.m_Size.height;
}

plUInt16 plVKResource::GetLayerCount() const
{
	return image.m_ArrayLayers;
}

plUInt16 plVKResource::GetLevelCount() const
{
	return image.m_LevelCount;
}

plUInt32 plVKResource::GetSampleCount() const
{
	return image.m_SampleCount;
}

plUInt64 plVKResource::GetAccelerationStructureHandle() const
{
	return m_Device.GetDevice().getAccelerationStructureAddressKHR({ m_AccelerationStructureHandle.get() });
}

void plVKResource::SetName(plStringView name)
{
	vk::DebugUtilsObjectNameInfoEXT info = {};
	plStringBuilder tmp;
	info.pObjectName = name.GetData(tmp);
	if (m_ResourceType == plRHIResourceType::kBuffer)
	{
		info.objectType = buffer.m_Res.get().objectType;
		info.objectHandle = reinterpret_cast<plUInt64>(static_cast<VkBuffer>(buffer.m_Res.get()));
	}
	else if (m_ResourceType == plRHIResourceType::kTexture)
	{
		info.objectType = image.m_Res.objectType;
		info.objectHandle = reinterpret_cast<plUInt64>(static_cast<VkImage>(image.m_Res));
	}
	m_Device.GetDevice().setDebugUtilsObjectNameEXT(info);
}

plUInt8* plVKResource::Map()
{
	plUInt8* dst_data = nullptr;
	vk::Result res = m_Device.GetDevice().mapMemory(m_VKMemory, 0, VK_WHOLE_SIZE, {}, reinterpret_cast<void**>(&dst_data));
	return dst_data;
}

void plVKResource::Unmap()
{
	m_Device.GetDevice().unmapMemory(m_VKMemory);
}

bool plVKResource::AllowCommonStatePromotion(plRHIResourceState state_after)
{
	return false;
}

plRHIMemoryRequirements plVKResource::GetMemoryRequirements() const
{
	vk::MemoryRequirements2 mem_requirements = {};
	if (m_ResourceType == plRHIResourceType::kBuffer)
	{
		vk::BufferMemoryRequirementsInfo2KHR buffer_mem_req = {};
		buffer_mem_req.buffer = buffer.m_Res.get();
		m_Device.GetDevice().getBufferMemoryRequirements2(&buffer_mem_req, &mem_requirements);
	}
	else if (m_ResourceType == plRHIResourceType::kTexture)
	{
		vk::ImageMemoryRequirementsInfo2KHR image_mem_req = {};
		image_mem_req.image = image.m_Res;
		m_Device.GetDevice().getImageMemoryRequirements2(&image_mem_req, &mem_requirements);
	}
	return { mem_requirements.memoryRequirements.size, mem_requirements.memoryRequirements.alignment, mem_requirements.memoryRequirements.memoryTypeBits };
}