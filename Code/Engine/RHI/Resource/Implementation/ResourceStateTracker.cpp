#include <RHI/RHIPCH.h>

#include <RHI/Resource/ResourceStateTracker.h>
#include <RHI/Resource/Resource.h>

plRHIResourceStateTracker::plRHIResourceStateTracker(plRHIResource& resource)
	: m_Resource(resource)
{
}

bool plRHIResourceStateTracker::HasResourceState() const
{
	// TODO: Validate this
	return !m_SubresourceStates.IsEmpty();
}

plRHIResourceState plRHIResourceStateTracker::GetResourceState() const
{
	return m_ResourceState;
}

void plRHIResourceStateTracker::SetResourceState(plRHIResourceState state)
{
	m_SubresourceStates.Clear();
	m_ResourceState = state;
	m_SubresourceStateGroups.Clear();
}

plRHIResourceState plRHIResourceStateTracker::GetSubresourceState(plUInt32 mipLevel, plUInt32 arrayLayer) const
{
	auto it = m_SubresourceStates.Find(std::make_tuple(mipLevel, arrayLayer));
	if (it.IsValid())
	{
		return it.Value();
	}
	return m_ResourceState;
}

void plRHIResourceStateTracker::SetSubresourceState(plUInt32 mipLevel, plUInt32 arrayLayer, plRHIResourceState state)
{
	if (HasResourceState() && GetResourceState() == state)
	{
		return;
	}
	
	std::tuple<plUInt32, plUInt32> key(mipLevel, arrayLayer);
	if (m_SubresourceStates.Contains(key))
	{
		if (--m_SubresourceStateGroups[m_SubresourceStates[key]] == 0)
		{
			m_SubresourceStateGroups.Remove(m_SubresourceStates[key]);
		}
	}
	m_SubresourceStates[key] = state;
	++m_SubresourceStateGroups[state];
	if (m_SubresourceStateGroups.GetCount() == 1 && m_SubresourceStateGroups.GetIterator().Value() == m_Resource.GetLayerCount() * m_Resource.GetLevelCount())
	{
		m_SubresourceStateGroups.Clear();
		m_SubresourceStates.Clear();
		m_ResourceState = state;
	}
}

void plRHIResourceStateTracker::Merge(const plRHIResourceStateTracker& other)
{
	if (other.HasResourceState())
	{
		auto state = other.GetResourceState();
		if (state != plRHIResourceState::kUnknown)
		{
			SetResourceState(state);
		}
	}
	else
	{
		for (plUInt32 i = 0; i < other.m_Resource.GetLevelCount(); ++i)
		{
			for (plUInt32 j = 0; j < other.m_Resource.GetLayerCount(); ++j)
			{
				auto state = other.GetSubresourceState(i, j);
				if (state != plRHIResourceState::kUnknown)
				{
					SetSubresourceState(i, j, state);
				}
			}
		}
	}
}






