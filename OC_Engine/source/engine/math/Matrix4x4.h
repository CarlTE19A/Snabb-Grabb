#pragma once
#include "Vector.h"

//#include <DirectXMath.h>
/*
template <class T>
class Matrix4x4
{
private:
	T myContainer[4][4];
public:
	// Creates the identity matrix.
	Matrix4x4<T>()
	{
		for (unsigned int y = 0; y < 4; y++)
		{
			for (unsigned int x = 0; x < 4; x++)
			{
				myContainer[y][x] = T() + x == y ? 1 : T();
			}
		}
	};

	// Copy Constructor.
	Matrix4x4<T>(const Matrix4x4<T>& aMatrix)
	{
		(*this) = aMatrix;
	};

	Matrix4x4<T>& operator=(const Matrix4x4<T>& aMatrix)
	{
		for (unsigned int y = 1; y <= 4; y++)
		{
			for (unsigned int x = 1; x <= 4; x++)
			{
				(*this)(y, x) = aMatrix(y, x);
			}
		}
		return *this;
	}

	// () operator for accessing element (row, column) for read/write or read, respectively.
	T& operator()(const unsigned int aRow, const unsigned int aColumn)
	{
		return myContainer[aRow - 1][aColumn - 1];
	};

	const T& operator()(const unsigned int aRow, const unsigned int aColumn) const
	{
		return myContainer[aRow - 1][aColumn - 1];
	};

	inline Vector3<T> Matrix4x4<T>::GetForward() const
	{
		Matrix4x4<T> rotation = *this;
		rotation(4, 1) = 0;
		rotation(4, 2) = 0;
		rotation(4, 3) = 0;
		Vector4<T> rotMult = Vector4<T>(0, 0, 1.f, 1.f);
		Vector4<T> result = rotMult * rotation;
		return { result.x, result.y, result.z };
	}

	static Matrix4x4<T> CreateScaleMatrix(const Vector3<T>& aScaleVector)
	{
		Matrix4x4<T> result;
		result.myContainer[0][0] = aScaleVector.x;
		result.myContainer[1][1] = aScaleVector.y;
		result.myContainer[2][2] = aScaleVector.z;

		return result;
	}

	static Matrix4x4<T> CreateTranslationMatrix(T x, T y, T z)
	{
		Matrix4x4<T> result;

		result(4, 1) = x;
		result(4, 2) = y;
		result(4, 3) = z;

		return result;
	}

	static Matrix4x4<T> CreateRollPitchYawMatrix(T x, T y, T z)
	{
		T X;
		T Y;
		T Z;
		T W;
		{
			const float radConst = 3.141f / 180.f;
			T cx = cos(x * radConst * T(0.5));
			T cy = cos(y * radConst * T(0.5));
			T cz = cos(z * radConst * T(0.5));
			T sx = sin(x * radConst * T(0.5));
			T sy = sin(y * radConst * T(0.5));
			T sz = sin(z * radConst * T(0.5));

			W = cx * cy * cz + sx * sy * sz; // W
			X = sx * cy * cz - cx * sy * sz; // X
			Y = cx * sy * cz + sx * cy * sz; // Y
			Z = cx * cy * sz - sx * sy * cz; // Z
		}


		Matrix4x4<T> result;

		T qxx(Y * Y);
		T qyy(Z * Z);
		T qzz(W * W);

		T qxz(Y * W);
		T qxy(Y * Z);
		T qyz(Z * W);

		T qwx(X * Y);
		T qwy(X * Z);
		T qwz(X * W);

		result(1, 1) = T(1) - T(2) * (qyy + qzz);
		result(1, 2) = T(2) * (qxy + qwz);
		result(1, 3) = T(2) * (qxz - qwy);

		result(2, 1) = T(2) * (qxy - qwz);
		result(2, 2) = T(1) - T(2) * (qxx + qzz);
		result(2, 3) = T(2) * (qyz + qwx);

		result(3, 1) = T(2) * (qxz + qwy);
		result(3, 2) = T(2) * (qyz - qwx);
		result(3, 3) = T(1) - T(2) * (qxx + qyy);

		result(4, 1) = 0;
		result(4, 2) = 0;
		result(4, 3) = 0;
		result(4, 4) = 1;

		return result;
	}

	// Static functions for creating rotation matrices.
	static Matrix4x4<T> CreateRotationAroundX(T aAngleInRadians)
	{
		Matrix4x4<T> rotationMatrix;
		rotationMatrix(2, 2) = cos(aAngleInRadians);
		rotationMatrix(2, 3) = sin(aAngleInRadians);

		rotationMatrix(3, 2) = -sin(aAngleInRadians);
		rotationMatrix(3, 3) = cos(aAngleInRadians);

		return rotationMatrix;
	};

	static Matrix4x4<T> CreateRotationAroundY(T aAngleInRadians)
	{
		Matrix4x4<T> rotationMatrix;
		rotationMatrix(1, 1) = cos(aAngleInRadians);
		rotationMatrix(1, 3) = -sin(aAngleInRadians);

		rotationMatrix(3, 1) = sin(aAngleInRadians);
		rotationMatrix(3, 3) = cos(aAngleInRadians);

		return rotationMatrix;
	};

	static Matrix4x4<T> CreateRotationAroundZ(T aAngleInRadians)
	{
		Matrix4x4<T> rotationMatrix;
		rotationMatrix(1, 1) = cos(aAngleInRadians);
		rotationMatrix(1, 2) = sin(aAngleInRadians);

		rotationMatrix(2, 1) = -sin(aAngleInRadians);
		rotationMatrix(2, 2) = cos(aAngleInRadians);

		return rotationMatrix;
	};

	// Static function for creating a transpose of a matrix.
	static Matrix4x4<T> Transpose(const Matrix4x4<T>& aMatrixToTranspose)
	{
		Matrix4x4<T> container = Matrix4x4<T>(aMatrixToTranspose);

		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				container(j + 1, i + 1) = aMatrixToTranspose(i + 1, j + 1);
			}
		}

		return container;
	};

	static Matrix4x4<T> GetFastInverse(const Matrix4x4<T>& aTransform)
	{
		Matrix4x4<T> container = Matrix4x4<T>(aTransform);

		Matrix4x4<T> inverseRotationMatrix = Transpose(aTransform);

		Vector4<T> v = { -aTransform(4, 1), -aTransform(4, 2) , -aTransform(4, 3), 1 };
		v = v * inverseRotationMatrix;

		for (unsigned int y = 1; y <= 3; y++) {
			for (unsigned int x = 1; x <= 3; x++) {
				container(y, x) = inverseRotationMatrix(y, x);
			}
		}

		container(4, 1) = v.x;
		container(4, 2) = v.y;
		container(4, 3) = v.z;

		return container;
	};

	static Vector3f GetEulerRotation()
	{
		float sy = sqrt(myContainer[0][0] * myContainer[0][0] + myContainer[1][0] * myContainer[1][0]);
		bool singular = sy < 1e-6; // If

		float x, y, z;
		if (!singular)
		{
			x = atan2(myContainer[2][1], myContainer[2][2]);
			y = atan2(-myContainer[2][0], sy);
			z = atan2(myContainer[1][0], myContainer[0][0]);
		}
		else
		{
			x = atan2(-myContainer[1][2], myContainer[1][1]);
			y = atan2(-myContainer[2][0], sy);
			z = 0;
		}
		return Vector3f(x, y, z);
	}

};

template <class T> Matrix4x4<T> operator+(const Matrix4x4<T>& aMatrix0, const Matrix4x4<T>& aMatrix1)
{
	Matrix4x4<T> result;
	for (unsigned int y = 1; y <= 4; y++)
	{
		for (unsigned int x = 1; x <= 4; x++)
		{
			result(y, x) = aMatrix0(y, x) + aMatrix1(y, x);
		}
	}
	return result;
}

template <class T> Matrix4x4<T> operator+=(Matrix4x4<T>& aMatrix0, const Matrix4x4<T>& aMatrix1)
{
	aMatrix0 = aMatrix0 + aMatrix1;
	return aMatrix0;
}

template <class T> Matrix4x4<T> operator-(const Matrix4x4<T>& aMatrix0, const Matrix4x4<T>& aMatrix1)
{
	Matrix4x4<T> result;
	for (unsigned int y = 1; y <= 4; y++)
	{
		for (unsigned int x = 1; x <= 4; x++)
		{
			result(y, x) = aMatrix0(y, x) - aMatrix1(y, x);
		}
	}
	return result;
}

template <class T> Matrix4x4<T> operator-=(Matrix4x4<T>& aMatrix0, const Matrix4x4<T>& aMatrix1)
{
	aMatrix0 = aMatrix0 - aMatrix1;
	return aMatrix0;
}

template <class T> Matrix4x4<T> operator*(const Matrix4x4<T>& aMatrix0, const Matrix4x4<T>& aMatrix1)
{
	Matrix4x4<T> result;
	for (unsigned int i = 1; i <= 4; i++)
	{
		for (unsigned int j = 1; j <= 4; j++)
		{
			T sum = T();

			for (unsigned int k = 1; k <= 4; k++)
			{
				sum += aMatrix0(i, k) * aMatrix1(k, j);
			}

			result(i, j) = sum;
		}
	}
	return result;
}

template <class T> Matrix4x4<T> operator*(const Matrix4x4<T>& aMatrix0, const DirectX::XMMATRIX& aMatrix1)
{
	Matrix4x4<T> result;
	for (unsigned int i = 1; i <= 4; i++)
	{
		for (unsigned int j = 1; j <= 4; j++)
		{
			T sum = T();

			for (unsigned int k = 1; k <= 4; k++)
			{
				sum += aMatrix0(i, k) * aMatrix1(k, j);
			}

			result(i, j) = sum;
		}
	}
	return result;
}

template <class T> Matrix4x4<T> operator*=(Matrix4x4<T>& aMatrix0, const Matrix4x4<T>& aMatrix1)
{
	aMatrix0 = aMatrix0 * aMatrix1;
	return aMatrix0;
}

template <class T> Vector4<T> operator*(const Vector4<T>& aVector, const Matrix4x4<T>& aMatrix)
{
	return Vector4<T>(
		aVector.x * aMatrix(1, 1) + aVector.y * aMatrix(2, 1) + aVector.z * aMatrix(3, 1) + aVector.w * aMatrix(4, 1),
		aVector.x * aMatrix(1, 2) + aVector.y * aMatrix(2, 2) + aVector.z * aMatrix(3, 2) + aVector.w * aMatrix(4, 2),
		aVector.x * aMatrix(1, 3) + aVector.y * aMatrix(2, 3) + aVector.z * aMatrix(3, 3) + aVector.w * aMatrix(4, 3),
		aVector.x * aMatrix(1, 4) + aVector.y * aMatrix(2, 4) + aVector.z * aMatrix(3, 4) + aVector.w * aMatrix(4, 4)
	);
}

template <class T> bool operator==(const Matrix4x4<T>& aMatrix0, const Matrix4x4<T>& aMatrix1)
{
	for (unsigned int y = 1; y <= 4; y++)
	{
		for (unsigned int x = 1; x <= 4; x++)
		{
			if (aMatrix0(y, x) != aMatrix1(y, x)) return false;
		}
	}
	return true;
}

*/