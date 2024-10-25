#pragma once

#include <Foundation/Math/Vec3.h>

/// \brief An axis-aligned bounding box implementation.
///
/// This class allows to construct AABBs and also provides a large set of functions to work with them,
/// e.g. for overlap queries and ray casts.

template <typename Type>
class plBoundingBoxTemplate
{
public:
  // Means this object can be copied using memcpy instead of copy construction.
  PL_DECLARE_POD_TYPE();

  using ComponentType = Type;

public:
  /// \brief Default constructor does not initialize anything.
  plBoundingBoxTemplate();

  /// \brief Constructs the box with the given minimum and maximum values.
  plBoundingBoxTemplate(const plVec3Template<Type>& vMin, const plVec3Template<Type>& vMax); // [tested]

#if PL_ENABLED(PL_MATH_CHECK_FOR_NAN)
  void AssertNotNaN() const
  {
    PL_ASSERT_ALWAYS(!IsNaN(), "This object contains NaN values. This can happen when you forgot to initialize it before using it. Please check that "
                               "all code-paths properly initialize this object.");
  }
#endif

  /// \brief Creates a box that is located at the origin and has zero size. This is a 'valid' box.
  [[nodiscard]] static plBoundingBoxTemplate<Type> MakeZero();

  /// \brief Creates a box that is in an invalid state. ExpandToInclude can then be used to make it into a bounding box for objects.
  [[nodiscard]] static plBoundingBoxTemplate<Type> MakeInvalid(); // [tested]

  /// \brief Creates a box from a center point and half-extents for each axis.
  [[nodiscard]] static plBoundingBoxTemplate<Type> MakeFromCenterAndHalfExtents(const plVec3Template<Type>& vCenter, const plVec3Template<Type>& vHalfExtents); // [tested]

  /// \brief Creates a box with the given minimum and maximum values.
  [[nodiscard]] static plBoundingBoxTemplate<Type> MakeFromMinMax(const plVec3Template<Type>& vMin, const plVec3Template<Type>& vMax); // [tested]

  /// \brief Creates a box around the given set of points. If uiNumPoints is zero, the returned box is invalid (same as MakeInvalid() returns).
  [[nodiscard]] static plBoundingBoxTemplate<Type> MakeFromPoints(const plVec3Template<Type>* pPoints, plUInt32 uiNumPoints, plUInt32 uiStride = sizeof(plVec3Template<Type>)); // [tested]

  /// \brief Checks whether the box is in an invalid state.
  bool IsValid() const; // [tested]

  /// \brief Checks whether any component is NaN.
  bool IsNaN() const; // [tested]

  /// \brief Writes the 8 different corners of the box to the given array.
  void GetCorners(plVec3Template<Type>* out_pCorners) const; // [tested]

  /// \brief Returns the center position of the box.
  const plVec3Template<Type> GetCenter() const; // [tested]

  /// \brief Returns the extents of the box along each axis.
  const plVec3Template<Type> GetExtents() const; // [tested]

  /// \brief Returns the half extents of the box along each axis.
  const plVec3Template<Type> GetHalfExtents() const; // [tested]

  /// \brief Expands the box such that the given point is inside it.
  void ExpandToInclude(const plVec3Template<Type>& vPoint); // [tested]

  /// \brief Expands the box such that the given box is inside it.
  void ExpandToInclude(const plBoundingBoxTemplate& rhs); // [tested]

  /// \brief Expands the box such that all the given points are inside it.
  void ExpandToInclude(const plVec3Template<Type>* pPoints, plUInt32 uiNumPoints, plUInt32 uiStride = sizeof(plVec3Template<Type>)); // [tested]

  /// \brief If the box is not cubic all extents are set to the value of the maximum extent, such that the box becomes cubic.
  void ExpandToCube(); // [tested]

  /// \brief Will increase the size of the box in all directions by the given amount (per axis).
  void Grow(const plVec3Template<Type>& vDiff); // [tested]

  /// \brief Checks whether the given point is inside the box.
  bool Contains(const plVec3Template<Type>& vPoint) const; // [tested]

  /// \brief Checks whether the given box is completely inside this box.
  bool Contains(const plBoundingBoxTemplate& rhs) const; // [tested]

  /// \brief Checks whether all the given points are inside this box.
  bool Contains(const plVec3Template<Type>* pPoints, plUInt32 uiNumPoints, plUInt32 uiStride = sizeof(plVec3Template<Type>)) const; // [tested]

  /// \brief Checks whether the given sphere is completely inside this box.
  bool Contains(const plBoundingSphereTemplate<Type>& sphere) const; // [tested]

