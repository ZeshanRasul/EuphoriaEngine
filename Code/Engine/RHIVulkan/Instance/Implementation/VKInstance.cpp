#include <RHIVulkan/RHIVulkanPCH.h>

#include <Foundation/Configuration/Startup.h>
#include <RHIVulkan/Adapter/VKAdapter.h>
#include <RHIVulkan/Instance/VKInstance.h>

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

plSharedPtr<plRHIInstance> CreateVKInstance()
{
	return PL_DEFAULT_NEW(plVKInstance);
}

PL_BEGIN_SUBSYSTEM_DECLARATION(RHIVulkan, plRHIInstanceFactory)
	ON_CORESYSTEMS_STARTUP
	{
		plRHIInstanceFactory::RegisterCreatorFunc(plRHIApiType::kVulkan, &CreateVKInstance);
	}

	ON_CORESYSTEMS_SHUTDOWN
	{
		plRHIInstanceFactory::UnregisterCreatorFunc(plRHIApiType::kVulkan);
	}

PL_END_SUBSYSTEM_DECLARATION;

static bool SkipIt(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT object_type, const plString& message)
{
	if (object_type == VK_DEBUG_REPORT_OBJECT_TYPE_INSTANCE_EXT && flags != VK_DEBUG_REPORT_ERROR_BIT_EXT)
		return true;

	static const char* muted_warnings[] = {
	  "UNASSIGNED-CoreValidation-Shader-InconsistentSpirv",
	  "VUID-vkCmdDrawIndexed-None-04007",
	  "VUID-vkDestroyDevice-device-00378",
	  "VUID-VkSubmitInfo-pWaitSemaphores-03243",
	  "VUID-VkSubmitInfo-pSignalSemaphores-03244",
	  "VUID-vkCmdPipelineBarrier-pDependencies-02285",
	  "VUID-VkImageMemoryBarrier-oldLayout-01213",
	  "VUID-vkCmdDrawIndexed-None-02721",
	  "VUID-vkCmdDrawIndexed-None-02699",
	  "VUID-vkCmdTraceRaysKHR-None-02699",
	  "VUID-VkShaderModuleCreateInfo-pCode-04147",
	};

	for (auto& str : muted_warnings)
	{
		if (message.FindSubString(str) != nullptr)
			return true;
	}
	return false;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugReportCallback(
	VkDebugReportFlagsEXT flags,
	VkDebugReportObjectTypeEXT objectType,
	plUInt64 object,
	size_t location,
	plInt32 messageCode,
	const char* pLayerPrefix,
	const char* pMessage,
	void* pUserData)
{
	constexpr size_t error_limit = 1024;
	static size_t error_count = 0;

	if (error_count >= error_limit || SkipIt(flags, objectType, pMessage))
	{
		return VK_FALSE;
	}

	if (error_count < error_limit)
	{
		plStringBuilder builder;
		builder.SetFormat("{}, {}, {}", pLayerPrefix, static_cast<VkDebugReportFlagBitsEXT>(flags), pMessage);
		plLog::Debug(builder.GetData());
	}

	++error_count;
	return VK_FALSE;
}

plVKInstance::plVKInstance()
{
	PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = m_DynamicLoader.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
	VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);

	auto layers = vk::enumerateInstanceLayerProperties();

	plSet<plString> reqLayers;

#if PL_ENABLED(PL_COMPILE_FOR_DEBUG)
	static const bool bDebugDevice = true;
#else
	static const bool bDebugDevice = false;
#endif

	if (bDebugDevice)
	{
		reqLayers.Insert("VK_LAYER_KHRONOS_validation");
	}

	plDynamicArray<const char*> foundLayers;
	for (const auto& layer : layers)
	{
		if (reqLayers.Contains(layer.layerName.data()))
		{
			foundLayers.PushBack(layer.layerName);
		}
	}

	auto extensions = vk::enumerateInstanceExtensionProperties();

	plSet<plString> reqExtensions;
	reqExtensions.Insert(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
	reqExtensions.Insert(VK_KHR_SURFACE_EXTENSION_NAME);

#if defined(VK_USE_PLATFORM_WIN32_KHR)
	reqExtensions.Insert(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_XLIB_KHR)
	reqExtensions.Insert(VK_KHR_XLIB_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_XCB_KHR)
	reqExtensions.Insert(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#endif

	reqExtensions.Insert(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	reqExtensions.Insert(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

	plDynamicArray<const char*> foundExtensions;
	for (const auto& extension : extensions)
	{
		if (reqExtensions.Contains(extension.extensionName.data()))
		{
			foundExtensions.PushBack(extension.extensionName);
		}
	}

	vk::ApplicationInfo appInfo = {};
	appInfo.apiVersion = VK_API_VERSION_1_2;

	vk::InstanceCreateInfo createInfo = {};
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledLayerCount = foundLayers.GetCount();
	createInfo.ppEnabledLayerNames = foundLayers.GetData();
	createInfo.enabledExtensionCount = foundExtensions.GetCount();
	createInfo.ppEnabledExtensionNames = foundExtensions.GetData();
	
	m_Instance = vk::createInstanceUnique(createInfo);
	VULKAN_HPP_DEFAULT_DISPATCHER.init(m_Instance.get());
}

plDynamicArray<plSharedPtr<plRHIAdapter>> plVKInstance::EnumerateAdapters()
{
	plDynamicArray<plSharedPtr<plRHIAdapter>> adapters;
	auto devices = m_Instance->enumeratePhysicalDevices();

	for (const auto& device : devices)
	{
		vk::PhysicalDeviceProperties deviceProperties = device.getProperties();

		if (device.getProperties().deviceType == vk::PhysicalDeviceType::eDiscreteGpu ||
			device.getProperties().deviceType == vk::PhysicalDeviceType::eIntegratedGpu)
		{
			adapters.PushBack(PL_DEFAULT_NEW(plVKAdapter, *this, device));
		}
	}

	return adapters;
}