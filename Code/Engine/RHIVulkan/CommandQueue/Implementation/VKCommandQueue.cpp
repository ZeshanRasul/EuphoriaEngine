#include <RHIVulkan/RHIVulkanPCH.h>

#include <RHIVulkan/CommandQueue/VKCommandQueue.h>
#include <RHIVulkan/Device/VKDevice.h>
#include <RHIVulkan/Fence/VKFence.h>

plVKCommandQueue::plVKCommandQueue(plVKDevice& device, plRHICommandListType type, plUInt32 queueFamilyIndex)
	: m_Device(device)
	, m_QueueFamilyIndex(queueFamilyIndex)
{
	m_Queue = m_Device.GetDevice().getQueue(m_QueueFamilyIndex, 0);
}

void plVKCommandQueue::Wait(const plSharedPtr<plRHIFence>& fence, plUInt64 value)
{
	plSharedPtr<plVKTimelineSemaphore> vkFence = fence.Downcast<plVKTimelineSemaphore>();
	vk::TimelineSemaphoreSubmitInfo timelineInfo = {};
	timelineInfo.waitSemaphoreValueCount = 1;
	timelineInfo.pWaitSemaphoreValues = &value;

	vk::SubmitInfo signalSubmitInfo = {};
	signalSubmitInfo.pNext = &timelineInfo;
	signalSubmitInfo.waitSemaphoreCount = 1;
	signalSubmitInfo.pWaitSemaphores = &vkFence->GetFence();
	vk::PipelineStageFlags waitDstStageFlags = vk::PipelineStageFlagBits::eAllCommands;
	signalSubmitInfo.pWaitDstStageMask = &waitDstStageFlags;
	vk::Result result = m_Queue.submit(1, &signalSubmitInfo, {});
	PL_IGNORE_UNUSED(result);
}

void plVKCommandQueue::Submit(const plSharedPtr<plRHIFence>& fence, plUInt64 value)
{
	plSharedPtr<plVKTimelineSemaphore> vkFence = fence.Downcast<plVKTimelineSemaphore>();
	vk::TimelineSemaphoreSubmitInfo timelineInfo = {};
	timelineInfo.waitSemaphoreValueCount = 1;
	timelineInfo.pWaitSemaphoreValues = &value;

	vk::SubmitInfo signalSubmitInfo = {};
	signalSubmitInfo.pNext = &timelineInfo;
	signalSubmitInfo.waitSemaphoreCount = 1;
	signalSubmitInfo.pWaitSemaphores = &vkFence->GetFence();
	vk::Result result = m_Queue.submit(1, &signalSubmitInfo, {});
	PL_IGNORE_UNUSED(result);
}

void plVKCommandQueue::ExecuteCommandList(const plDynamicArray<plSharedPtr<plRHICommandList>>& commandLists)
{
}

plVKDevice& plVKCommandQueue::GetDevice()
{
	return m_Device;
}

plUInt32 plVKCommandQueue::GetQueueFamilyIndex()
{
	return m_QueueFamilyIndex;
}

vk::Queue plVKCommandQueue::GetQueue()
{
	return m_Queue;
}

