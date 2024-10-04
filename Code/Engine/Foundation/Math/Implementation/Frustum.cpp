#include <Foundation/FoundationPCH.h>

#include <Foundation/Math/Frustum.h>
#include <Foundation/SimdMath/SimdBBox.h>
#include <Foundation/SimdMath/SimdConversion.h>
#include <Foundation/SimdMath/SimdVec4f.h>
#include <Foundation/Utilities/GraphicsUtils.h>

plFrustum::plFrustum() = default;
plFrustum::~plFrustum() = default;

plFrustum plFrustum::MakeInvalid()
{
  plFrustum frustum;
  for (plUInt32 i = 0; i < PLANE_COUNT; ++i)
  {
    frustum.m_Planes[i] = plPlane::MakeInvalid();
  }

  return frustum;
}

const plPlane& plFrustum::GetPlane(plUInt8 uiPlane) const
{
  PL_ASSERT_DEBUG(uiPlane < PLANE_COUNT, "Invalid plane index.");

  return m_Planes[uiPlane];
}

plPlane& plFrustum::AccessPlane(plUInt8 uiPlane)
{
  PL_ASSERT_DEBUG(uiPlane < PLANE_COUNT, "Invalid plane index.");

  return m_Planes[uiPlane];
}

bool plFrustum::IsValid() const
{
  // For frustums with infinite farplanes we test a finite frustum slice for validity, as the
  // computations below don't work when 4 of the corner points are at infinity.
  if (plMath::Abs(m_Planes[FarPlane].m_fNegDistance) == plMath::Infinity<float>())
  {
    plFrustum finiteSlice = *this;
    finiteSlice.m_Planes[FarPlane].m_fNegDistance = -2.f * plMath::Abs(m_Planes[NearPlane].m_fNegDistance);
    return finiteSlice.IsValid();
  }

  for (plUInt32 i = 0; i < PLANE_COUNT; ++i)
  {
    if (!m_Planes[i].IsValid() || (i != FarPlane && !plMath::IsFinite(m_Planes[i].m_fNegDistance)))
      return false;
  }

  plVec3 corners[8];
  if (ComputeCornerPoints(corners).Failed())
    return false;

  plVec3 center = plVec3::MakeZero();
  for (plUInt32 i = 0; i < 8; ++i)
  {
    center += corners[i];
  }
  center /= 8.0f;

  if (GetObjectPosition(&center, 1) != plVolumePosition::Inside)
    return false;

  return true;
}

plFrustum plFrustum::MakeFromPlanes(const plPlane* pPlanes)
{
  plFrustum frustum;
  const plResult res = TryMakeFromPlanes(frustum, pPlanes);
  PL_ASSERT_DEV(res.Succeeded() && frustum.IsValid(), "Frustum is not valid after construction.");
  PL_IGNORE_UNUSED(res);
  return frustum;
}

plResult plFrustum::TryMakeFromPlanes(plFrustum& out_frustum , const plPlane* pPlanes)
{
  plFrustum f;

  for (plUInt32 i = 0; i < PLANE_COUNT; ++i)
    f.m_Planes[i] = pPlanes[i];

  if (f.IsValid())
  {
    out_frustum  = std::move(f);
    return PL_SUCCESS;
  }

  return PL_FAILURE;
}

void plFrustum::TransformFrustum(const plMat4& mTransform)
{
  for (plUInt32 i = 0; i < PLANE_COUNT; ++i)
  {
    m_Planes[i].Transform(mTransform);
  }
}

plFrustum plFrustum::GetTransformedFrustum(const plMat4& mTransform) const
{
  plFrustum result = *this;
  result.TransformFrustum(mTransform);
  return result;
}

