#include "Door.h"

#include "Paths.h"
#include "string"
#include "gameObject/GameObject.h"
#include "component/mesh/MeshComponent.h"
#include "graphics/model/ModelFactory.h"
#include "mainSingelton/MainSingelton.h"

#include "PhysXCode/PhysXManager.h"
#include <component/PhysicsComponent.h>
#include "DoorComponent.h"
#include "DoorPartComponent.h"


Door::Door()
{
	myDoorFilterGroup = PhysXManager::FilterGroup::eDOOR;
	myDoorFilterMask = PhysXManager::FilterGroup::ePLAYERATTACK | PhysXManager::FilterGroup::ePLAYERDESTROY;

	myDoorPartFilterGroup = PhysXManager::FilterGroup::eDOORPART;
	myDoorPartFilterMask = 0;
}

Door::~Door()
{
}

GameObject* Door::CreateDoor(Vector3f aPosition, float aRotationX, float aRotationY)
{
	ModelFactory& modelFactory = MainSingleton::GetInstance().GetModelFactory();

	std::string doorPath = RELATIVE_ASSET_ASSET_PATH "Door/sm_force_field.fbx";

	std::vector<Mesh*> doorMesh = modelFactory.Load(doorPath);

	MeshComponent* doorMeshComponent = new MeshComponent(doorMesh);

	doorMeshComponent->SetPath(doorPath);
	doorMeshComponent->SetIsHighLighted(true);
	Transform* transform = new Transform();

	transform->SetPosition(aPosition);
	transform->RotateAroundX(aRotationX);
	transform->RotateAroundY(aRotationY);
	Transform* ResetTransform = new Transform();
	ResetTransform->SetPosition(aPosition);
	ResetTransform->RotateAroundX(aRotationX);
	ResetTransform->RotateAroundY(aRotationY);
	DoorComponent* doorComponent = new DoorComponent();
	/*float xSize = 150;
	float ySize = 10;
	float zSize = 150;*/
	float xSize = 150;
	float ySize = 150;
	float zSize = 10;
	//if (aRotationX == 0)
	//{
	//	xSize = 150;
	//	ySize = 150;
	//	zSize = 10;
	//}

	physx::PxBoxGeometry boxCollision(xSize, ySize, zSize);
	PhysicsComponent* physicsComponent = new PhysicsComponent();
	physicsComponent->AddGeometry(boxCollision, true);
	physicsComponent->SetFilter(myDoorFilterGroup, myDoorFilterMask);

	GameObject* doorObject = new GameObject(false);

	doorObject->myTransform = transform;
	doorObject->myResetTransform = ResetTransform;

	doorObject->AddComponent<MeshComponent>(doorMeshComponent);
	doorObject->AddComponent<DoorComponent>(doorComponent);
	doorObject->AddComponent<PhysicsComponent>(physicsComponent);
	physicsComponent->SetCollisionFunction(std::bind(&DoorComponent::OnCollision, doorComponent, std::placeholders::_1));
	aPosition.y -= 124;
	if (aRotationX == 0)
	{
		doorObject->myChildren = CreateFracturedDoorVertical(aPosition, aRotationX, aRotationY, doorObject);
	}
	else
	{
		doorObject->myChildren = CreateFracturedDoorHorizontal(aPosition, aRotationX, aRotationY, doorObject);
	}
	doorObject->myTag = "Door";
	doorObject->myName = "Door";



	for (GameObject* child : doorObject->GetChildren())
	{
		child->myParent = doorObject;
	}


	return doorObject;
}

void Door::FixDoor(std::vector<GameObject*>& aDoorPart)
{
	for (GameObject* child : aDoorPart)
	{

		if (child->myTag=="MainDoor")
		{
			FixMainDoor(child);
		}
		else if (child->myTag=="DoorPart")
		{
			FixDoorPart(child);
		}
		else
		{
			FixFramePart(child);
		}

		

	}
}

