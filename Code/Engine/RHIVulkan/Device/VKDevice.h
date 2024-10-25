#pragma once

#include <RHIVulkan/RHIVulkanDLL.h>

#include <RHI/Device/Device.h>

class plVKAdapter;
class plVKCommandQueue;

class plVKDevice : public plRHIDevice
{
public:
	plVKDevice(plVKAdapter& adapter);

	plSharedPtr<plRHIMemory> AllocateMemory(plUInt64 size, plRHIMemoryType memoryType, plUInt32 memoryTypeBits) override;
	plSharedPtr<plRHICommandQueue> GetCommandQueue(plRHICommandListType type) override;

	plSharedPtr<plRHISwapchain> CreateSwapchain(plRHIWindow window, plUInt32 width, plUInt32 height, plUInt32 frameCount, bool vsync) override;
	void DestroySwapchain(plRHISwapchain* swapchain) override;

	plSharedPtr<plRHICommandList> CreateCommandList(plRHICommandListType type) override;
	void DestroyCommandList(plRHICommandList* commandList) override;

	plSharedPtr<plRHIFence> CreateFence(plUInt64 initialValue) override;
	void DestroyFence(plRHIFence* fence) override;

	plSharedPtr<plRHIResource> CreateTexture(plRHITextureType type, plUInt32 bindFlags, plRHIResourceFormat::Enum format, plUInt32 sampleCount, int width, int height, int depth, int mipLevels) override;
	void DestroyTexture(plRHIResource* texture) override;

	plSharedPtr<plRHIResource> CreateBuffer(plUInt32 bindFlags, plUInt64 size) override;
	void DestroyBuffer(plRHIResource* buffer) override;

	plSharedPtr<plRHIResource> CreateSampler(const plRHISamplerDesc& desc) override;
	void DestroySampler(plRHIResource* sampler) override;

	plSharedPtr<plRHIView> CreateView(const plSharedPtr<plRHIResource>& resource, const plRHIViewDesc& viewDesc) override;
	void DestroyView(plRHIView* view) override;

	plSharedPtr<plRHIBindingSetLayout> CreateBindingSetLayout(const std::vector<plRHIBindKey>& descs) override;
	void DestroyBindingSetLayout(plRHIBindingSetLayout* layout) override;

	plSharedPtr<plRHIBindingSet> CreateBindingSet(const plSharedPtr<plRHIBindingSetLayout>& layout) override;
	void DestroyBindingSet(plRHIBindingSet* set) override;

	plSharedPtr<plRHIRenderPass> CreateRenderPass(const plRHIRenderPassDesc& desc) override;
	void DestroyRenderPass(plRHIRenderPass* renderPass) override;

	plSharedPtr<plRHIFramebuffer> CreateFramebuffer(const plRHIFramebufferDesc& desc) override;
	void DestroyFramebuffer(plRHIFramebuffer* framebuffer) override;

	plSharedPtr<plRHIShader> CreateShader(const plRHIShaderDesc& desc, plDynamicArray<plUInt8> byteCode, plSharedPtr<plRHIShaderReflection> reflection) override;
	void DestroyShader(plRHIShader* shader) override;

	plSharedPtr<plRHIProgram> CreateProgram(const std::vector<plSharedPtr<plRHIShader>>& shaders) override;
	void DestroyProgram(plRHIProgram* program) override;

	plSharedPtr<plRHIPipeline> CreateGraphicsPipeline(const plRHIGraphicsPipelineDesc& desc) override;
	void DestroyGraphicsPipeline(plRHIPipeline* pipeline) override;

	plSharedPtr<plRHIPipeline> CreateComputePipeline(const plRHIComputePipelineDesc& desc) override;
	void DestroyComputePipeline(plRHIPipeline* pipeline) override;

	plSharedPtr<plRHIPipeline> CreateRayTracingPipeline(const plRHIRayTracingPipelineDesc& desc) override;
	void DestroyRayTracingPipeline(plRHIPipeline* pipeline) override;

	plSharedPtr<plRHIResource> CreateAccelerationStructure(plRHIAccelerationStructureType type, const plSharedPtr<plRHIResource>& resource, plUInt64 offset) override;
	void DestroyAccelerationStructure(plRHIResource* resource) override;

	plSharedPtr<plRHIQueryHeap> CreateQueryHeap(plRHIQueryHeapType type, plUInt32 count) override;
	void DestroyQueryHeap(plRHIQueryHeap* queryHeap) override;

	plUInt32 GetTextureDataPitchAlignment() const override;
	bool IsRTSupported() const override;
	bool IsRayQuerySupported() const override;
	bool IsVariableRateShadingSupported() const override;
	bool IsMeshShaderSupported() const override;
	plUInt32 GetShadingRateImageTileSize() override;
	plRHIMemoryBudget GetMemoryBudget() const override;
	plUInt32 GetShaderGroupHandleSize() const override;
	plUInt32 GetShaderRecordAlignment() const override;
	plUInt32 GetShaderTableAlignment() const override;
	plRHIRaytracingASPrebuildInfo GetBLASPrebuildInfo(const plDynamicArray<plRHIRaytracingGeometryDesc>& descs, plRHIBuildAccelerationStructureFlags flags) const override;
	plRHIRaytracingASPrebuildInfo GetTLASPrebuildInfo(plUInt32 instanceCount, plRHIBuildAccelerationStructureFlags flags) const override;

	plVKAdapter& GetAdapter();
	vk::Device GetDevice();
	plUInt32 FindMemoryType(plUInt32 typeFilter, vk::MemoryPropertyFlags properties);
	plRHICommandListType GetAvailableCommandListType(plRHICommandListType type);
	vk::AccelerationStructureGeometryKHR FillRaytracingGeometryTriangles(const plRHIBufferDesc& vertex, const plRHIBufferDesc& index, plRHIRaytracingGeometryFlags flags) const;

private:
	plRHIRaytracingASPrebuildInfo GetAccelerationStructurePrebuildInfo(const vk::AccelerationStructureBuildGeometryInfoKHR& accelerationStructureInfo, const plDynamicArray<plUInt32>& maxPrimitiveCounts) const;

	plVKAdapter& m_Adapter;
	const vk::PhysicalDevice& m_PhysicalDevice;
	vk::UniqueDevice m_Device;

	struct QueueInfo
	{
		plUInt32 m_uiQueueFamilyIndex;
		plUInt32 m_uiQueueCount;
	};
	plMap<plRHICommandListType, QueueInfo> m_QueueInfo;
	plMap <plRHICommandListType, vk::UniqueCommandBuffer> m_CmdPools;
	plMap<plRHICommandListType, plSharedPtr<plVKCommandQueue>> m_CommandQueues;

	bool m_bIsVariableRateShadingSupported = false;
	bool m_bIsRTSupported = false;
	bool m_bIsRayQuerySupported = false;
	bool m_bIsMeshShaderSupported = false;
	plUInt32 m_uiShadingRateImageTileSize = 0;
	plUInt32 m_uiShaderGroupHandleSize = 0;
	plUInt32 m_uiShaderRecordAlignment = 0;
	plUInt32 m_uiShaderTableAlignment = 0;
};