#pragma once

#include <Core/Interfaces/PhysicsWorldModule.h>
#include <Core/World/WorldModule.h>

struct plNavmeshTriangle
{
  plVec3 m_Vertices[3];
  const plSurfaceResource* m_pSurface = nullptr;
};

/// \brief A world module that retrieves triangle data that should be used for building navmeshes at runtime.
///
/// If a physics engine is active, it usually automatically provides such a world module to retrieve the triangle data
/// through physics queries.
///
/// In other types of games, a custom world module can be implemented, to generate this data in a different way.
/// If a physics engine is active, but a custom method should be used, you can write a custom world module
/// and then use plWorldModuleFactory::RegisterInterfaceImplementation() to specify which module to use.
/// Also see plWorldModuleConfig.
class PL_CORE_DLL plNavmeshGeoWorldModuleInterface : public plWorldModule
{
  PL_ADD_DYNAMIC_REFLECTION(plNavmeshGeoWorldModuleInterface, plWorldModule);

protected:
  plNavmeshGeoWorldModuleInterface(plWorld* pWorld)
    : plWorldModule(pWorld)
  {
  }

public:
  virtual void RetrieveGeometryInArea(plUInt32 uiCollisionLayer, const plBoundingBox& box, plDynamicArray<plNavmeshTriangle>& out_triangles) const = 0;
};