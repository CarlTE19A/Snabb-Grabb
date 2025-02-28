#pragma once

#include "math/Vector.h"
#include <vector>

class GameObject;

class Door
{
public:
	Door();
	~Door();

	GameObject* CreateDoor(Vector3f aPosition, float aRotationX, float aRotationY);
	void FixDoor(std::vector<GameObject*>& aDoorPart);
	void FixMainDoor(GameObject* aGameObject);
	void FixDoorPart(GameObject* aGameObject);
	void FixFramePart(GameObject* aGameObject);

private:

	std::vector<GameObject*> CreateFracturedDoorVertical(Vector3f aPosition, float aRotationX, float aRotationY, GameObject* aParent);
	std::vector<GameObject*> CreateFracturedDoorHorizontal(Vector3f aPosition, float aRotationX, float aRotationY, GameObject* aParent);



	//this is to make the doorPart static to test the position, it should be false
	bool myTestPart = false;

	unsigned int myDoorPartFilterGroup=0;
	unsigned int myDoorPartFilterMask=0;

	unsigned int myDoorFilterGroup=0;
	unsigned int myDoorFilterMask=0;

};