void Door::FixMainDoor(GameObject* aGameObject)
{
	Vector3f position = aGameObject->GetPosition();
	//float yOffSet =145;
	//position.y += yOffSet;
	aGameObject->SetPosition(position);
	ModelFactory& modelFactory = MainSingleton::GetInstance().GetModelFactory();

	std::string doorPath = RELATIVE_ASSET_ASSET_PATH "Door/sm_force_field.fbx";

	std::vector<Mesh*> doorMesh = modelFactory.Load(doorPath);

	MeshComponent* doorMeshComponent = new MeshComponent(doorMesh);

	doorMeshComponent->SetPath(doorPath);
	doorMeshComponent->SetIsHighLighted(true);

	DoorComponent* doorComponent = new DoorComponent();

	float xSize = 150;
	float ySize = 150;
	float zSize = 10;

	physx::PxBoxGeometry boxCollision(xSize, ySize, zSize);
	PhysicsComponent* physicsComponent = new PhysicsComponent();
	physicsComponent->AddGeometry(boxCollision, true);
	physicsComponent->SetFilter(myDoorFilterGroup, myDoorFilterMask);

	
	aGameObject->AddComponent<MeshComponent>(doorMeshComponent);
	aGameObject->AddComponent<DoorComponent>(doorComponent);
	aGameObject->AddComponent<PhysicsComponent>(physicsComponent);
	physicsComponent->SetCollisionFunction(std::bind(&DoorComponent::OnCollision, doorComponent, std::placeholders::_1));
}

void Door::FixDoorPart(GameObject* aGameObject)
{
	PhysicsComponent* physicsComponent = new PhysicsComponent();

		physx::PxBoxGeometry boxGeo(25, 25, 15);

		physicsComponent->AddGeometry(boxGeo, myTestPart);
		physicsComponent->SetFilter(myDoorPartFilterGroup, myDoorPartFilterMask);
		physicsComponent->SetActive(false);
		aGameObject->AddComponent<PhysicsComponent>(physicsComponent);

		MeshComponent* meshComponent = aGameObject->GetComponent<MeshComponent>();

		if (!meshComponent)
			return;

		DoorPartComponent* doorPartComponent = new DoorPartComponent(meshComponent);

		aGameObject->AddComponent<DoorPartComponent>(doorPartComponent);
		aGameObject->SetActive(false);
}

void Door::FixFramePart(GameObject* aGameObject)
{
	PhysicsComponent* physicsComponent = new PhysicsComponent();

	physx::PxBoxGeometry boxGeo(25, 25, 15);

	physicsComponent->AddGeometry(boxGeo, true);
	physicsComponent->SetFilter(PhysXManager::eGROUND,0);
	physicsComponent->SetActive(true);
	aGameObject->AddComponent<PhysicsComponent>(physicsComponent);

	MeshComponent* meshComponent = aGameObject->GetComponent<MeshComponent>();

	if (!meshComponent)
		return;

	DoorPartComponent* doorPartComponent = new DoorPartComponent(meshComponent);

	aGameObject->AddComponent<DoorPartComponent>(doorPartComponent);
}



