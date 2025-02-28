#pragma once
#include "Vector3.h"

template<class T>
class AABB3D
{
private:

	Vector3<T> myCenter;
	Vector3<T> mySize;
	Vector3<T> myMin;
	Vector3<T> myMax;
	Vector3<T> myOriginalMin;
	Vector3<T> myOriginalMax;
	float x = 50;
	float z = 50;

public:
	// Default constructor: there is no AABB, both min and max points are the zero vector.
	AABB3D() : myMin(), myMax()
	{

	};
	// Copy constructor.
	AABB3D(const AABB3D<T>& aAABB3D)
	{
		myMin = aAABB3D.myMin;
		myMax = aAABB3D.myMax;
		myOriginalMin = aAABB3D.myMin;
		myOriginalMax = aAABB3D.myMax;
	};
	// Constructor taking the positions of the minimum and maximum corners.
	AABB3D(const Vector3<T>& aMin, const Vector3<T>& aMax)
	{
		InitWithMinAndMax(aMin, aMax);
	};
	// Init the AABB with the positions of the minimum and maximum corners, same as
	// the constructor above.
	void InitWithMinAndMax(const Vector3<T>& aMin, const Vector3<T>& aMax)
	{
		myMin = aMin;
		myMax = aMax;

		myOriginalMin = myMin;
		myOriginalMax = myMax;
	};
	void SetCenterAndSize(const Vector3<T>& aCenter, const Vector3<T>& aSize)
	{
		myCenter = aCenter;
		mySize = aSize;
	}
	// Returns whether a point is inside the AABB: it is inside when the point is on anyA
	// of the AABB's sides or inside of the AABB.
	bool IsInside(const Vector3<T>& aPosition) const
	{
		if (myMax.x < aPosition.x) return false;
		if (myMin.x > aPosition.x) return false;
		if (myMax.y < aPosition.y) return false;
		if (myMin.y > aPosition.y) return false;
		if (myMax.z < aPosition.z) return false;
		if (myMin.z > aPosition.z) return false;
		return true; // AABB överlappar
	};

	bool IsInside(AABB3D& aAABB3D)
	{
		if (myMax.x > aAABB3D.myMin.x &&
			myMin.x < aAABB3D.myMax.x &&
			myMax.y > aAABB3D.myMin.y &&
			myMin.y < aAABB3D.myMax.y &&
			myMax.z > aAABB3D.myMin.z &&
			myMin.z < aAABB3D.myMax.z)
		{
			


			return true;
		}

		return false;
	}


	bool IsInside(const Vector3<T>& aPosition, const Vector3<T>& aNormal) const
	{
		return (
			((aPosition.x >= myMin.x) &&
				(aPosition.x <= myMax.x) || aNormal.x != 0) &&
			((aPosition.y >= myMin.y) &&
				(aPosition.y <= myMax.y) || aNormal.y != 0) &&
			((aPosition.z >= myMin.z) &&
				(aPosition.z <= myMax.z) || aNormal.z != 0)
			);
	}

	const Vector3<T>& GetMin() const
	{
		return myMin;
	};
	const Vector3<T>& GetMax() const
	{
		return myMax;
	};

	void SetMinMax(const Vector3f& aPosition)

	{
		myMax = myOriginalMax;
		myMin = myOriginalMin;

		// Offset the box collision based on the 
		// s position
		Vector3f offset = aPosition;

		// Adjust the offset based on the dimensions of the box
		offset.z += -myMax.z+z;
		offset.x += -myMin.x-x;

		// Apply the adjusted offset to the box collision


		myMin += offset;
		myMax += offset;

	/*	myMin += aPosition;
		myMax += aPosition;*/
	}
};