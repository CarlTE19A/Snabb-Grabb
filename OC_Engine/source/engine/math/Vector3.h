#pragma once

template <class T>
class Vector3
{
public:
	T x;
	T y;
	T z;
	//Creates a null-vector
	Vector3<T>()
	{
		x = 0;
		y = 0;
		z = 0;
	};

	//Creates a vector (aX, aY, aZ)
	Vector3<T>(const T& aX, const T& aY, const T& aZ)
	{
		x = aX;
		y = aY;
		z = aZ;
	};

	//Copy constructor (compiler generated)
	Vector3<T>(const Vector3<T>& aVector) = default;
	//Assignment operator (compiler generated)
	Vector3<T>& operator=(const Vector3<T>& aVector3) = default;
	//Destructor (compiler generated)
	~Vector3<T>() = default;
	//Returns the squared length of the vector
	T LengthSqr() const
	{
		return x * x + y * y + z * z;
	};
	//Returns the length of the vector
	T Length() const
	{
		return std::sqrt(Vector3<T>::LengthSqr());
	};
	//Returns a normalized copy of this
	Vector3<T> GetNormalized() const
	{
		if (Length() == 0) return Vector3<T>();

		return *this / Length();
	};
	//Normalizes the vector
	void Normalize()
	{
		if (Length() == 0) return;

		*this /= Length();
	};
	//Returns the dot product of this and aVector
	T Dot(const Vector3<T>& aVector) const
	{
		//T a = (x * aVector.x) + (y * aVector.y) + (z * aVector.z);
		T ax = (x * aVector.x);
		T ay = (y * aVector.y);
		T az = (z * aVector.z);
		return ax + ay + az;
	};
	//Returns the cross product of this and aVector
	Vector3<T> Cross(const Vector3<T>& aVector) const
	{
		return Vector3<T>(
			y * aVector.z - z * aVector.y,
			z * aVector.x - x * aVector.z,
			x * aVector.y - y * aVector.x
		);
	};

	static Vector3<T> Lerp(const Vector3<T>& aStart, const Vector3<T>& aEnd, const float aPercent)
	{
		return (aStart + aPercent * (aEnd - aStart));
	};
};
//Returns the vector sum of aVector0 and aVector1
template <class T> Vector3<T> operator+(const Vector3<T>& aVector0, const Vector3<T>& aVector1)
{
	return Vector3<T>(
		aVector0.x + aVector1.x,
		aVector0.y + aVector1.y,
		aVector0.z + aVector1.z
	);
}
//Returns the vector difference of aVector0 and aVector1
template <class T> Vector3<T> operator-(const Vector3<T>& aVector0, const Vector3<T>& aVector1)
{
	return Vector3<T>(
		aVector0.x - aVector1.x,
		aVector0.y - aVector1.y,
		aVector0.z - aVector1.z
	);
}
//Returns the vector aVector multiplied by the scalar aScalar
template <class T> Vector3<T> operator*(const Vector3<T>& aVector, const T& aScalar)
{
	return Vector3<T>(
		aVector.x * aScalar,
		aVector.y * aScalar,
		aVector.z * aScalar
	);
}
//Returns the vector aVector multiplied by the scalar aScalar
template <class T> Vector3<T> operator*(const T& aScalar, const Vector3<T>& aVector)
{
	return Vector3<T>(
		aVector.x * aScalar,
		aVector.y * aScalar,
		aVector.z * aScalar
	);
}
//Returns the vector aVector divided by the scalar aScalar (equivalent to aVector multiplied by 1 / aScalar)
template <class T> Vector3<T> operator/(const Vector3<T>& aVector, const T& aScalar)
{
	return Vector3<T>(
		aVector.x / aScalar,
		aVector.y / aScalar,
		aVector.z / aScalar
	);
}
//Equivalent to setting aVector0 to (aVector0 + aVector1)
template <class T> void operator+=(Vector3<T>& aVector0, const Vector3<T>& aVector1)
{
	aVector0.x += aVector1.x;
	aVector0.y += aVector1.y;
	aVector0.z += aVector1.z;
}
//Equivalent to setting aVector0 to (aVector0 - aVector1)
template <class T> void operator-=(Vector3<T>& aVector0, const Vector3<T>& aVector1)
{
	aVector0.x -= aVector1.x;
	aVector0.y -= aVector1.y;
	aVector0.z -= aVector1.z;
}
//Equivalent to setting aVector to (aVector * aScalar)
template <class T> void operator*=(Vector3<T>& aVector, const T& aScalar)
{
	aVector.x *= aScalar;
	aVector.y *= aScalar;
	aVector.z *= aScalar;
}
//Equivalent to setting aVector to (aVector / aScalar)
template <class T> void operator/=(Vector3<T>& aVector, const T& aScalar)
{
	aVector.x /= aScalar;
	aVector.y /= aScalar;
	aVector.z /= aScalar;
}
//Equivalent to setting aVector to (aVector / aScalar)
template <class T> Vector3<T> operator*(Vector3<T>& aVector, const Vector3<T>& aOther)
{
	return Vector3<T>(
		aVector.x * aOther.x,
		aVector.y * aOther.y,
		aVector.z * aOther.z
	);
}
//Equivalent to setting aVector to (aVector / aScalar)
template <class T> void operator*=(Vector3<T>& aVector, const Vector3<T>& aOther)
{
	aVector.x *= aOther.x;
	aVector.y *= aOther.y;
	aVector.z *= aOther.z;
}
//Equivalent to x == x && y == y && z == z
template <class T> bool operator==(Vector3<T> aVector, const Vector3<T> aOther)
{
	return (aVector.x == aOther.x && aVector.y == aOther.y && aVector.z == aOther.z);
}
//Equivalent to x != x || y != y || z != z
template <class T> bool operator!=(Vector3<T> aVector, const Vector3<T> aOther)
{
	return (aVector.x != aOther.x || aVector.y != aOther.y || aVector.z != aOther.z);
}
