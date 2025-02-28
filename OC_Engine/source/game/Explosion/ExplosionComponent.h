#pragma once
#include "component/Component.h"
#include "math/Vector.h"
#include "math/Transform.h"
#include <PhysX/PxRigidDynamic.h>
class ExplosionPool;
class PhysicsComponent;
class ExplosionComponent :public Component
{
public:

	ExplosionComponent();
	~ExplosionComponent();

	virtual void Update(const float aDeltaTime, Scene&)override;
	//void SetPosition(const Vector3f aPosition);
	//void SetDirection(const Vector3f aDirection);
	void SetPoolIndex(const int aIndex);
	bool IsItExpired();
	int GetPoolIndex() const;

	void SetPoolOwner(ExplosionPool* aOwnerPool);
	void SetActor(physx::PxRigidDynamic* aActor);
	ExplosionPool* GetOwnerPool();
	void ExplosionReset();

	void Spawn(const Vector3f aPosition, const float aTime, const float aMaxRadius);
	void OnCollision(physx::PxShape* aShape);

	inline physx::PxSphereGeometry& GetGeometry()const { return *myGeometry; }

private:
	//PhysicsComponent* myPhysicsComponent = nullptr;
	ExplosionPool* myPoolOwner=nullptr;
	physx::PxRigidDynamic* myActor = nullptr;
	physx::PxSphereGeometry* myGeometry;

	float myElapsedTime;
	float myActiveTime;
	float myMaxSize=0;
	int myIndex=0;

	bool myReturnExplosion = false;


	// Inherited via Component
	void ImguiUpdate() override;
	void Reset() override;

};
