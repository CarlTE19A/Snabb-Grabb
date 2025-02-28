#pragma once
#include <vector>

#include "math\Vector.h"
#include "graphics\model\Vertex.h"

class SpherePrimitive
{
public:
	SpherePrimitive(float radius, int slices, int stacks);
	~SpherePrimitive() {};

	inline static const std::vector<SimplestVertex>& GetVertices() { return myVertices; };
	inline static const std::vector<unsigned int>& GetIndices() { return myIndices; };

private:
	inline static std::vector<SimplestVertex> myVertices;
	inline static std::vector<unsigned int> myIndices;
};