plVolumePosition::Enum plFrustum::GetObjectPosition(const plVec3* pVertices, plUInt32 uiNumVertices) const
{
  /// \test Not yet tested

  bool bOnSomePlane = false;

  for (plUInt32 i = 0; i < PLANE_COUNT; ++i)
  {
    const plPositionOnPlane::Enum pos = m_Planes[i].GetObjectPosition(pVertices, uiNumVertices);

    if (pos == plPositionOnPlane::Back)
      continue;

    if (pos == plPositionOnPlane::Front)
      return plVolumePosition::Outside;

    bOnSomePlane = true;
  }

  if (bOnSomePlane)
    return plVolumePosition::Intersecting;

  return plVolumePosition::Inside;
}

static plPositionOnPlane::Enum GetPlaneObjectPosition(const plPlane& p, const plVec3* const pPoints, plUInt32 uiVertices, const plMat4& mTransform)
{
  bool bFront = false;
  bool bBack = false;

  for (plUInt32 i = 0; i < uiVertices; ++i)
  {
    switch (p.GetPointPosition(mTransform * pPoints[i]))
    {
      case plPositionOnPlane::Front:
      {
        if (bBack)
          return plPositionOnPlane::Spanning;

        bFront = true;
      }
      break;

      case plPositionOnPlane::Back:
      {
        if (bFront)
          return (plPositionOnPlane::Spanning);

        bBack = true;
      }
      break;

      default:
        break;
    }
  }

  return (bFront ? plPositionOnPlane::Front : plPositionOnPlane::Back);
}


plVolumePosition::Enum plFrustum::GetObjectPosition(const plVec3* pVertices, plUInt32 uiNumVertices, const plMat4& mObjectTransform) const
{
  /// \test Not yet tested

  bool bOnSomePlane = false;

  for (plUInt32 i = 0; i < PLANE_COUNT; ++i)
  {
    const plPositionOnPlane::Enum pos = GetPlaneObjectPosition(m_Planes[i], pVertices, uiNumVertices, mObjectTransform);

    if (pos == plPositionOnPlane::Back)
      continue;

    if (pos == plPositionOnPlane::Front)
      return plVolumePosition::Outside;

    bOnSomePlane = true;
  }

  if (bOnSomePlane)
    return plVolumePosition::Intersecting;

  return plVolumePosition::Inside;
}

plVolumePosition::Enum plFrustum::GetObjectPosition(const plBoundingSphere& sphere) const
{
  /// \test Not yet tested

  bool bOnSomePlane = false;

  for (plUInt32 i = 0; i < PLANE_COUNT; ++i)
  {
    const plPositionOnPlane::Enum pos = m_Planes[i].GetObjectPosition(sphere);

    if (pos == plPositionOnPlane::Back)
      continue;

    if (pos == plPositionOnPlane::Front)
      return plVolumePosition::Outside;

    bOnSomePlane = true;
  }

  if (bOnSomePlane)
    return plVolumePosition::Intersecting;

  return plVolumePosition::Inside;
}

plVolumePosition::Enum plFrustum::GetObjectPosition(const plBoundingBox& box) const
{
  /// \test Not yet tested

  bool bOnSomePlane = false;

  for (plUInt32 i = 0; i < PLANE_COUNT; ++i)
  {
    const plPositionOnPlane::Enum pos = m_Planes[i].GetObjectPosition(box);

    if (pos == plPositionOnPlane::Back)
      continue;

    if (pos == plPositionOnPlane::Front)
      return plVolumePosition::Outside;

    bOnSomePlane = true;
  }

  if (bOnSomePlane)
    return plVolumePosition::Intersecting;

  return plVolumePosition::Inside;
}

void plFrustum::InvertFrustum()
{
  for (plUInt32 i = 0; i < PLANE_COUNT; ++i)
    m_Planes[i].Flip();
}

