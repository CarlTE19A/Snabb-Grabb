#pragma once
#include <array>
#include "math\Vector.h"
#include "graphics\model\Vertex.h"


class CubePrimitive
{
public:
	CubePrimitive();
	~CubePrimitive();

	inline static const std::array<SimpleVertex, 8>& GetVertices() { return myVertices; };
	inline static const std::array<int, 24>& GetIndices() { return myIndices; };
private:
	inline static std::array<SimpleVertex, 8> myVertices;
	inline static std::array<int, 24> myIndices;
	inline static bool myIsInitialized = false;
};