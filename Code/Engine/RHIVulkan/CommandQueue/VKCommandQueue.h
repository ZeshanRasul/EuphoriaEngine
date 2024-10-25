#pragma once

#include <RHIVulkan/RHIVulkanDLL.h>

#include <RHI/CommandQueue/CommandQueue.h>
#include <RHI/Types/BasicTypes.h>

class plVKDevice;

class plVKCommandQueue : public plRHICommandQueue
{
public:
	plVKCommandQueue(plVKDevice& device, plRHICommandListType type, plUInt32 queueFamilyIndex);
	void Wait(const plSharedPtr<plRHIFence>& fence, plUInt64 value) override;
	void Submit(const plSharedPtr<plRHIFence>& fence, plUInt64 value) override;
	void ExecuteCommandList(const plDynamicArray<plSharedPtr<plRHICommandList>>& commandLists) override;

	plVKDevice& GetDevice();
	plUInt32 GetQueueFamilyIndex();
	vk::Queue GetQueue();

private:
	plVKDevice& m_Device;
	plUInt32 m_QueueFamilyIndex;
	vk::Queue m_Queue;
};