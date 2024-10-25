#include <RHIVulkan/RHIVulkanPCH.h>

#include <RHIVulkan/Adapter/VKAdapter.h>

#include <RHIVulkan/Device/VKDevice.h>


plVKAdapter::plVKAdapter(plVKInstance& instance, const vk::PhysicalDevice& physical_device)
	: m_Instance(instance)
	, m_PhysicalDevice(physical_device)
	, m_Name(physical_device.getProperties().deviceName.data())
{}

const plString& plVKAdapter::GetName() const
{
	return m_Name;
}

plSharedPtr<plRHIDevice> plVKAdapter::CreateDevice()
{
	return PL_DEFAULT_NEW(plVKDevice, *this);
}

plVKInstance& plVKAdapter::GetInstance()
{
	return m_Instance;
}

vk::PhysicalDevice& plVKAdapter::GetPhysicalDevice()
{
	return m_PhysicalDevice;
}