plResult plFrustum::ComputeCornerPoints(plVec3 out_pPoints[FrustumCorner::CORNER_COUNT]) const
{
  PL_SUCCEED_OR_RETURN(plPlane::GetPlanesIntersectionPoint(m_Planes[NearPlane], m_Planes[TopPlane], m_Planes[LeftPlane], out_pPoints[FrustumCorner::NearTopLeft]));
  PL_SUCCEED_OR_RETURN(plPlane::GetPlanesIntersectionPoint(m_Planes[NearPlane], m_Planes[TopPlane], m_Planes[RightPlane], out_pPoints[FrustumCorner::NearTopRight]));
  PL_SUCCEED_OR_RETURN(plPlane::GetPlanesIntersectionPoint(m_Planes[NearPlane], m_Planes[BottomPlane], m_Planes[LeftPlane], out_pPoints[FrustumCorner::NearBottomLeft]));
  PL_SUCCEED_OR_RETURN(plPlane::GetPlanesIntersectionPoint(m_Planes[NearPlane], m_Planes[BottomPlane], m_Planes[RightPlane], out_pPoints[FrustumCorner::NearBottomRight]));

  PL_SUCCEED_OR_RETURN(plPlane::GetPlanesIntersectionPoint(m_Planes[FarPlane], m_Planes[TopPlane], m_Planes[LeftPlane], out_pPoints[FrustumCorner::FarTopLeft]));
  PL_SUCCEED_OR_RETURN(plPlane::GetPlanesIntersectionPoint(m_Planes[FarPlane], m_Planes[TopPlane], m_Planes[RightPlane], out_pPoints[FrustumCorner::FarTopRight]));
  PL_SUCCEED_OR_RETURN(plPlane::GetPlanesIntersectionPoint(m_Planes[FarPlane], m_Planes[BottomPlane], m_Planes[LeftPlane], out_pPoints[FrustumCorner::FarBottomLeft]));
  PL_SUCCEED_OR_RETURN(plPlane::GetPlanesIntersectionPoint(m_Planes[FarPlane], m_Planes[BottomPlane], m_Planes[RightPlane], out_pPoints[FrustumCorner::FarBottomRight]));

  return PL_SUCCESS;
}

plFrustum plFrustum::MakeFromMVP(const plMat4& mModelViewProjection0, plClipSpaceDepthRange::Enum depthRange, plHandedness::Enum handedness)
{
  plFrustum frustum;
  const plResult res = TryMakeFromMVP(frustum, mModelViewProjection0, depthRange, handedness);
  PL_ASSERT_DEV(res.Succeeded() && frustum.IsValid(), "Frustum is not valid after construction.");
  PL_IGNORE_UNUSED(res);
  return frustum;
}

