#pragma once
#include <RHIVulkan/RHIVulkanDLL.h>

#include <RHI/Resource/ResourceBase.h>

static bool operator<(const VkImageSubresourceRange& lhs, const VkImageSubresourceRange& rhs) 
{ 
	return std::tie(lhs.aspectMask, lhs.baseArrayLayer, lhs.baseMipLevel, lhs.layerCount, lhs.levelCount) < 
		std::tie(rhs.aspectMask, rhs.baseArrayLayer, rhs.baseMipLevel, rhs.layerCount, rhs.levelCount); 
};

class plVKDevice;

class plVKResource : public plRHIResourceBase
{
public:
	plVKResource(plVKDevice& device);

	void CommitMemory(plRHIMemoryType memoryType) override;
	void BindMemory(const plSharedPtr<plRHIMemory>& memory, plUInt64 offset) override;
	plUInt64 GetWidth() const override;
	plUInt64 GetHeight() const override;
	plUInt16 GetLayerCount() const override;
	plUInt16 GetLevelCount() const override;
	plUInt32 GetSampleCount() const override;
	plUInt64 GetAccelerationStructureHandle() const override;
	void SetName(const plStringView name) override;
	plUInt8* Map() override;
	void Unmap() override;
	bool AllowCommonStatePromotion(plRHIResourceState state_after) override;
	plRHIMemoryRequirements GetMemoryRequirements() const override;

	struct Image
	{
		vk::Image m_Res;
		vk::UniqueImage m_Res_owner;
		vk::Format m_Format = vk::Format::eUndefined;
		vk::Extent2D m_Size = {};
		plUInt32 m_LevelCount = 1;
		plUInt32 m_SampleCount = 1;
		plUInt32 m_ArrayLayers = 1;
	} image;

	struct Buffer
	{
		vk::UniqueBuffer m_Res;
		plUInt32 m_Size = 0;
	} buffer;

	struct Sampler
	{
		vk::UniqueSampler m_Res;
	} sampler;

	vk::UniqueAccelerationStructureKHR m_AccelerationStructureHandle = {};

private:
	plVKDevice& m_Device;
	vk::DeviceMemory m_VKMemory;
};