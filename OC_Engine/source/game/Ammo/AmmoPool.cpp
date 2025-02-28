#include "AmmoPool.h"
#include "AmmoComponent.h"
#include "gameObject/GameObject.h"
#include "graphics/model/ModelFactory.h"
#include "mainSingelton/MainSingelton.h"
#include "Paths.h"
#include "component/mesh/AnimatedMeshComponent.h"
#include "PhysXCode/PhysXManager.h"
#include "component/PhysicsComponent.h"
#include "math/Transform.h"

#include "scene/SceneManager.h"
#include "scene/Scene.h"

#include "Paths.h"
#include "Log.h"
#include "nlohmann/json.hpp"
#include <fstream>
#include "component\Particle\ParticleComponent.h"

AmmoPool::AmmoPool()
{
	myAmmoPool.resize(AMMO_POOL_SIZE);
	ModelFactory& modelFactory = MainSingleton::GetInstance().GetModelFactory();
	std::string bulletPath = "Assets/Bullet/sm_bullet.fbx";
	//std::string balloonPath = RELATIVE_ASSET_ASSET_PATH "Balloon_Enemy/SK_balloon.fbx";

	ReadJson();

	for (int i = 0; i < AMMO_POOL_SIZE; ++i)
	{
		std::vector<Mesh*> bulletMesh = modelFactory.Load(bulletPath);
		MeshComponent* bulletMeshComponent = new MeshComponent(bulletMesh);
		bulletMeshComponent->SetPath(bulletPath);
		//balloonMeshComponent->SetIsHighLighted(true);

		GameObject* gameObject = new GameObject(false);
		AmmoComponent* ammo = new AmmoComponent();
		ammo->EditBullet(myActiveTime, myBulletSpeed);
		PhysicsComponent* collisionComponent = new PhysicsComponent();

		physx::PxSphereGeometry sphereGeometry(myRadius);
		collisionComponent->AddGeometry(sphereGeometry, false);
		collisionComponent->SetSimulationDoPair(false);
		collisionComponent->myValue = 100;
		//Filters are set in GetAmmo

		physx::PxRigidDynamic* actor = collisionComponent->GetActor()->is<physx::PxRigidDynamic>();
		//actor->setActorFlag(physx::PxActorFlag::eDISABLE_SIMULATION, true);
		actor->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, true);
		actor->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, true);
		//actor->setRigidBodyFlag(physx::PxRigidBodyFlag::eENABLE_CCD, true);
		if (actor == nullptr)
		{
			assert(false && "AmmoPool:: actor is not dynamic, something was most likely chanhed in physicsComponent.cpp");
		}
		actor->setMaxAngularVelocity(0);

		myAmmoPool[i].myAmmo = gameObject;
		myAmmoPool[i].myAmmoComponent = ammo;
		myAmmoPool[i].myPhysicsComponent = collisionComponent;
		myAmmoPool[i].myActor = actor;

		myAmmoPool[i].myAmmo->myResetTransform = new Transform();
		myAmmoPool[i].myAmmo->AddComponent<AmmoComponent>(myAmmoPool[i].myAmmoComponent);
		myAmmoPool[i].myAmmo->AddComponent<PhysicsComponent>(myAmmoPool[i].myPhysicsComponent);
		myAmmoPool[i].myAmmo->AddComponent<MeshComponent>(bulletMeshComponent);

 		ParticleComponent* particleTrailEffect = new ParticleComponent();
		particleTrailEffect->Init(1);
		particleTrailEffect->GetParticleSystem().LoadParticleSystemSettings("BulletParticle.json");

		ParticleComponent* particleHitEffect = new ParticleComponent();
		particleHitEffect->Init(1);

		particleHitEffect->GetParticleSystem().LoadParticleSystemSettings("BulletHit.json");

		myAmmoPool[i].myTrailEffect = particleTrailEffect;
		myAmmoPool[i].myHitParticleEffect = particleHitEffect;

		myAmmoPool[i].myAmmo->AddComponent<ParticleComponent>(particleHitEffect);
		myAmmoPool[i].myAmmo->AddComponent<ParticleComponent>(particleTrailEffect);

		myAmmoPool[i].myAmmoComponent->SetActor(myAmmoPool[i].myActor);
		myAmmoPool[i].myAmmoComponent->SetPoolIndex(i);
		myAmmoPool[i].myAmmoComponent->SetPoolOwner(this);
		std::string name = "Ammo(" + std::to_string(i) + ")";
		myAmmoPool[i].myAmmo->myName = name;
		myAmmoPool[i].myAmmo->myTag = "Ammo";

		myAmmoPool[i].myPhysicsComponent->SetCollisionFunction(std::bind(&AmmoComponent::OnCollision, myAmmoPool[i].myAmmoComponent, std::placeholders::_1));
	}
}

AmmoPool::~AmmoPool()
{
	myAmmoPool.clear();
}

