#pragma once
#include <math\Transform.h>
//#include <vector>
//#include <string>
#include <unordered_map>

struct Animation
{
	struct Frame
	{
		std::unordered_map<std::string, Transform> LocalTransforms;
	};

	std::vector<Frame> Frames;

	unsigned int Length;
	float FramesPerSecond;
	float Duration;
	std::string Name;
};