std::vector<GameObject*> Door::CreateFracturedDoorVertical(Vector3f aPosition, float aRotationX, float aRotationY, GameObject* aParent)
{

	std::vector<GameObject*>doorsChildren;

	Vector3f standarPosition = aPosition;

	float xMin = 50;
	float yMin = 25;
	standarPosition.x -= (xMin * 2) + 25;
	standarPosition.y += yMin;
	yMin += yMin;

	ModelFactory& modelFactory = MainSingleton::GetInstance().GetModelFactory();
	std::string brokenDoorPath = RELATIVE_ASSET_ASSET_PATH "Door/sm_door_broken.fbx";
	std::vector<Mesh*> brokenDoor = modelFactory.Load(brokenDoorPath);

	doorsChildren.reserve(brokenDoor.size());
	int doorSize = (int)brokenDoor.size() - 1;
	int indexName = 0;
	doorSize;
	for (size_t i = 0; i < 6; ++i)
	{
		for (size_t j = 0; j < 6; j++)
		{

			Vector3f position = standarPosition;
			position.x += xMin * j;
			position.y += yMin * i;
			Transform* transform = new Transform();
			transform->RotateAroundX(aRotationX);
			transform->RotateAroundY(aRotationY);
			transform->SetPosition(position);
			Transform* resetTransform = new Transform();
			resetTransform->RotateAroundX(aRotationX);
			resetTransform->RotateAroundY(aRotationY);
			resetTransform->SetPosition(position);
			std::vector<Mesh*> doorPart;
			//doorPart.push_back(brokenDoor[doorSize - (i + j)]);
			doorPart.push_back(brokenDoor[indexName]);
			MeshComponent* doorPartMeshComponent = new MeshComponent(doorPart);
			doorPartMeshComponent->SetPath(brokenDoorPath);
			doorPartMeshComponent->SetIsHighLighted(true);
			GameObject* child = new GameObject(false);
			child->AddComponent<MeshComponent>(doorPartMeshComponent);

			physx::PxBoxGeometry boxGeo(25, 25, 15);
			PhysicsComponent* physicsComponent = new PhysicsComponent();
			physicsComponent->AddGeometry(boxGeo, myTestPart);
			physicsComponent->SetFilter(myDoorPartFilterGroup, myDoorPartFilterMask);
			child->AddComponent<PhysicsComponent>(physicsComponent);
			DoorPartComponent* doorPartComponent = new DoorPartComponent(doorPartMeshComponent);
			child->AddComponent<DoorPartComponent>(doorPartComponent);
			child->myParent = aParent;
			child->myName = "DoorPart(" + std::to_string(indexName) + ")";
			child->myTag = "DoorPart";
			child->myTransform = transform;
			child->myResetTransform = resetTransform;
			doorsChildren.push_back(child);
			++indexName;
		}
	}
	return std::vector<GameObject*>(doorsChildren);
}

std::vector<GameObject*> Door::CreateFracturedDoorHorizontal(Vector3f aPosition, float aRotationX, float aRotationY, GameObject* aParent)
{
	std::vector<GameObject*>doorsChildren;

	Vector3f standarPosition = aPosition;

	float xMin = 50;
	float zMin = 25;
	standarPosition.x -= (xMin * 2) + 25;
	standarPosition.z -= zMin;
	zMin += zMin;

	ModelFactory& modelFactory = MainSingleton::GetInstance().GetModelFactory();
	std::string brokenDoorPath = RELATIVE_ASSET_ASSET_PATH "Door/sm_door_broken.fbx";
	std::vector<Mesh*> brokenDoor = modelFactory.Load(brokenDoorPath);
	doorsChildren.reserve(brokenDoor.size());
	for (size_t i = 0; i < 6; ++i)
	{
		for (size_t j = 0; j < 6; j++)
		{

			Vector3f position = standarPosition;
			position.x += xMin * j;
			position.z -= zMin * i;
			Transform* transform = new Transform();
			transform->RotateAroundX(aRotationX);
			transform->RotateAroundY(aRotationY);
			transform->SetPosition(position);
			Transform* resetTransform = new Transform();
			resetTransform->RotateAroundX(aRotationX);
			resetTransform->RotateAroundY(aRotationY);
			resetTransform->SetPosition(position);
			std::vector<Mesh*> doorPart;
			doorPart.push_back(brokenDoor[i + j]);
			MeshComponent* doorPartMeshComponent = new MeshComponent(doorPart);
			doorPartMeshComponent->SetPath(brokenDoorPath);
			doorPartMeshComponent->SetIsHighLighted(true);
			GameObject* child = new GameObject(false);
			child->AddComponent<MeshComponent>(doorPartMeshComponent);

			physx::PxBoxGeometry boxGeo(25, 25, 15);
			PhysicsComponent* physicsComponent = new PhysicsComponent();
			physicsComponent->AddGeometry(boxGeo, myTestPart);
			physicsComponent->SetFilter(myDoorPartFilterGroup, myDoorPartFilterMask);
			child->AddComponent<PhysicsComponent>(physicsComponent);
			DoorPartComponent* doorPartComponent = new DoorPartComponent(doorPartMeshComponent);
			child->AddComponent<DoorPartComponent>(doorPartComponent);
			child->myParent = aParent;
			child->myName = "DoorPart(" + std::to_string(i + j) + ")";
			child->myTag = "DoorPart";
			child->myTransform = transform;
			child->myResetTransform = resetTransform;
			doorsChildren.push_back(child);
		}
	}


	return std::vector<GameObject*>(doorsChildren);
}

