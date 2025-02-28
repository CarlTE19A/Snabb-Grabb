#pragma once
#include <vector>
#include <PhysX/PxRigidDynamic.h>
#define AMMO_POOL_SIZE 100
class GameObject;
class AmmoComponent;
class PhysicsComponent;
class ParticleComponent;

struct AmmoReserved
{
	GameObject* myAmmo;
	AmmoComponent* myAmmoComponent;
	PhysicsComponent* myPhysicsComponent;
	ParticleComponent* myTrailEffect;
	ParticleComponent* myHitParticleEffect;
	physx::PxRigidDynamic* myActor;
	bool myAmmoIsAvailable = true;
	~AmmoReserved();
};
class AmmoPool
{
public:
	enum AmmoType
	{
		EnemyBullet,
		PlayerBullet
	};


	AmmoPool();
	~AmmoPool();
	GameObject* GetAmmo(AmmoType aAmmoType);
	bool DoIHaveAmmo();
	void ReturnAmmo(const int aPoolIndex);

	float GetBulletSpeed();
	float GetActiveTime();
	float GetAmmoRadius();

	void SetBulletSpeed(const float aBulletSpeed);

	void SetActiveTime(const float aActiveTime);

	void SetRadius(const float aRadius);

	void EditBullet();

	void ChangeRadius(const int aIndex);

	void ReadJson();

	void SaveData();

	void LoadData();

	std::vector<AmmoReserved>& GetAmmoPool() { return myAmmoPool; }

private:

	float myActiveTime = 0;
	float myBulletSpeed = 0;
	float myRadius = 5.0f;

	std::vector<AmmoReserved>myAmmoPool;
};