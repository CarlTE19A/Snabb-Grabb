#include "stdafx.h"
#include "SpherePrimitive.h"
SpherePrimitive::SpherePrimitive(float radius, int slices, int stacks)
{
	for (int i = 0; i <= stacks; ++i)
	{
		float phi = static_cast<float>(i) / stacks * 3.14159265359f;

		for (int j = 0; j <= slices; ++j)
		{
			float theta = static_cast<float>(j) / slices * 2.0f * 3.14159265359f;

			SimplestVertex vertex;
			vertex.x = radius * sin(phi) * cos(theta);
			vertex.y = radius * cos(phi);
			vertex.z = radius * sin(phi) * sin(theta);
			vertex.w = 1.0f;

			myVertices.push_back(vertex);
		}
	}

	for (int i = 0; i < stacks; ++i)
	{
		for (int j = 0; j < slices; ++j)
		{
			int first = i * (slices + 1) + j;
			int second = first + slices + 1;

			myIndices.push_back(static_cast<unsigned int>(first + 1));
			myIndices.push_back(static_cast<unsigned int>(second));
			myIndices.push_back(static_cast<unsigned int>(first));

			myIndices.push_back(static_cast<unsigned int>(first + 1));
			myIndices.push_back(static_cast<unsigned int>(second + 1));
			myIndices.push_back(static_cast<unsigned int>(second));
		}
	}
}