plResult plFrustum::TryMakeFromMVP(plFrustum& out_frustum , const plMat4& mModelViewProjection0, plClipSpaceDepthRange::Enum depthRange, plHandedness::Enum handedness)
{
  plMat4 ModelViewProjection = mModelViewProjection0;
  plGraphicsUtils::ConvertProjectionMatrixDepthRange(ModelViewProjection, depthRange, plClipSpaceDepthRange::MinusOneToOne);

  plVec4 planes[6];

  if (handedness == plHandedness::LeftHanded)
  {
    ModelViewProjection.SetRow(0, -ModelViewProjection.GetRow(0));
  }

  planes[LeftPlane] = -ModelViewProjection.GetRow(3) - ModelViewProjection.GetRow(0);
  planes[RightPlane] = -ModelViewProjection.GetRow(3) + ModelViewProjection.GetRow(0);
  planes[BottomPlane] = -ModelViewProjection.GetRow(3) - ModelViewProjection.GetRow(1);
  planes[TopPlane] = -ModelViewProjection.GetRow(3) + ModelViewProjection.GetRow(1);
  planes[NearPlane] = -ModelViewProjection.GetRow(3) - ModelViewProjection.GetRow(2);
  planes[FarPlane] = -ModelViewProjection.GetRow(3) + ModelViewProjection.GetRow(2);

  // Normalize planes
  for (int p = 0; p < 6; ++p)
  {
    const float len = planes[p].GetAsVec3().GetLength();
    // doing the division here manually since we want to accept the case where length is 0 (infinite plane)
    const float invLen = 1.f / len;
    planes[p].x *= plMath::IsFinite(invLen) ? invLen : 0.f;
    planes[p].y *= plMath::IsFinite(invLen) ? invLen : 0.f;
    planes[p].z *= plMath::IsFinite(invLen) ? invLen : 0.f;
    planes[p].w *= invLen;
  }

  // The last matrix row is giving the camera's plane, which means its normal is
  // also the camera's viewing direction.
  const plVec3 cameraViewDirection = ModelViewProjection.GetRow(3).GetAsVec3();

  // Making sure the near/far plane is always closest/farthest. The way we derive the
  // planes always yields the closer plane pointing towards the camera and the farther
  // plane pointing away from the camera, so flip when that relationship inverts.
  if (planes[FarPlane].GetAsVec3().Dot(cameraViewDirection) < 0)
  {
    PL_ASSERT_DEBUG(planes[NearPlane].GetAsVec3().Dot(cameraViewDirection) >= 0, "");
    plMath::Swap(planes[NearPlane], planes[FarPlane]);
  }

  // In case we have an infinity far plane projection, the normal is invalid.
  // We'll just take the mirrored normal from the near plane.
  PL_ASSERT_DEBUG(planes[NearPlane].IsValid(), "Near plane is expected to be non-nan and finite at this point!");
  if (plMath::Abs(planes[FarPlane].w) == plMath::Infinity<float>())
  {
    planes[FarPlane] = (-planes[NearPlane].GetAsVec3()).GetAsVec4(planes[FarPlane].w);
  }

  static_assert(sizeof(plFrustum) == sizeof(planes));
  if (reinterpret_cast<plFrustum*>(planes)->IsValid())
  {
    static_assert(offsetof(plPlane, m_vNormal) == offsetof(plVec4, x) && offsetof(plPlane, m_fNegDistance) == offsetof(plVec4, w));
    plMemoryUtils::Copy(out_frustum .m_Planes, (plPlane*)planes, 6);
    return PL_SUCCESS;
  }

  return PL_FAILURE;
}

plFrustum plFrustum::MakeFromFOV(const plVec3& vPosition, const plVec3& vForwards, const plVec3& vUp, plAngle fovX, plAngle fovY, float fNearPlane, float fFarPlane)
{
  plFrustum frustum;
  const plResult res = TryMakeFromFOV(frustum, vPosition, vForwards, vUp, fovX, fovY, fNearPlane, fFarPlane);
  PL_ASSERT_DEV(res.Succeeded() && frustum.IsValid(), "Frustum is not valid after construction.");
  PL_IGNORE_UNUSED(res);
  return frustum;
}

