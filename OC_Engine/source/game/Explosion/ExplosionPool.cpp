#include "ExplosionPool.h"
#include "ExplosionComponent.h"
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

ExplosionPool::ExplosionPool()
{
	myExplosionPool.resize(Explosion_POOL_SIZE);
	ModelFactory& modelFactory = MainSingleton::GetInstance().GetModelFactory();
	//std::string balloonPath = RELATIVE_ASSET_ASSET_PATH "Balloon_Enemy/SK_balloon.fbx";
	std::string vfxPath = "Assets/Explosion/sm_explosion.fbx";

	for (int i = 0; i < Explosion_POOL_SIZE; ++i)
	{
		std::vector<Mesh*> vfxMesh = modelFactory.Load(vfxPath);
		MeshComponent* vfxMeshComponent = new MeshComponent(vfxMesh);
		vfxMeshComponent->SetPath(vfxPath);
		//balloonMeshComponent->SetIsHighLighted(true);

		GameObject* gameObject = new GameObject(false);
		ExplosionComponent* explosion = new ExplosionComponent();
		PhysicsComponent* collisionComponent = new PhysicsComponent();

		collisionComponent->AddGeometry(explosion->GetGeometry(), false);
		collisionComponent->SetSimulationDoPair(false);
		collisionComponent->myValue = 1000;
		//Filters are set in GetExplosion

		physx::PxRigidDynamic* actor = collisionComponent->GetActor()->is<physx::PxRigidDynamic>();
		//actor->setActorFlag(physx::PxActorFlag::eDISABLE_SIMULATION, true);
		actor->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, true);
		actor->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, true);
		if (actor == nullptr)
		{
			assert(false && "ExplosionPool:: actor is not dynamic, something was most likely chanhed in physicsComponent.cpp");
		}
		actor->setMaxAngularVelocity(0);

		myExplosionPool[i].myExplosion = gameObject;
		myExplosionPool[i].myExplosionComponent = explosion;
		myExplosionPool[i].myPhysicsComponent = collisionComponent;
		myExplosionPool[i].myActor = actor;

		myExplosionPool[i].myExplosion->AddComponent<ExplosionComponent>(myExplosionPool[i].myExplosionComponent);
		myExplosionPool[i].myExplosion->AddComponent<PhysicsComponent>(myExplosionPool[i].myPhysicsComponent);
		myExplosionPool[i].myExplosion->AddComponent<MeshComponent>(vfxMeshComponent);

		//ParticleComponent* particleComponent = new ParticleComponent();
		//particleComponent->Init(10000);
		//myExplosionPool[i].myExplosion->AddComponent<ParticleComponent>(particleComponent);


		myExplosionPool[i].myExplosionComponent->SetActor(myExplosionPool[i].myActor);
		myExplosionPool[i].myExplosionComponent->SetPoolIndex(i);
		myExplosionPool[i].myExplosionComponent->SetPoolOwner(this);
		std::string name = "Explosion(" + std::to_string(i) + ")";
		myExplosionPool[i].myExplosion->myName = name;
		myExplosionPool[i].myExplosion->myTag = "Explosion";

		myExplosionPool[i].myPhysicsComponent->SetCollisionFunction(std::bind(&ExplosionComponent::OnCollision, myExplosionPool[i].myExplosionComponent, std::placeholders::_1));
	}
}

ExplosionPool::~ExplosionPool()
{
	myExplosionPool.clear();
}

GameObject* ExplosionPool::GetExplosion()
{
	for (size_t i = 0; i < Explosion_POOL_SIZE; i++)
	{
		if (myExplosionPool[i].myExplosionIsAvailable)
		{
			myExplosionPool[i].myExplosionIsAvailable = false;
			myExplosionPool[i].myExplosion->SetActive(true);
			myExplosionPool[i].myPhysicsComponent->SetSimulationDoPair(true);
			myExplosionPool[i].myPhysicsComponent->SetActive(true);
			myExplosionPool[i].myPhysicsComponent->SetFilter(PhysXManager::FilterGroup::eBOMB, 0);

			//myExplosionPool[i].myExplosion->GetComponent<ParticleComponent>()->GetParticleSystem().UpdateBuffers();
			return myExplosionPool[i].myExplosion;
		}
	}
	LogError("ExplosionPool::GetExplosion: No Explosion available");
	return nullptr;
}

bool ExplosionPool::DoIHaveExplosion()
{
	for (size_t i = 0; i < Explosion_POOL_SIZE; i++)
	{
		if (myExplosionPool[i].myExplosionIsAvailable)
		{
			return true;
		}
	}
	return false;
}

void ExplosionPool::ReturnExplosion(const int aPoolIndex)
{
	myExplosionPool[aPoolIndex].myExplosionIsAvailable = true;
	myExplosionPool[aPoolIndex].myExplosion->SetActive(false);
	
	myExplosionPool[aPoolIndex].myExplosion->GetTransform()->SetPosition({100000,100000,100000});
	myExplosionPool[aPoolIndex].myPhysicsComponent->SetSimulationDoPair(false);
	//Remove from scene
	//Scene* scene = MainSingleton::GetInstance().GetSceneManager().GetActiveScene();
	//scene->RemoveGameObject(myExplosionPool[aPoolIndex].myExplosion);
}
ExplosionReserved::~ExplosionReserved()
{
	delete myExplosion;
	myExplosion = nullptr;
}
