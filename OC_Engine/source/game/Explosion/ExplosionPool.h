#pragma once
#include <vector>
#include <PhysX/PxRigidDynamic.h>
#define Explosion_POOL_SIZE 100
class GameObject;
class ExplosionComponent;
class PhysicsComponent;

struct ExplosionReserved
{
	GameObject* myExplosion;
	ExplosionComponent* myExplosionComponent;
	PhysicsComponent* myPhysicsComponent;
	physx::PxRigidDynamic* myActor;
	bool myExplosionIsAvailable = true;
	~ExplosionReserved();
};
class ExplosionPool
{
public:
	ExplosionPool();
	~ExplosionPool();
	GameObject* GetExplosion();
	bool DoIHaveExplosion();
	void ReturnExplosion(const int aPoolIndex);

private:
	std::vector<ExplosionReserved>myExplosionPool;
};