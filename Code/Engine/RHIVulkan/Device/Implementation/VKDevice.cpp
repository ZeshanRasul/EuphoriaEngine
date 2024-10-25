#include <RHIVulkan/RHIVulkanPCH.h>

#include <RHIVulkan/Adapter/VKAdapter.h>
#include <RHIVulkan/Device/VKDevice.h>
#include <RHIVulkan/Memory/VKMemory.h>
#include <RHIVulkan/CommandQueue/VKCommandQueue.h>
#include <RHIVulkan/Fence/VKFence.h>

PL_DEFINE_AS_POD_TYPE(vk::QueueFamilyProperties);
PL_DEFINE_AS_POD_TYPE(vk::ExtensionProperties);
PL_DEFINE_AS_POD_TYPE(vk::PhysicalDeviceFragmentShadingRateKHR);

plVKDevice::plVKDevice(plVKAdapter& adapter)
	: m_Adapter(adapter)
	, m_PhysicalDevice(adapter.GetPhysicalDevice())
{
	plUInt32 queueFamiliesCount = 0;
	m_PhysicalDevice.getQueueFamilyProperties(&queueFamiliesCount, nullptr);

	plDynamicArray<vk::QueueFamilyProperties> queueFamilies;
	queueFamilies.SetCountUninitialized(queueFamiliesCount);
	m_PhysicalDevice.getQueueFamilyProperties(&queueFamiliesCount, queueFamilies.GetData());

	auto hasAllBits = [](auto flags, auto bits)
		{
			return (flags & bits) == bits;
		};


	auto hasAnyBits = [](auto flags, auto bits)
		{
			return flags & bits;
		};

	for (plUInt32 i = 0; i < queueFamilies.GetCount(); i++)
	{
		const auto& queue = queueFamilies[i];

		if (queue.queueCount > 0 && hasAllBits(queue.queueFlags, vk::QueueFlagBits::eGraphics | vk::QueueFlagBits::eCompute | vk::QueueFlagBits::eTransfer))
		{
			m_QueueInfo[plRHICommandListType::kGraphics].m_uiQueueFamilyIndex = i;
			m_QueueInfo[plRHICommandListType::kGraphics].m_uiQueueFamilyIndex = queue.queueCount;
		}
		else if (queue.queueCount > 0 && hasAllBits(queue.queueFlags, vk::QueueFlagBits::eCompute | vk::QueueFlagBits::eTransfer) && !hasAnyBits(queue.queueFlags, vk::QueueFlagBits::eGraphics))
		{
			m_QueueInfo[plRHICommandListType::kCompute].m_uiQueueFamilyIndex = i;
			m_QueueInfo[plRHICommandListType::kCompute].m_uiQueueFamilyIndex = queue.queueCount;
		}
		else if (queue.queueCount > 0 && hasAllBits(queue.queueFlags, vk::QueueFlagBits::eTransfer) && !hasAnyBits(queue.queueFlags, vk::QueueFlagBits::eGraphics | vk::QueueFlagBits::eCompute))
		{
			m_QueueInfo[plRHICommandListType::kCopy].m_uiQueueFamilyIndex = i;
			m_QueueInfo[plRHICommandListType::kCopy].m_uiQueueFamilyIndex = queue.queueCount;
		}
	}

	plUInt32 extensionCount = 0;
	vk::Result result = m_PhysicalDevice.enumerateDeviceExtensionProperties(nullptr, &extensionCount, nullptr);

	plDynamicArray<vk::ExtensionProperties> extensions;
	extensions.SetCountUninitialized(extensionCount);
	result = m_PhysicalDevice.enumerateDeviceExtensionProperties(nullptr, &extensionCount, extensions.GetData());

	plSet<plString> requiredExtensions;

	requiredExtensions.Insert(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
	requiredExtensions.Insert(VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME);
	requiredExtensions.Insert(VK_KHR_RAY_QUERY_EXTENSION_NAME);
	requiredExtensions.Insert(VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME);
	requiredExtensions.Insert(VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME);
	requiredExtensions.Insert(VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME);
	requiredExtensions.Insert(VK_KHR_MAINTENANCE3_EXTENSION_NAME);
	requiredExtensions.Insert(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
	requiredExtensions.Insert(VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME);
	requiredExtensions.Insert(VK_KHR_FRAGMENT_SHADING_RATE_EXTENSION_NAME);
	requiredExtensions.Insert(VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME);
	requiredExtensions.Insert(VK_KHR_MAINTENANCE1_EXTENSION_NAME);
	requiredExtensions.Insert(VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME);
	requiredExtensions.Insert(VK_EXT_MEMORY_BUDGET_EXTENSION_NAME);
	requiredExtensions.Insert(VK_EXT_MESH_SHADER_EXTENSION_NAME);
	requiredExtensions.Insert(VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME);

	plDynamicArray<const char*> foundExtensions;

	for (const auto& extension : extensions)
	{
		if (requiredExtensions.Contains(extension.extensionName.data()))
			foundExtensions.PushBack(extension.extensionName.data());

		if (plStringUtils::IsEqual(extension.extensionName.data(), VK_KHR_FRAGMENT_SHADING_RATE_EXTENSION_NAME))
		{
			m_bIsVariableRateShadingSupported = true;
		}
		if (plStringUtils::IsEqual(extension.extensionName.data(), VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME))
		{
			m_bIsRTSupported = true;
		}
		if (plStringUtils::IsEqual(extension.extensionName.data(), VK_EXT_MESH_SHADER_EXTENSION_NAME))
		{
			m_bIsMeshShaderSupported = true;
		}
		if (plStringUtils::IsEqual(extension.extensionName.data(), VK_KHR_RAY_QUERY_EXTENSION_NAME))
		{
			m_bIsRayQuerySupported = true;
		}
	}

	void* deviceCreateInfoNext = nullptr;
	auto addExtension = [&](auto& extension)
		{
			extension.pNext = deviceCreateInfoNext;
			deviceCreateInfoNext = &extension;
		};

	if (m_bIsVariableRateShadingSupported)
	{
		plMap<plRHIShadingRate, vk::Extent2D> shadingRatePalette;
		shadingRatePalette.Insert(plRHIShadingRate::k1x1, vk::Extent2D{ 1, 1 });
		shadingRatePalette.Insert(plRHIShadingRate::k1x2, vk::Extent2D{ 1, 2 });
		shadingRatePalette.Insert(plRHIShadingRate::k2x1, vk::Extent2D{ 2, 1 });
		shadingRatePalette.Insert(plRHIShadingRate::k2x2, vk::Extent2D{ 2, 2 });
		shadingRatePalette.Insert(plRHIShadingRate::k2x4, vk::Extent2D{ 2, 4 });
		shadingRatePalette.Insert(plRHIShadingRate::k4x2, vk::Extent2D{ 4, 2 });
		shadingRatePalette.Insert(plRHIShadingRate::k4x4, vk::Extent2D{ 4, 4 });


		plUInt32 fragmentShadingRateCount = 0;
		result = m_Adapter.GetPhysicalDevice().getFragmentShadingRatesKHR(&fragmentShadingRateCount, nullptr);

		plDynamicArray<vk::PhysicalDeviceFragmentShadingRateKHR> fragmentShadingRates;
		fragmentShadingRates.SetCountUninitialized(fragmentShadingRateCount);
		result = m_Adapter.GetPhysicalDevice().getFragmentShadingRatesKHR(&fragmentShadingRateCount, fragmentShadingRates.GetData());

		for (const auto& fragmentShadingRate : fragmentShadingRates)
		{
			vk::Extent2D size = fragmentShadingRate.fragmentSize;
			plUInt8 shadingRate = ((size.width >> 1) << 2) | (size.height >> 1);
			PL_ASSERT_ALWAYS((1 << ((shadingRate >> 2) & 3)) == size.width, "");
			PL_ASSERT_ALWAYS((1 << (shadingRate & 3)) == size.height, "");
			PL_ASSERT_ALWAYS(shadingRatePalette[(plRHIShadingRate)shadingRate] == size, "");
		}

		PL_ASSERT_ALWAYS(shadingRatePalette.IsEmpty(), "");

		vk::PhysicalDeviceFragmentShadingRatePropertiesKHR shadingRateImageProperties = {};
		vk::PhysicalDeviceProperties2 deviceProps2 = {};
		deviceProps2.pNext = &shadingRateImageProperties;
		m_Adapter.GetPhysicalDevice().getProperties2(&deviceProps2);
		PL_ASSERT_ALWAYS(shadingRateImageProperties.minFragmentShadingRateAttachmentTexelSize == shadingRateImageProperties.maxFragmentShadingRateAttachmentTexelSize, "");
		PL_ASSERT_ALWAYS(shadingRateImageProperties.minFragmentShadingRateAttachmentTexelSize.width == shadingRateImageProperties.minFragmentShadingRateAttachmentTexelSize.height, "");
		PL_ASSERT_ALWAYS(shadingRateImageProperties.maxFragmentShadingRateAttachmentTexelSize.width == shadingRateImageProperties.maxFragmentShadingRateAttachmentTexelSize.height, "");
		m_uiShadingRateImageTileSize = shadingRateImageProperties.maxFragmentShadingRateAttachmentTexelSize.height;

		vk::PhysicalDeviceFragmentShadingRateFeaturesKHR fragmentShadingRateFeatures = {};
		fragmentShadingRateFeatures.attachmentFragmentShadingRate = true;
		addExtension(fragmentShadingRateFeatures);
	}

	if (m_bIsRTSupported)
	{
		vk::PhysicalDeviceRayTracingPipelinePropertiesKHR raytracingProperties = {};
		vk::PhysicalDeviceProperties2 deviceProperties2 = {};
		deviceProperties2.pNext = &raytracingProperties;
		m_PhysicalDevice.getProperties2(&deviceProperties2);
		m_uiShaderGroupHandleSize = raytracingProperties.shaderGroupHandleSize;
		m_uiShaderRecordAlignment = raytracingProperties.shaderGroupHandleAlignment;
		m_uiShaderTableAlignment = raytracingProperties.shaderGroupBaseAlignment;
	}

	const float queuePriority = 1.0f;
	plDynamicArray<vk::DeviceQueueCreateInfo> queuesCreateInfo;
	for (const auto& queueInfo : m_QueueInfo)
	{
		vk::DeviceQueueCreateInfo& queueCreateInfo = queuesCreateInfo.ExpandAndGetRef();
		queueCreateInfo.queueFamilyIndex = queueInfo.Value().m_uiQueueFamilyIndex;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
	}

	vk::PhysicalDeviceFeatures deviceFeatures = {};
	deviceFeatures.textureCompressionBC = true;
	deviceFeatures.vertexPipelineStoresAndAtomics = true;
	deviceFeatures.samplerAnisotropy = true;
	deviceFeatures.fragmentStoresAndAtomics = true;
	deviceFeatures.sampleRateShading = true;
	deviceFeatures.geometryShader = true;
	deviceFeatures.imageCubeArray = true;
	deviceFeatures.shaderImageGatherExtended = true;

	vk::PhysicalDeviceVulkan12Features deviceVulkan12Features = {};
	deviceVulkan12Features.drawIndirectCount = true;
	deviceVulkan12Features.bufferDeviceAddress = true;
	deviceVulkan12Features.timelineSemaphore = true;
	deviceVulkan12Features.runtimeDescriptorArray = true;
	deviceVulkan12Features.descriptorBindingVariableDescriptorCount = true;

	vk::PhysicalDeviceMeshShaderFeaturesEXT meshShaderFeatures = {};
	meshShaderFeatures.taskShader = true;
	meshShaderFeatures.meshShader = true;
	if (m_bIsMeshShaderSupported)
	{
		addExtension(meshShaderFeatures);
	}

	vk::PhysicalDeviceRayTracingPipelineFeaturesKHR raytracingPipelineFeatures = {};
	raytracingPipelineFeatures.rayTracingPipeline = true;

	vk::PhysicalDeviceAccelerationStructureFeaturesKHR accelerationStructureFeature = {};
	accelerationStructureFeature.accelerationStructure = true;

	vk::PhysicalDeviceRayQueryFeaturesKHR rayQueryPipelineFeatures = {};
	rayQueryPipelineFeatures.rayQuery = true;

	if (m_bIsRTSupported)
	{
		addExtension(raytracingPipelineFeatures);
		addExtension(accelerationStructureFeature);
		if (m_bIsRayQuerySupported)
		{
			raytracingPipelineFeatures.rayTraversalPrimitiveCulling = true;
			addExtension(rayQueryPipelineFeatures);
		}
	}

	vk::DeviceCreateInfo deviceCreateInfo = {};
	deviceCreateInfo.pNext = deviceCreateInfoNext;
	deviceCreateInfo.queueCreateInfoCount = queuesCreateInfo.GetCount();
	deviceCreateInfo.pQueueCreateInfos = queuesCreateInfo.GetData();
	deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
	deviceCreateInfo.enabledExtensionCount = foundExtensions.GetCount();
	deviceCreateInfo.ppEnabledExtensionNames = foundExtensions.GetData();

	m_Device = m_PhysicalDevice.createDeviceUnique(deviceCreateInfo);
	VULKAN_HPP_DEFAULT_DISPATCHER.init(m_Device.get());

	for (auto& queueInfo : m_QueueInfo)
	{
		vk::CommandPoolCreateInfo cmdPoolCreateInfo = {};
		cmdPoolCreateInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
		cmdPoolCreateInfo.queueFamilyIndex = queueInfo.Value().m_uiQueueFamilyIndex;
		m_CmdPools.Insert(queueInfo.Key(), m_Device->createCommandPoolUnique(cmdPoolCreateInfo));
		m_CommandQueues[queueInfo.Key()] = PL_DEFAULT_NEW(plVKCommandQueue, *this, queueInfo.Key(), queueInfo.Value().m_uiQueueFamilyIndex);
	}
}

plSharedPtr<plRHIMemory> plVKDevice::AllocateMemory(plUInt64 size, plRHIMemoryType memoryType, plUInt32 memoryTypeBits)
{
	return PL_DEFAULT_NEW(plVKMemory, *this, size, memoryType, memoryTypeBits, nullptr);
}

plSharedPtr<plRHICommandQueue> plVKDevice::GetCommandQueue(plRHICommandListType type)
{
	return m_CommandQueues[GetAvailableCommandListType(type)];
}

plSharedPtr<plRHISwapchain> plVKDevice::CreateSwapchain(plRHIWindow window, plUInt32 width, plUInt32 height, plUInt32 frameCount, bool vsync)
{
	return plSharedPtr<plRHISwapchain>();
}

void plVKDevice::DestroySwapchain(plRHISwapchain* swapchain)
{
}

plSharedPtr<plRHICommandList> plVKDevice::CreateCommandList(plRHICommandListType type)
{
	return plSharedPtr<plRHICommandList>();
}

void plVKDevice::DestroyCommandList(plRHICommandList* commandList)
{
}

plSharedPtr<plRHIFence> plVKDevice::CreateFence(plUInt64 initialValue)
{
	return PL_DEFAULT_NEW(plVKTimelineSemaphore, *this, initialValue);
}

void plVKDevice::DestroyFence(plRHIFence* fence)
{
}

plSharedPtr<plRHIResource> plVKDevice::CreateTexture(plRHITextureType type, plUInt32 bindFlags, plRHIResourceFormat::Enum format, plUInt32 sampleCount, int width, int height, int depth, int mipLevels)
{
	return plSharedPtr<plRHIResource>();
}

void plVKDevice::DestroyTexture(plRHIResource* texture)
{
}

plSharedPtr<plRHIResource> plVKDevice::CreateBuffer(plUInt32 bindFlags, plUInt64 size)
{
	return plSharedPtr<plRHIResource>();
}

void plVKDevice::DestroyBuffer(plRHIResource* buffer)
{
}

plSharedPtr<plRHIResource> plVKDevice::CreateSampler(const plRHISamplerDesc& desc)
{
	return plSharedPtr<plRHIResource>();
}

void plVKDevice::DestroySampler(plRHIResource* sampler)
{
}

plSharedPtr<plRHIView> plVKDevice::CreateView(const plSharedPtr<plRHIResource>& resource, const plRHIViewDesc& viewDesc)
{
	return plSharedPtr<plRHIView>();
}

void plVKDevice::DestroyView(plRHIView* view)
{
}

plSharedPtr<plRHIBindingSetLayout> plVKDevice::CreateBindingSetLayout(const std::vector<plRHIBindKey>& descs)
{
	return plSharedPtr<plRHIBindingSetLayout>();
}

void plVKDevice::DestroyBindingSetLayout(plRHIBindingSetLayout* layout)
{
}

plSharedPtr<plRHIBindingSet> plVKDevice::CreateBindingSet(const plSharedPtr<plRHIBindingSetLayout>& layout)
{
	return plSharedPtr<plRHIBindingSet>();
}

void plVKDevice::DestroyBindingSet(plRHIBindingSet* set)
{
}

plSharedPtr<plRHIRenderPass> plVKDevice::CreateRenderPass(const plRHIRenderPassDesc& desc)
{
	return plSharedPtr<plRHIRenderPass>();
}

void plVKDevice::DestroyRenderPass(plRHIRenderPass* renderPass)
{
}

plSharedPtr<plRHIFramebuffer> plVKDevice::CreateFramebuffer(const plRHIFramebufferDesc& desc)
{
	return plSharedPtr<plRHIFramebuffer>();
}

void plVKDevice::DestroyFramebuffer(plRHIFramebuffer* framebuffer)
{
}

plSharedPtr<plRHIShader> plVKDevice::CreateShader(const plRHIShaderDesc& desc, plDynamicArray<plUInt8> byteCode, plSharedPtr<plRHIShaderReflection> reflection)
{
	return plSharedPtr<plRHIShader>();
}

void plVKDevice::DestroyShader(plRHIShader* shader)
{
}

plSharedPtr<plRHIProgram> plVKDevice::CreateProgram(const std::vector<plSharedPtr<plRHIShader>>& shaders)
{
	return plSharedPtr<plRHIProgram>();
}

void plVKDevice::DestroyProgram(plRHIProgram* program)
{
}

plSharedPtr<plRHIPipeline> plVKDevice::CreateGraphicsPipeline(const plRHIGraphicsPipelineDesc& desc)
{
	return plSharedPtr<plRHIPipeline>();
}

void plVKDevice::DestroyGraphicsPipeline(plRHIPipeline* pipeline)
{
}

plSharedPtr<plRHIPipeline> plVKDevice::CreateComputePipeline(const plRHIComputePipelineDesc& desc)
{
	return plSharedPtr<plRHIPipeline>();
}

void plVKDevice::DestroyComputePipeline(plRHIPipeline* pipeline)
{
}

plSharedPtr<plRHIPipeline> plVKDevice::CreateRayTracingPipeline(const plRHIRayTracingPipelineDesc& desc)
{
	return plSharedPtr<plRHIPipeline>();
}

void plVKDevice::DestroyRayTracingPipeline(plRHIPipeline* pipeline)
{
}

plSharedPtr<plRHIResource> plVKDevice::CreateAccelerationStructure(plRHIAccelerationStructureType type, const plSharedPtr<plRHIResource>& resource, plUInt64 offset)
{
	return plSharedPtr<plRHIResource>();
}

void plVKDevice::DestroyAccelerationStructure(plRHIResource* resource)
{
}

plSharedPtr<plRHIQueryHeap> plVKDevice::CreateQueryHeap(plRHIQueryHeapType type, plUInt32 count)
{
	return plSharedPtr<plRHIQueryHeap>();
}

void plVKDevice::DestroyQueryHeap(plRHIQueryHeap* queryHeap)
{
}

plUInt32 plVKDevice::GetTextureDataPitchAlignment() const
{
	return 1;
}

bool plVKDevice::IsRTSupported() const
{
	return m_bIsRTSupported;
}

bool plVKDevice::IsRayQuerySupported() const
{
	return m_bIsRayQuerySupported;
}

bool plVKDevice::IsVariableRateShadingSupported() const
{
	return m_bIsVariableRateShadingSupported;
}

bool plVKDevice::IsMeshShaderSupported() const
{
	return m_bIsMeshShaderSupported;
}

plUInt32 plVKDevice::GetShadingRateImageTileSize()
{
	return m_uiShadingRateImageTileSize;
}

plRHIMemoryBudget plVKDevice::GetMemoryBudget() const
{
	vk::PhysicalDeviceMemoryBudgetPropertiesEXT memoryBudget = {};
	vk::PhysicalDeviceMemoryProperties2 memoryProperties2 = {};
	memoryProperties2.pNext = &memoryBudget;
	m_Adapter.GetPhysicalDevice().getMemoryProperties2(&memoryProperties2);
	plRHIMemoryBudget res = {};
	for (size_t i = 0; i < VK_MAX_MEMORY_HEAPS; ++i)
	{
		res.m_uiBudget += memoryBudget.heapBudget[i];
		res.m_uiUsage += memoryBudget.heapUsage[i];
	}
	return res;
}

plUInt32 plVKDevice::GetShaderGroupHandleSize() const
{
	return m_uiShaderGroupHandleSize;
}

plUInt32 plVKDevice::GetShaderRecordAlignment() const
{
	return m_uiShaderRecordAlignment;
}

plUInt32 plVKDevice::GetShaderTableAlignment() const
{
	return m_uiShaderTableAlignment;
}

vk::BuildAccelerationStructureFlagsKHR Convert(plRHIBuildAccelerationStructureFlags flags)
{
	vk::BuildAccelerationStructureFlagsKHR vk_flags = {};
	if (flags & plRHIBuildAccelerationStructureFlags::kAllowUpdate)
		vk_flags |= vk::BuildAccelerationStructureFlagBitsKHR::eAllowUpdate;
	if (flags & plRHIBuildAccelerationStructureFlags::kAllowCompaction)
		vk_flags |= vk::BuildAccelerationStructureFlagBitsKHR::eAllowCompaction;
	if (flags & plRHIBuildAccelerationStructureFlags::kPreferFastTrace)
		vk_flags |= vk::BuildAccelerationStructureFlagBitsKHR::ePreferFastTrace;
	if (flags & plRHIBuildAccelerationStructureFlags::kPreferFastBuild)
		vk_flags |= vk::BuildAccelerationStructureFlagBitsKHR::ePreferFastBuild;
	if (flags & plRHIBuildAccelerationStructureFlags::kMinimizeMemory)
		vk_flags |= vk::BuildAccelerationStructureFlagBitsKHR::eLowMemory;
	return vk_flags;
}

vk::AccelerationStructureTypeKHR Convert(plRHIAccelerationStructureType type)
{
	switch (type)
	{
	case plRHIAccelerationStructureType::kTopLevel:
		return vk::AccelerationStructureTypeKHR::eTopLevel;
	case plRHIAccelerationStructureType::kBottomLevel:
		return vk::AccelerationStructureTypeKHR::eBottomLevel;
	}
	assert(false);
	return {};
}

plRHIRaytracingASPrebuildInfo plVKDevice::GetBLASPrebuildInfo(const plDynamicArray<plRHIRaytracingGeometryDesc>& descs, plRHIBuildAccelerationStructureFlags flags) const
{
	plDynamicArray<vk::AccelerationStructureGeometryKHR> geometryDescs;
	plDynamicArray<plUInt32> maxPrimitiveCounts;

	for (const auto& desc : descs)
	{
		geometryDescs.PushBack(FillRaytracingGeometryTriangles(desc.vertex, desc.index, desc.flags));
		if (desc.index.res)
		{
			maxPrimitiveCounts.PushBack(desc.index.count / 3);
		}
		else
		{
			maxPrimitiveCounts.PushBack(desc.vertex.count / 3);
		}
	}

	vk::AccelerationStructureBuildGeometryInfoKHR accelerationStructureInfo = {};
	accelerationStructureInfo.type = vk::AccelerationStructureTypeKHR::eBottomLevel;
	accelerationStructureInfo.geometryCount = geometryDescs.GetCount();
	accelerationStructureInfo.pGeometries = geometryDescs.GetData();
	accelerationStructureInfo.flags = Convert(flags);
	return GetAccelerationStructurePrebuildInfo(accelerationStructureInfo, maxPrimitiveCounts);
}

plRHIRaytracingASPrebuildInfo plVKDevice::GetTLASPrebuildInfo(plUInt32 instanceCount, plRHIBuildAccelerationStructureFlags flags) const
{
	vk::AccelerationStructureGeometryKHR geometryInfo = {};
	geometryInfo.geometryType = vk::GeometryTypeKHR::eInstances;
	geometryInfo.geometry.setInstances({});

	vk::AccelerationStructureBuildGeometryInfoKHR accelerationStructureInfo = {};
	accelerationStructureInfo.type = vk::AccelerationStructureTypeKHR::eTopLevel;
	accelerationStructureInfo.pGeometries = &geometryInfo;
	accelerationStructureInfo.geometryCount = 1;
	accelerationStructureInfo.flags = Convert(flags);
	plDynamicArray<plUInt32> maxPrimitiveCounts;
	maxPrimitiveCounts.EnsureCount(instanceCount);
	return GetAccelerationStructurePrebuildInfo(accelerationStructureInfo, maxPrimitiveCounts);
}

vk::AccelerationStructureGeometryKHR plVKDevice::FillRaytracingGeometryTriangles(const plRHIBufferDesc& vertex, const plRHIBufferDesc& index, plRHIRaytracingGeometryFlags flags) const
{
	vk::AccelerationStructureGeometryKHR geometry_desc = {};
	geometry_desc.geometryType = vk::GeometryTypeNV::eTriangles;
	switch (flags)
	{
	case plRHIRaytracingGeometryFlags::kOpaque:
		geometry_desc.flags = vk::GeometryFlagBitsKHR::eOpaque;
		break;
	case plRHIRaytracingGeometryFlags::kNoDuplicateAnyHitInvocation:
		geometry_desc.flags = vk::GeometryFlagBitsKHR::eNoDuplicateAnyHitInvocation;
		break;
	}

	// auto vk_vertex_res = vertex.res.Downcast<plVKResource>();
	// auto vk_index_res = index.res.Downcast<plVKResource>();
	//
	// auto vertex_stride = plRHIResourceFormat::GetFormatStride(vertex.format);
	// geometry_desc.geometry.triangles.vertexData = m_Device->getBufferAddress({vk_vertex_res->buffer.res.get()}) + vertex.offset * vertex_stride;
	// geometry_desc.geometry.triangles.vertexStride = vertex_stride;
	// geometry_desc.geometry.triangles.vertexFormat = plVKUtils::ToVkFormat(vertex.format);
	// geometry_desc.geometry.triangles.maxVertex = vertex.count;
	// if (vk_index_res)
	// {
	//     auto index_stride = plRHIResourceFormat::GetFormatStride(index.format);
	//     geometry_desc.geometry.triangles.indexData = m_Device->getBufferAddress({vk_index_res->buffer.res.get()}) + index.offset * index_stride;
	//     geometry_desc.geometry.triangles.indexType = GetVkIndexType(index.format);
	// }
	// else
	// {
	//     geometry_desc.geometry.triangles.indexType = vk::IndexType::eNoneNV;
	// }

	return geometry_desc;
}

plVKAdapter& plVKDevice::GetAdapter()
{
	return m_Adapter;
}

vk::Device plVKDevice::GetDevice()
{
	return m_Device.get();
}

plUInt32 plVKDevice::FindMemoryType(plUInt32 typeFilter, vk::MemoryPropertyFlags properties)
{
	vk::PhysicalDeviceMemoryProperties memProperties;
	m_PhysicalDevice.getMemoryProperties(&memProperties);

	for (plUInt32 i = 0; i < memProperties.memoryTypeCount; ++i)
	{
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
		{
			return i;
		}
	}
	throw std::runtime_error("Failed to find suitable memory type");
}

plRHICommandListType plVKDevice::GetAvailableCommandListType(plRHICommandListType type)
{
	return plRHICommandListType();
}

plRHIRaytracingASPrebuildInfo plVKDevice::GetAccelerationStructurePrebuildInfo(const vk::AccelerationStructureBuildGeometryInfoKHR& accelerationStructureInfo, const plDynamicArray<plUInt32>& maxPrimitiveCounts) const
{
	vk::AccelerationStructureBuildSizesInfoKHR sizeInfo = {};
	m_Device->getAccelerationStructureBuildSizesKHR(vk::AccelerationStructureBuildTypeKHR::eDevice, &accelerationStructureInfo, maxPrimitiveCounts.GetData(), &sizeInfo);
	plRHIRaytracingASPrebuildInfo prebuildInfo = {};
	prebuildInfo.accelerationStructureSize = sizeInfo.accelerationStructureSize;
	prebuildInfo.buildScratchDataSize = sizeInfo.buildScratchSize;
	prebuildInfo.updateScratchDataSize = sizeInfo.updateScratchSize;
	return prebuildInfo;
}