  /// \brief Checks whether this box overlaps with the given box.
  bool Overlaps(const plBoundingBoxTemplate& rhs) const; // [tested]

  /// \brief Checks whether any of the given points is inside this box.
  bool Overlaps(const plVec3Template<Type>* pPoints, plUInt32 uiNumPoints, plUInt32 uiStride = sizeof(plVec3Template<Type>)) const; // [tested]

  /// \brief Checks whether the given sphere overlaps with this box.
  bool Overlaps(const plBoundingSphereTemplate<Type>& sphere) const; // [tested]

  /// \brief Checks whether this box and the other box are exactly identical.
  bool IsIdentical(const plBoundingBoxTemplate& rhs) const; // [tested]

  /// \brief Checks whether this box and the other box are equal within some threshold.
  bool IsEqual(const plBoundingBoxTemplate& rhs, Type fEpsilon = plMath::DefaultEpsilon<Type>()) const; // [tested]

  /// \brief Moves the box by the given vector.
  void Translate(const plVec3Template<Type>& vDiff); // [tested]

  /// \brief Scales the box along each axis, but keeps its center constant.
  void ScaleFromCenter(const plVec3Template<Type>& vScale); // [tested]

  /// \brief Scales the box's corners by the given factors, thus also moves the box around.
  void ScaleFromOrigin(const plVec3Template<Type>& vScale); // [tested]

  /// \brief Transforms the corners of the box in its local space. The center of the box does not change, unless the transform contains a translation.
  void TransformFromCenter(const plMat4Template<Type>& mTransform); // [tested]

  /// \brief Transforms the corners of the box and recomputes the AABB of those transformed points. Rotations and scalings will influence the center position of the box.
  void TransformFromOrigin(const plMat4Template<Type>& mTransform); // [tested]

  /// \brief The given point is clamped to the volume of the box, i.e. it will be either inside the box or on its surface and it will have the closest
  /// possible distance to the original point.
  const plVec3Template<Type> GetClampedPoint(const plVec3Template<Type>& vPoint) const; // [tested]

  /// \brief Returns the squared minimum distance from the box's surface to the point. Zero if the point is inside the box.
  Type GetDistanceSquaredTo(const plVec3Template<Type>& vPoint) const; // [tested]

  /// \brief Returns the minimum squared distance between the two boxes. Zero if the boxes overlap.
  Type GetDistanceSquaredTo(const plBoundingBoxTemplate& rhs) const; // [tested]

  /// \brief Returns the minimum distance from the box's surface to the point. Zero if the point is inside the box.
  Type GetDistanceTo(const plVec3Template<Type>& vPoint) const; // [tested]

  /// \brief Returns the minimum distance between the box and the sphere. Zero or negative if both overlap.
  Type GetDistanceTo(const plBoundingSphereTemplate<Type>& sphere) const; // [tested]

  /// \brief Returns the minimum distance between the two boxes. Zero if the boxes overlap.
  Type GetDistanceTo(const plBoundingBoxTemplate& rhs) const; // [tested]

  /// \brief Returns whether the given ray intersects the box. Optionally returns the intersection distance and position.
  /// Note that vRayDir is not required to be normalized.
  bool GetRayIntersection(const plVec3Template<Type>& vStartPos, const plVec3Template<Type>& vRayDir, Type* out_pIntersectionDistance = nullptr,
    plVec3Template<Type>* out_pIntersection = nullptr) const; // [tested]

  /// \brief Checks whether the line segment intersects the box. Optionally returns the intersection point and the fraction along the line segment
  /// where the intersection occurred.
  bool GetLineSegmentIntersection(const plVec3Template<Type>& vStartPos, const plVec3Template<Type>& vEndPos, Type* out_pLineFraction = nullptr,
    plVec3Template<Type>* out_pIntersection = nullptr) const; // [tested]

  /// \brief Returns a bounding sphere that encloses this box.
  const plBoundingSphereTemplate<Type> GetBoundingSphere() const; // [tested]


public:
  plVec3Template<Type> m_vMin;
  plVec3Template<Type> m_vMax;
};

/// \brief Checks whether this box and the other are identical.
template <typename Type>
bool operator==(const plBoundingBoxTemplate<Type>& lhs, const plBoundingBoxTemplate<Type>& rhs); // [tested]

/// \brief Checks whether this box and the other are not identical.
template <typename Type>
bool operator!=(const plBoundingBoxTemplate<Type>& lhs, const plBoundingBoxTemplate<Type>& rhs); // [tested]


#include <Foundation/Math/Implementation/BoundingBox_inl.h>