plResult plFrustum::TryMakeFromFOV(plFrustum& out_frustum, const plVec3& vPosition, const plVec3& vForwards, const plVec3& vUp, plAngle fovX, plAngle fovY, float fNearPlane, float fFarPlane)
{
  PL_ASSERT_DEBUG(plMath::Abs(vForwards.GetNormalized().Dot(vUp.GetNormalized())) < 0.999f, "Up dir must be different from forward direction");

  const plVec3 vForwardsNorm = vForwards.GetNormalized();
  const plVec3 vRightNorm = vForwards.CrossRH(vUp).GetNormalized();
  const plVec3 vUpNorm = vRightNorm.CrossRH(vForwards).GetNormalized();

  plFrustum res;

  // Near Plane
  res.m_Planes[NearPlane] = plPlane::MakeFromNormalAndPoint(-vForwardsNorm, vPosition + fNearPlane * vForwardsNorm);

  // Far Plane
  res.m_Planes[FarPlane] = plPlane::MakeFromNormalAndPoint(vForwardsNorm, vPosition + fFarPlane * vForwardsNorm);

  // Making sure the near/far plane is always closest/farthest.
  if (fNearPlane > fFarPlane)
  {
    plMath::Swap(res.m_Planes[NearPlane], res.m_Planes[FarPlane]);
  }

  plMat3 mLocalFrame;
  mLocalFrame.SetColumn(0, vRightNorm);
  mLocalFrame.SetColumn(1, vUpNorm);
  mLocalFrame.SetColumn(2, -vForwardsNorm);

  const float fCosFovX = plMath::Cos(fovX * 0.5f);
  const float fSinFovX = plMath::Sin(fovX * 0.5f);

  const float fCosFovY = plMath::Cos(fovY * 0.5f);
  const float fSinFovY = plMath::Sin(fovY * 0.5f);

  // Left Plane
  {
    plVec3 vPlaneNormal = mLocalFrame * plVec3(-fCosFovX, 0, fSinFovX);
    vPlaneNormal.Normalize();

    res.m_Planes[LeftPlane] = plPlane::MakeFromNormalAndPoint(vPlaneNormal, vPosition);
  }

  // Right Plane
  {
    plVec3 vPlaneNormal = mLocalFrame * plVec3(fCosFovX, 0, fSinFovX);
    vPlaneNormal.Normalize();

    res.m_Planes[RightPlane] = plPlane::MakeFromNormalAndPoint(vPlaneNormal, vPosition);
  }

  // Bottom Plane
  {
    plVec3 vPlaneNormal = mLocalFrame * plVec3(0, -fCosFovY, fSinFovY);
    vPlaneNormal.Normalize();

    res.m_Planes[BottomPlane] = plPlane::MakeFromNormalAndPoint(vPlaneNormal, vPosition);
  }

  // Top Plane
  {
    plVec3 vPlaneNormal = mLocalFrame * plVec3(0, fCosFovY, fSinFovY);
    vPlaneNormal.Normalize();

    res.m_Planes[TopPlane] = plPlane::MakeFromNormalAndPoint(vPlaneNormal, vPosition);
  }

  if (res.IsValid())
  {
    out_frustum = std::move(res);
    return PL_SUCCESS;
  }

  return PL_FAILURE;
}

plFrustum plFrustum::MakeFromCorners(const plVec3 pCorners[FrustumCorner::CORNER_COUNT])
{
  plFrustum frustum;
  const plResult res = TryMakeFromCorners(frustum, pCorners);
  PL_ASSERT_DEV(res.Succeeded() && frustum.IsValid(), "Frustum is not valid after construction.");
  PL_IGNORE_UNUSED(res);
  return frustum;
}

plResult plFrustum::TryMakeFromCorners(plFrustum& out_frustum , const plVec3 pCorners[FrustumCorner::CORNER_COUNT])
{
  plFrustum res;

  res.m_Planes[PlaneType::LeftPlane] = plPlane::MakeFromPoints(pCorners[FrustumCorner::FarTopLeft], pCorners[FrustumCorner::NearBottomLeft], pCorners[FrustumCorner::NearTopLeft]);

  res.m_Planes[PlaneType::RightPlane] = plPlane::MakeFromPoints(pCorners[FrustumCorner::NearTopRight], pCorners[FrustumCorner::FarBottomRight], pCorners[FrustumCorner::FarTopRight]);

  res.m_Planes[PlaneType::BottomPlane] = plPlane::MakeFromPoints(pCorners[FrustumCorner::NearBottomLeft], pCorners[FrustumCorner::FarBottomRight], pCorners[FrustumCorner::NearBottomRight]);

  res.m_Planes[PlaneType::TopPlane] = plPlane::MakeFromPoints(pCorners[FrustumCorner::FarTopLeft], pCorners[FrustumCorner::NearTopRight], pCorners[FrustumCorner::FarTopRight]);

  res.m_Planes[PlaneType::FarPlane] = plPlane::MakeFromPoints(pCorners[FrustumCorner::FarTopLeft], pCorners[FrustumCorner::FarBottomRight], pCorners[FrustumCorner::FarBottomLeft]);

  res.m_Planes[PlaneType::NearPlane] = plPlane::MakeFromPoints(pCorners[FrustumCorner::NearTopLeft], pCorners[FrustumCorner::NearBottomRight], pCorners[FrustumCorner::NearTopRight]);

  if (res.IsValid())
  {
    out_frustum  = std::move(res);
    return PL_SUCCESS;
  }

  return PL_FAILURE;
}
