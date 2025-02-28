#pragma once
#include "Vector.h"
#include "Line.h"
#include "Plane.h"
#include "Sphere.h"
#include "Ray.h"
#include "AABB3D.h"
#include <cmath>

#include "graphics\model\Mesh.h"

// If the ray does not intersect the plane aOutIntersectionPoint remains unchanged and false is returned.
// If the ray intersects the plane the intersection point is stored in aOutIntersectionPoint and true is returned.
template<class T>
inline bool IntersectionPlaneRay(const Plane<T>& aPlane, const Ray<T>& aRay, Vector3<T>& aOutIntersectionPoint)
{
	//Vector3<T> n = aPlane.GetNormal();
	//Vector3<T> p = aRay.GetOrigin();


	//T i = Vector3<T>(aPlane.GetPoint() - p).Dot(n);
	//if (i > -0.000001f && i < 0.000001f)
	//{
	//	aOutIntersectionPoint = aPlane.GetPoint();
	//	return true; // is inside plane
	//}



	//T dirdn = aRay.GetDirection().Dot(aPlane.GetNormal());

	//if (dirdn == 0)
	//{
	//	return false;
	//}

	//T d = aPlane.GetPoint().Dot(aPlane.GetNormal());
	//T p0n = aRay.GetOrigin().Dot(aPlane.GetNormal());
	//T distance = (d - p0n) / dirdn;
	////T distance = aPlane.GetPoint().Dot(aPlane.GetNormal()) - (aRay.GetOrigin().Dot(aPlane.GetNormal())) / aRay.GetDirection().Dot(aPlane.GetNormal());

	//if (distance < 0)
	//{
	//	return false;
	//}

	//aOutIntersectionPoint = aRay.GetOrigin() + aRay.GetDirection() * distance;
	//return true;

	Vector3<T> n = aPlane.GetNormal();
	Vector3<T> p = aRay.GetOrigin();

	T i = Vector3<T>(aPlane.GetPoint() - p).Dot(n);
	if (i > -0.000001f && i < 0.000001f)
	{
		aOutIntersectionPoint = aPlane.GetPoint();
		return true; // is inside plane
	}

	if (aRay.GetDirection().Dot(aPlane.GetNormal()) == 0.f)
	{
		return false;
	}

	T distance = (aPlane.GetPoint().Dot(aPlane.GetNormal()) - aRay.GetOrigin().Dot(aPlane.GetNormal())) / aRay.GetDirection().Dot(aPlane.GetNormal());

	if (distance < 0)
	{
		return false;
	}

	aOutIntersectionPoint = aRay.GetOrigin() + aRay.GetDirection() * distance;
	return true;

}

template<typename T>
inline bool IsInside(const T& aRayOriginValue, const T& aRayDeltaValue, const T& aMinValue, const T& aMaxValue, T& anOutValue)
{
	if (aRayOriginValue < aMinValue)
	{
		anOutValue = aMinValue - aRayOriginValue;
		if (anOutValue > aRayDeltaValue) return false;
		anOutValue /= aRayDeltaValue;
	}
	else if (aRayOriginValue > aMaxValue)
	{
		anOutValue = aMaxValue - aRayOriginValue;
		if (anOutValue < aRayDeltaValue) return false;
		anOutValue /= aRayDeltaValue;
	}
	else
	{
		anOutValue = 1.0f;
	}
	return true;
}
// If the ray intersects the AABB, true is returned, if not, false is returned.
// Any ray starting on the inside is considered to intersect the AABB
//template<typename T>
//inline bool IntersectionAABBRay(const AABB3D<T>& aAABB, const Ray<T>& aRay, Vector3<T>& aOutIntersectionPoint)
//{
//	const Vector3<T> rayOrg = aRay.GetOrigin();
//	const Vector3<T> rayDelta = aRay.GetDirection();
//	const Vector3<T> min = aAABB.GetMin();
//	const Vector3<T> max = aAABB.GetMax();
//
//	T xout, yout, zout;
//	if (!IsInside(rayOrg.x, rayDelta.x, min.x, max.x, xout)) return false;
//	if (!IsInside(rayOrg.y, rayDelta.y, min.y, max.y, yout)) return false;
//	if (!IsInside(rayOrg.z, rayDelta.z, min.z, max.z, zout)) return false;
//
//	// Select farthest plane - This is the plane of intersection.
//	int which = 0;
//	T t = xout;
//	if (yout > t)
//	{
//		which = 1;
//		t = yout;
//	}
//	if (zout > t)
//	{
//		which = 2;
//		t = zout;
//	}
//
//	T x = T(), y = T(), z = T();
//
//	switch (which)
//	{
//	case 0: // Intersect with yz plane
//	{
//		y = rayOrg.y + rayDelta.y * t;
//		if (y < min.y || y > max.y) return false;
//		z = rayOrg.z + rayDelta.z * t;
//		if (z < min.z || z > max.z) return false;
//	}
//	break;
//	case 1: // Intersect with xz plane
//	{
//		x = rayOrg.x + rayDelta.x * t;
//		if (x < min.x || x > max.x) return false;
//		z = rayOrg.z + rayDelta.z * t;
//		if (z < min.z || z > max.z) return false;
//	}
//	break;
//	case 2: // Intersect with xy plane
//	{
//		x = rayOrg.x + rayDelta.x * t;
//		if (x < min.x || x > max.x) return false;
//		y = rayOrg.y + rayDelta.y * t;
//		if (y < min.y || y > max.y) return false;
//	}
//	break;
//	}
//
//	aOutIntersectionPoint = Vector3<T>{ x, y, z };
//	return true;
//}

