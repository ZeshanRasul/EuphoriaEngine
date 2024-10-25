#pragma once

#include <RHI/RHIDLL.h>

#include <RHI/Types/BasicTypes.h>
#include <functional>

class PL_RHI_DLL plRHIResourceStateTracker
{
public:
	plRHIResourceStateTracker(plRHIResource& resource);
	bool HasResourceState() const;
	plRHIResourceState GetResourceState() const;
	void SetResourceState(plRHIResourceState state);
	plRHIResourceState GetSubresourceState(plUInt32 mipLevel, plUInt32 arrayLayer) const;
	void SetSubresourceState(plUInt32 mipLevel, plUInt32 arrayLayer, plRHIResourceState state);
	void Merge(const plRHIResourceStateTracker& other);

private:
	plRHIResource& m_Resource;
	plMap<std::tuple<plUInt32, plUInt32>, plRHIResourceState> m_SubresourceStates;
	plMap<plRHIResourceState, size_t> m_SubresourceStateGroups;
	plRHIResourceState m_ResourceState = plRHIResourceState::kUnknown;
};