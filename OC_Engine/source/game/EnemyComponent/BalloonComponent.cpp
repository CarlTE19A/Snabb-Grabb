#include "BalloonComponent.h"
#include "../BehaviorTree/BehaviorNode.h"
#include "component/mesh/AnimatedMeshComponent.h"
#include <iostream>
#include "gameObject/GameObject.h"
#include "../game/EnemyAnimation/AnimationHandler.h"
#include "../game/EnemyAnimation/AnimationHandler.cpp"
#include "../game/EnemyAnimation/AnimationType.h"
#include "component/PhysicsComponent.h"
#include "PhysXCode/PhysXManager.h"
#include <component/mesh/MeshComponent.h>
#include <Windows.h>
#include "scene/Scene.h"
#include "sound/SoundManager.h"
BalloonComponent::BalloonComponent(PollingStation* aPollingStation)
	: EnemyComponent(aPollingStation)
	, myAnimationHandler(nullptr)
{

}

BalloonComponent::~BalloonComponent()
{
	delete myBehaviorNode;
	myBehaviorNode = nullptr;

	delete myAnimationHandler;
}

void BalloonComponent::Init()
{
	myAnimationHandler = new AnimationHandler<BalloonAnimation>(myParent->GetComponent<AnimatedMeshComponent>());
	myAnimationHandler->AddDontChangrToIdleAnimation(BalloonAnimation::eDeath);

}

void BalloonComponent::Update(const float aDeltaTime, Scene& aScene)
{
	EnemyComponent::Update(aDeltaTime, aScene);
	if (myBehaviorNode&&myIsAlive)
	{
		myBehaviorNode->Execute(aDeltaTime, aScene);
	}

	myAnimationHandler->Update();

	/*if (GetAsyncKeyState('I'))
	{
		myAnimationHandler->ChangeAnimationType(BalloonAnimation::eDeath);
	}*/

	if (myAnimationHandler->DeathAnimationFinish())
	{
		this->GetOwner()->SetActive(false);
		SetActive(false);
		myPhysicsComponent->SetActive(false);
	}

	/*if (GetAsyncKeyState('I'))
	{
		myHealthPoint=0;
	}*/

	CheckIfDied(aScene);
}


void BalloonComponent::ImguiUpdate()
{
}

void BalloonComponent::Reset()
{
	EnemyComponent::Reset();
	this->GetOwner()->SetActive(true);
	SetActive(true);
	myPhysicsComponent->SetActive(true);
	myParent->GetComponent<AnimatedMeshComponent>()->SetActive(true);
	myHealthPoint = myMaxHealth;
	myIsAlive = true;
	ChangeAnimation(BalloonAnimation::eIdle);
	myAnimationHandler->AnimationReset();
	
}

void BalloonComponent::CheckIfDied(Scene& aScene)
{
	if (!myIsAlive)
		return;
	if (myHealthPoint<=0)
	{
		ChangeAnimation(BalloonAnimation::eDeath);
		aScene.GetSoundManager()->EditParameter(3, "AmbienceSound", 2);
		aScene.GetSoundManager()->UpdatePosition(3, GetPosition());
		aScene.GetSoundManager()->PlayEvent(3);
		myIsAlive = false;
		myPhysicsComponent->SetActive(false);
		AddCardToScene(aScene);
	}
}

void BalloonComponent::ChangeAnimation(BalloonAnimation aAnimationType)
{
	myAnimationHandler->ChangeAnimationType(aAnimationType);
}

void BalloonComponent::OnCollision(physx::PxShape* aShape)
{
	physx::PxFilterData filterData = aShape->getQueryFilterData();
	//myParent->GetComponent<AnimatedMeshComponent>()->SetActive(false);

	if (filterData.word0 & PhysXManager::FilterGroup::ePLAYER)
	{
		TakeDamage(1000);
	}
	if (filterData.word0 & PhysXManager::FilterGroup::ePLAYERATTACK)
	{
		float damageToTake = 0;
		try { damageToTake = static_cast<PhysicsComponent*>(aShape->userData)->myValue; }
		catch (const std::exception& e) { std::cout << e.what() << std::endl; }
		
		TakeDamage(damageToTake);
	}
}
