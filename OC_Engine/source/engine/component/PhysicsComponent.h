#pragma once
#include "Component.h"
#include "PhysX\PxRigidDynamic.h"
#include "PhysXCode/PhysicsFunctionCalls.h"
class MeshComponent;
namespace physx
{
	class PxConvexMesh;
}

class PhysicsComponent : public Component, public PhysicsFunctionCalls
{
public:
	PhysicsComponent();
	~PhysicsComponent() override;

	void Update(const float aDeltaTime, Scene& aScene) override;

	bool AddGeometry(physx::PxGeometry& aGeometry, bool isStatic, bool isUnique = true);
	bool RemoveGeometry();

	bool SetFilter(unsigned int aFilterGroup, unsigned int aFilterMask);

	void ImguiUpdate() override;

	void SetObjectsPosition();

	void Reset() override;

	void SetActive(const bool& aActive) override;

	inline physx::PxActor* GetActor() { return myActor; }
	inline physx::PxShape* GetShape() { return myShape; }
	inline physx::PxMaterial* GetMaterial() { return myMaterial; }

	void SetMaterial(physx::PxMaterial* aMaterial);
	void SetSimulationDoPair(bool aDoPair);

private:
	physx::PxActor* myActor = nullptr;
	physx::PxShape* myShape = nullptr;
	physx::PxMaterial* myMaterial = nullptr; 
};


//!TODO RESET physics->SetActive(false);