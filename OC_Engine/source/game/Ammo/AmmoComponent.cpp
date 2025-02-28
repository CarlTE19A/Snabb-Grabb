#include "AmmoComponent.h"
#include "gameObject/GameObject.h"
#include "AmmoPool.h"
#include "Log.h"

#include "mainSingelton/MainSingelton.h"
#include "scene/SceneManager.h"
#include "scene/Scene.h"
#include "../Explosion/ExplosionPool.h"
#include "../Explosion/ExplosionComponent.h"

AmmoComponent::AmmoComponent()
{
}
AmmoComponent::~AmmoComponent()
{
}
void AmmoComponent::Update(const float aDeltaTime, Scene&)
{
	myElapsedTime += aDeltaTime;
	/*if (myActiveTime < myElapsedTime)
	{
		myPoolOwner->ReturnAmmo(myIndex);
	}*/
	Vector3f position = myParent->GetPosition() + myDirection * myBulletSpeed * aDeltaTime;

	//myTargetTransform.SetPosition(position);
	//myTargetTransform.RotateTowards(myDirection, 1000);
	//myTargetTransform.RotateTowards(position + myDirection, 1000);

	myParent->SetPosition(position);

	myParent->GetTransform()->RotateTowards(position + myDirection, 1000);
	physx::PxTransform transform(myParent->GetTransform()->GetPxMat44());
	myActor->setKinematicTarget(transform);
}

void AmmoComponent::SetPoolIndex(const int aIndex)
{
	myIndex = aIndex;
}

void AmmoComponent::SetActiveTime(const float aActiveTime)
{
	myActiveTime = aActiveTime;
}

void AmmoComponent::setSpeed(const float aSpeed)
{
	myBulletSpeed = aSpeed;
}

bool AmmoComponent::IsItExpired()
{
	if (myReturnBullet || myElapsedTime >= myActiveTime)
	{
		myElapsedTime = 0;
		return true;
	}
	return false;
}

int AmmoComponent::GetPoolIndex() const
{
	return myIndex;
}

void AmmoComponent::SetPoolOwner(AmmoPool* aOwnerPool)
{
	myPoolOwner = aOwnerPool;
}

void AmmoComponent::SetActor(physx::PxRigidDynamic* aActor)
{
	myActor = aActor;
}

AmmoPool* AmmoComponent::GetOwnerPool()
{
	return myPoolOwner;
}

void AmmoComponent::AmmoReset()
{
	myReturnBullet = false;
	myElapsedTime = 0;
	myDirection = { 0,0,0 };
}

void AmmoComponent::Shoot(const Vector3f aPosition, const Vector3f aDirection, bool doSpawnExplosion, float explosionTime, float explosionMaxSize)
{
	mySpawnExplosion = doSpawnExplosion;
	myExplosionTime = explosionTime;
	myExplosionMaxSize = explosionMaxSize;

	myElapsedTime = 0;
	Transform transform;
	myDirection = aDirection.GetNormalized();
	transform.SetPosition(aPosition);
	transform.RotateTowards(aPosition + aDirection, 1000);
	myParent->SetPosition(aPosition);
	physx::PxTransform pxTransform = physx::PxTransform(transform.GetPxMat44());
	myActor->setGlobalPose(pxTransform);

	//const int maxHits = 5;
	//physx::PxSweepHit hit;
	//SweepCallback sweepCallback(hits, maxHits);
	//float distance = 1000;
	/*
	PhysXManager& physxManager = MainSingleton::GetInstance().GetPhysXManager();
	if (physxManager.Sweep(
		standardBulletGeo,
		pxTransform,
		aDirection,
		distance,
		hit,
		PhysXManager::FilterGroup::eGROUND | PhysXManager::FilterGroup::eBALLONENEMY | PhysXManager::FilterGroup::eGROUNDENEMY))
	{
		float distance = hit.distance;
		float timeToDeath = distance / myBulletSpeed;
		//Log("HIT SOMETHING :)");
		LogInfo("Hit: " + std::to_string(hit.distance) + ", " + std::to_string(timeToDeath));
		//if(hit.actor->getActorFlags() |= PhysXManager::FilterGroup::eBALLONENEMY)
	}
	*/
	//myActor->addForce({ aDirection.x * aSpeed,aDirection.y * aSpeed,aDirection.z * aSpeed }, physx::PxForceMode::eIMPULSE);
}

void AmmoComponent::OnCollision(physx::PxShape* aShape)
{
	if (mySpawnExplosion)
	{
		MainSingleton& mainSingleton = MainSingleton::GetInstance();
		GameObject* explosion = mainSingleton.GetExplosionPool().GetExplosion();
		mainSingleton.GetSceneManager().GetActiveScene()->AddExplosionObject(explosion);
		explosion->GetComponent<ExplosionComponent>()->Spawn(myParent->GetPosition(), myExplosionTime, myExplosionMaxSize);
	}
	//physx::PxFilterData filterData = aShape->getQueryFilterData();
	//myPoolOwner->ReturnAmmo(myIndex);
	myReturnBullet = true;
	aShape;
}

void AmmoComponent::EditBullet(const float aActiveTime, const float aBulletSpeed)
{
	myActiveTime = aActiveTime;
	myBulletSpeed = aBulletSpeed;
}

void AmmoComponent::ImguiUpdate()
{
}

void AmmoComponent::Reset()
{
	myPoolOwner->ReturnAmmo(myIndex);
}