template<typename T>
bool IntersectionAABBRay(const AABB3D<T>& aAABB, const Ray<T>& aRay, Vector3<T>& aOutIntersectionPoint, Vector3<T>& aOutNormalDirection)
{
	if (aAABB.IsInside(aRay.GetOrigin()))
	{
		aOutIntersectionPoint = aRay.GetOrigin();
		return true;
	}

	const Vector3<T> min = aAABB.GetMin();
	const Vector3<T> max = aAABB.GetMax();
	Plane<T> planes[6];
	planes[0] = Plane<T>(min, Vector3<T>(-1, 0, 0));
	planes[1] = Plane<T>(min, Vector3<T>(0, -1, 0));
	planes[2] = Plane<T>(min, Vector3<T>(0, 0, -1));
	planes[3] = Plane<T>(max, Vector3<T>(1, 0, 0));
	planes[4] = Plane<T>(max, Vector3<T>(0, 1, 0));
	planes[5] = Plane<T>(max, Vector3<T>(0, 0, 1));

	float intersectionDistance = FLT_MAX;
	Vector3<T> closestIntersectionPoint;
	int finalPlane = 0;

	for (int i = 0; i < 6; i++)
	{
		if (IntersectionPlaneRay(planes[i], aRay, aOutIntersectionPoint))
		{
			if (aAABB.IsInside(aOutIntersectionPoint, planes[i].GetNormal()))
			{
				Vector3<T> distance = aOutIntersectionPoint - aRay.GetOrigin();

				if (distance.Length() < intersectionDistance)
				{
					intersectionDistance = static_cast<float>(distance.Length());
					closestIntersectionPoint = aOutIntersectionPoint;
					finalPlane = i;
				}
			}
		}
	}

	if (intersectionDistance == FLT_MAX)
	{
		return false;
	}

	aOutNormalDirection = planes[finalPlane].GetNormal();
	aOutIntersectionPoint = closestIntersectionPoint;
	return true;
}


// If the ray intersects the sphere, true is returned, if not, false is returned.
// Any ray starting on the inside is considered to intersect the sphere
template<class T>
inline bool IntersectionSphereRay(const Sphere<T>& aSphere, const Ray<T>& aRay, Vector3<T>& aOutIntersectionPoint)
{
	const Vector3<T>& p0 = aRay.GetOrigin();
	const Vector3<T>& d = aRay.GetDirection().GetNormalized();
	const Vector3<T>& c = aSphere.GetCenter();

	const Vector3<T> e = c - p0;
	if (e.GetNormalized().Dot(d) <= 0) return false;

	const T a = e.Dot(d);
	const T b = e.LengthSqr() - a * a;


	if (b > aSphere.GetRadius() * aSphere.GetRadius()) return false;

	const T fArg = aSphere.GetRadius() * aSphere.GetRadius() - e.LengthSqr() + a * a;

	if (fArg < 0) return false;

	const T t = a - sqrt(fArg);

	aOutIntersectionPoint = d * t + p0;

	return true;
}

inline bool IntersectLineLine(Linef& aLine0, Linef& aLine1, Vector2f& aOutIntersectPoint)
{
	float x1 = aLine0.myPoint.x;
	float y1 = aLine0.myPoint.y;
	float x2 = aLine1.myPoint.x;
	float y2 = aLine1.myPoint.y;



	float m1 = aLine0.myDirection.y / aLine0.myDirection.x;
	float m2 = aLine1.myDirection.y / aLine1.myDirection.x;



	if (!std::isfinite(abs(m1)) && !std::isfinite(abs(m2))) {
		// Lines are both vertical (perpendicular), and they intersect at (x1, y2) or (x2, y1).
		aOutIntersectPoint.x = x1;
		aOutIntersectPoint.y = y2;
		return true;
	}



	if (!std::isfinite(abs(m1))) {
		// Line aLine0 is vertical, and the intersection is calculated using the equation of line aLine1.
		aOutIntersectPoint.x = x1;
		aOutIntersectPoint.y = m2 * (x1 - x2) + y2;
		return true;
	}



	if (!std::isfinite(abs(m2))) {
		// Line aLine1 is vertical, and the intersection is calculated using the equation of line aLine0.
		aOutIntersectPoint.x = x2;
		aOutIntersectPoint.y = m1 * (x2 - x1) + y1;
		return true;
	}



	if (m1 != m2) {
		aOutIntersectPoint.x = (y2 - y1 + m1 * x1 - m2 * x2) / (m1 - m2);
		aOutIntersectPoint.y = m1 * (aOutIntersectPoint.x - x1) + y1;
		return true;
	}



	return false;

}

template<class T>
inline bool IntersectTriangleRay(Vector3<T> aFirstCorner, Vector3<T> aSecondCorner, Vector3<T> aThirdCorner, const Ray<T>& aRay, Vector3<T>& aOutIntersectionPoint)
{
	Vector3<T> e1 = aSecondCorner - aFirstCorner;
	Vector3<T> e2 = aThirdCorner - aFirstCorner;
	Vector3<T> p = aRay.GetDirection().Cross(e2);
	float a = e1.Dot(p);

	if (a > -0.000001f && a < 0.000001f)
	{
		return false;
	}

	float f = 1 / a;
	Vector3<T> s = aRay.GetOrigin() - aFirstCorner;
	float u = f * s.Dot(p);

	if (u < 0.0f || u > 1.0f)
	{
		return false;
	}

	Vector3<T> q = s.Cross(e1);
	float v = f * aRay.GetDirection().Dot(q);

	if (v < 0.0f || u + v > 1.0f)
	{
		return false;
	}

	float t = f * e2.Dot(q);

	if (t > 0.000001f)
	{
		aOutIntersectionPoint = aRay.GetOrigin() + aRay.GetDirection() * t;
		return true;
	}

	return false;
}