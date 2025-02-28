#pragma once
#include "component/Component.h"
#include "math/Vector.h"
#include "math/Transform.h"
#include <PhysX/PxRigidDynamic.h>
class AmmoPool;
class AmmoComponent :public Component
{
public:

	AmmoComponent();
	~AmmoComponent();

	void Update(const float aDeltaTime, Scene&)override;
	//void SetPosition(const Vector3f aPosition);
	//void SetDirection(const Vector3f aDirection);
	void SetPoolIndex(const int aIndex);
	void SetActiveTime(const float aActiveTime);
	void setSpeed(const float aSpeed);
	bool IsItExpired();
	int GetPoolIndex() const;

	void SetPoolOwner(AmmoPool* aOwnerPool);
	void SetActor(physx::PxRigidDynamic* aActor);
	AmmoPool* GetOwnerPool();
	void AmmoReset();

	void Shoot(const Vector3f aPosition, const Vector3f aDirection, bool doSpawnExplosion = false, float explosionTime = 1.f, float explosionMaxSize = 100.f);
	void OnCollision(physx::PxShape* aShape);

	void EditBullet(const float aActiveTime, const float aBulletSpeed);

private:

	//Vector3f myPosition{};
	Vector3f myDirection{};
	AmmoPool* myPoolOwner=nullptr;
	//Transform myTargetTransform;
	physx::PxRigidDynamic* myActor = nullptr;

	float myElapsedTime;
	float myActiveTime;
	float myBulletSpeed=0;
	int myIndex=0;


	bool myReturnBullet = false;
	bool mySpawnExplosion = false;
	float myExplosionTime = 1.f;
	float myExplosionMaxSize = 100.f;



	// Inherited via Component
	void ImguiUpdate() override;

	void Reset() override;
};