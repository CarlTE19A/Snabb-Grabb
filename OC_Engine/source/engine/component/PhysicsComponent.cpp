#include "PhysicsComponent.h"
#include "stdafx.h"
#include "PhysicsComponent.h"

#include "mainSingelton\MainSingelton.h"
#include "PhysXCode\PhysXManager.h"
#include "component\mesh\MeshComponent.h"
#include "gameObject\GameObject.h"
#include "math/Transform.h"

#include "PhysX\foundation/Px.h"
#include "PhysX\foundation/PxMat44.h"

static const physx::PxShapeFlags standardShapeFlags = physx::PxShapeFlag::eVISUALIZATION | physx::PxShapeFlag::eSIMULATION_SHAPE | physx::PxShapeFlag::eSCENE_QUERY_SHAPE;

PhysicsComponent::PhysicsComponent()
{
	myMaterial = MainSingleton::GetInstance().GetPhysXManager().GetDefaultMaterial();
	//myStaticActor = nullptr;
	myActor = nullptr;
	myShape = nullptr;
}

PhysicsComponent::~PhysicsComponent()
{}

void PhysicsComponent::Update(const float aDeltaTime, Scene& aScene)
{

	if (myActor == nullptr)
		return;

	aDeltaTime;
	aScene;
	physx::PxMat44 mat;


	if (myActor->is<physx::PxRigidStatic>())
	{
		mat = physx::PxMat44(myActor->is<physx::PxRigidStatic>()->getGlobalPose());

	}
	else if (myActor->is<physx::PxRigidDynamic>())
	{
		mat = physx::PxMat44(myActor->is<physx::PxRigidDynamic>()->getGlobalPose());
	}
	//mat.scale(physx::PxVec4(1.f, scale.x * 1.01f, scale.y * 1.01f, scale.z * 1.01f));
	Transform* transform = myParent->GetTransform();
	Vector3f scale = myParent->GetTransform()->GetScale();
	mat.scale(physx::PxVec4(1.f, scale.x, scale.y, scale.z));
	transform->SetMatrixFromPxMat44(mat);
}

bool PhysicsComponent::AddGeometry(physx::PxGeometry& aGeometry, bool isStatic, bool isUnique)
{
	PhysXManager& physXManager = MainSingleton::GetInstance().GetPhysXManager();

	//Remove possible old geometry
	RemoveGeometry();
	//Setup values
	myShape = physXManager.GetPhysics()->createShape(aGeometry, *myMaterial, isUnique, standardShapeFlags);
	physx::PxTransform transform = physx::PxTransform(physx::PxTransform(physx::PxIdentity));
	if (isStatic)
	{
		myActor = physx::PxCreateStatic(*physXManager.GetPhysics(), transform, *myShape);

	}
	else
	{
		physx::PxReal density = 15.0f;
		//myActor = physx::PxCreateDynamic(*physXManager.GetPhysics(), transform, *myShape, density);
		auto* test = physx::PxCreateDynamic(*physXManager.GetPhysics(), transform, *myShape, density);
		myActor = test;
	}
	if (myActor == nullptr)
	{
		return false;
	}
	myActor->userData = this;
	myShape->userData = this;
	//.AddActor(*myActor);
	SetActive(true);
	return true;
}

bool PhysicsComponent::RemoveGeometry()
{
	if (myActor == nullptr)
	{
		//LogError("PhysicsComponent::RemoveGeometry: myActor is nullptr");
		return false;
	}

	PhysXManager& physXManager = MainSingleton::GetInstance().GetPhysXManager();
	if (myActor->getScene())
	{
		//myActor->getScene()->removeActor(*myActor);
		physXManager.RemoveActor(*myActor);
	}
	myActor->release();
	myActor = nullptr;
	myShape->release();
	myShape = nullptr;
	return true;
}

bool PhysicsComponent::SetFilter(unsigned int aFilterGroup, unsigned int aFilterMask)
{
	if (myShape == nullptr)
	{
		LogError("PhysicsComponent::SetFilter: myShape is nullptr");
		return false;
	}

	PhysXManager& physXManager = MainSingleton::GetInstance().GetPhysXManager();
	physXManager.SetFilterOfShape(myShape, aFilterGroup, aFilterMask);
	return true;
}

void PhysicsComponent::ImguiUpdate()
{
	bool isStatic = false;
	if (myActor != nullptr)
	{
		if (myActor->is<physx::PxRigidStatic>())
		{
			myActor->is<physx::PxRigidStatic>()->setGlobalPose(physx::PxTransform(myParent->GetTransform()->GetPxMat44()));
			isStatic = true;
		}
		else if (myActor->is<physx::PxRigidDynamic>())
		{
			myActor->is<physx::PxRigidDynamic>()->setGlobalPose(physx::PxTransform(myParent->GetTransform()->GetPxMat44()));
			isStatic = false;
		}
	}
	if (ImGui::BeginTabItem("PhysicsComponent"))
	{
		bool active = IsActive();
		if (ImGui::Checkbox("Active", &active))
		{
			SetActive(active);
		}
		ImGui::BeginDisabled();
		ImGui::Checkbox("Static", &isStatic);
		if(myShape != nullptr)
		{
			int filterValue = myShape->getSimulationFilterData().word0;
			ImGui::InputInt("Filter Group:", &filterValue);
			filterValue = myShape->getSimulationFilterData().word1;
			ImGui::InputInt("Filter Mask:", &filterValue);
		}
		ImGui::EndDisabled();
		
		ImGui::EndTabItem();
	}
}

void PhysicsComponent::SetObjectsPosition()
{
	if (myActor)
	{
		if (myActor->is<physx::PxRigidStatic>())
		{
			myActor->is<physx::PxRigidStatic>()->setGlobalPose(physx::PxTransform(myParent->GetTransform()->GetPxMat44()));
		}
		else if (myActor->is<physx::PxRigidDynamic>())
		{
			myActor->is<physx::PxRigidDynamic>()->setGlobalPose(physx::PxTransform(myParent->GetTransform()->GetPxMat44()));
		}
	}
	else
	{
		LogError("Can't set position of object (NO static or dynamic actor)");
	}
}

void PhysicsComponent::Reset()
{
	SetActive(true);
}

void PhysicsComponent::SetActive(const bool& aActive)
{
	if (myActor == nullptr) 
		return;
	bool hasScene = myActor->getScene() != nullptr;
	if (aActive && !hasScene)
	{
		MainSingleton::GetInstance().GetPhysXManager().AddActor(*myActor);
	}
	else if (!aActive && hasScene)
	{
		MainSingleton::GetInstance().GetPhysXManager().RemoveActor(*myActor);
	}
}

void PhysicsComponent::SetMaterial(physx::PxMaterial* aMaterial)
{
	myMaterial = aMaterial;
	myShape->setMaterials(&myMaterial, 1);
}

void PhysicsComponent::SetSimulationDoPair(bool aDoPair)
{
	myShape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, aDoPair);
}
