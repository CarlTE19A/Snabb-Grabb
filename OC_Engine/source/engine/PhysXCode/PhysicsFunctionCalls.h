#pragma once
#include <functional>
#include "Log.h"
class PhysicsFunctionCalls
{
public:
	inline void SetCollisionFunction(std::function<void(physx::PxShape*)> aFunction) 
	{
		myCollisionFunction = aFunction;
		isBound = true;
	}
	inline void CallCollisionFunction(physx::PxShape* aShape) 
	{
		if (!isBound)
		{
			LogError("No Collision Function Bound");
			return;
		}
		if (myCollisionFunction == nullptr)
		{
			LogError("Failed Collision Function");
			return;
		}
		if (aShape == nullptr)
		{
			LogError("Failed Collision Shape Detected");
			return;
		}
		myCollisionFunction(aShape);
	}
	//This is usally used for the damege of the object
	float myValue;

private:
	std::function<void(physx::PxShape*)> myCollisionFunction = nullptr;
	bool isBound = false;
};