GameObject* AmmoPool::GetAmmo(AmmoType aAmmoType)
{
	for (size_t i = 0; i < AMMO_POOL_SIZE; i++)
	{
		if (myAmmoPool[i].myAmmoIsAvailable)
		{
			myAmmoPool[i].myAmmoIsAvailable = false;
			myAmmoPool[i].myPhysicsComponent->SetSimulationDoPair(true);
			myAmmoPool[i].myPhysicsComponent->SetActive(true);
			myAmmoPool[i].myAmmoComponent->AmmoReset();
			switch (aAmmoType)
			{
			case AmmoPool::EnemyBullet:
				myAmmoPool[i].myPhysicsComponent->SetFilter(PhysXManager::FilterGroup::eENEMYBULLET,
					PhysXManager::FilterGroup::ePLAYER |
					PhysXManager::FilterGroup::eGROUND);
				break;
			case AmmoPool::PlayerBullet:
				myAmmoPool[i].myPhysicsComponent->SetFilter(PhysXManager::FilterGroup::ePLAYERATTACK,
					PhysXManager::FilterGroup::eGROUNDENEMY |
					PhysXManager::FilterGroup::eBALLONENEMY |
					PhysXManager::FilterGroup::eGROUND |
					PhysXManager::FilterGroup::eDOOR
				);
				break;
			default:
				break;
			}
			myAmmoPool[i].myAmmo->GetComponent<ParticleComponent>()->GetParticleSystem().UpdateBuffers();
			return myAmmoPool[i].myAmmo;
		}
	}
	LogError("AmmoPool::GetAmmo: No ammo available");
	return nullptr;
}

bool AmmoPool::DoIHaveAmmo()
{
	for (size_t i = 0; i < AMMO_POOL_SIZE; i++)
	{
		if (myAmmoPool[i].myAmmoIsAvailable)
		{
			return true;
		}
	}
	return false;
}

void AmmoPool::ReturnAmmo(const int aPoolIndex)
{
	myAmmoPool[aPoolIndex].myAmmoIsAvailable = true;
	myAmmoPool[aPoolIndex].myAmmo->SetActive(false);
	myAmmoPool[aPoolIndex].myAmmoComponent->EditBullet(myActiveTime, myBulletSpeed);
	//myAmmoPool[aPoolIndex].myActor->setActorFlag(physx::PxActorFlag::eDISABLE_SIMULATION, true);
	myAmmoPool[aPoolIndex].myAmmo->myTransform->SetPosition({ 100000,100000,100000 });
	myAmmoPool[aPoolIndex].myPhysicsComponent->SetSimulationDoPair(false);
	//Remove from scene
	/*Scene* scene = MainSingleton::GetInstance().GetSceneManager().GetActiveScene();
	scene->RemoveGameObject(myAmmoPool[aPoolIndex].myAmmo);*/
}

float AmmoPool::GetBulletSpeed()
{
	return myBulletSpeed;
}

float AmmoPool::GetActiveTime()
{
	return myActiveTime;
}

float AmmoPool::GetAmmoRadius()
{
	return myRadius;
}

void AmmoPool::SetBulletSpeed(const float aBulletSpeed)
{
	myBulletSpeed = aBulletSpeed;
}

void AmmoPool::SetActiveTime(const float aActiveTime)
{
	myActiveTime = aActiveTime;
}

void AmmoPool::SetRadius(const float aRadius)
{
	myRadius = aRadius;
}

void AmmoPool::EditBullet()
{
	for (int i = 0; i < myAmmoPool.size(); ++i)
	{
		auto& ammoComponent = myAmmoPool[i].myAmmoComponent;
		ChangeRadius(i);
		ammoComponent->EditBullet(myActiveTime, myBulletSpeed);
	}
}

void AmmoPool::ChangeRadius(const int aIndex)
{
	aIndex;
	/*auto& collisionComponent = myAmmoPool[aIndex].myPhysicsComponent;

	physx::PxSphereGeometry sphereGeometry(myRadius);
	collisionComponent->AddGeometry(sphereGeometry, false);
	collisionComponent->SetFilter(PhysXManager::FilterGroup::eENEMYBULLET, 0);
	collisionComponent->SetSimulationDoPair(false);

	myAmmoPool[aIndex].myActor = collisionComponent->GetActor()->is<physx::PxRigidDynamic>();

	myAmmoPool[aIndex].myPhysicsComponent->SetCollisionFunction(std::bind(&AmmoComponent::OnCollision, myAmmoPool[aIndex].myAmmoComponent, std::placeholders::_1));*/

}
void AmmoPool::ReadJson()
{
	std::ifstream readJson;
	readJson.open(STATIC_ASSET_PATH "json/AmmoSettings.json");
	if (readJson.is_open())
	{
		nlohmann::json json;
		readJson >> json;
		myActiveTime = json["ActiveTime"].get<float>();
		myBulletSpeed = json["BulletSpeed"].get<float>();
		myRadius = json["Radius"].get<float>();
	}
}

void AmmoPool::SaveData()
{

	nlohmann::json json;

	json["ActiveTime"] = myActiveTime;
	json["BulletSpeed"] = myBulletSpeed;
	json["Radius"] = myRadius;
	std::ofstream file;

	file.open(STATIC_ASSET_PATH "json/AmmoSettings.json");

	if (file.is_open())
	{
		file << json.dump(3);
	}
	file.close();

	EditBullet();

}

void AmmoPool::LoadData()
{
	ReadJson();
	EditBullet();
}

AmmoReserved::~AmmoReserved()
{
	delete myAmmo;
	myAmmo = nullptr;
}
