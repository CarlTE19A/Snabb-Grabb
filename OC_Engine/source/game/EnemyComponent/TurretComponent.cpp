#include "TurretComponent.h"
#include "../BehaviorTree/BehaviorNode.h"

#include "gameObject/GameObject.h"
#include "../game/EnemyAnimation/AnimationHandler.h"
#include "../game/EnemyAnimation/AnimationHandler.cpp"
#include "../game/EnemyAnimation/AnimationType.h"
#include <iostream>

#include "nlohmann/json.hpp"
#include <fstream>
#include "Paths.h"

#ifdef _DEBUG

#include "mainSingelton/MainSingelton.h"
#include "../game/EnemyComponent/EnemyPool.h"
#endif // _DEBUG

#include "PhysXCode/PhysXManager.h"
#include "component/PhysicsComponent.h"
#include "scene/Scene.h"
#include "scene/Scene.h"
#include "sound/SoundManager.h"
TurretComponent::TurretComponent(PollingStation* aPollingStation)
	:EnemyComponent(aPollingStation)
	, myAnimationHandler(nullptr)
{
}

TurretComponent::~TurretComponent()
{
	delete myBehaviorNode;
	myBehaviorNode = nullptr;

	delete myAnimationHandler;
}

void TurretComponent::Init()
{
	myAnimationHandler = new AnimationHandler<TurretAnimation>(myParent->GetComponent<AnimatedMeshComponent>());
	myAnimationHandler->AddSpecialAnimation(TurretAnimation::eDeath);
	myAnimationHandler->AddDontChangrToIdleAnimation(TurretAnimation::eDeath);
	/*myAnimationHandler->AddDontChangrToIdleAnimation(TurretAnimation::eDeath);
	myAnimationHandler->AddDontChangrToIdleAnimation(TurretAnimation::eWindup);*/
}

void TurretComponent::Update(const float aDeltaTime, Scene& aScene)
{
	EnemyComponent::Update(aDeltaTime, aScene);
	if (myIsAlive)
	{
		myBehaviorNode->Execute(aDeltaTime, aScene);
	}
	if (myAttackTimer >= 0)
	{
		myAttackTimer -= aDeltaTime;
	}

	
	myAnimationHandler->Update();


	/*if (GetAsyncKeyState('P'))
	{
		myAnimationHandler->ChangeAnimationType(TurretAnimation::eIdle);
	}
	else if (GetAsyncKeyState('O'))
	{
		myAnimationHandler->ChangeAnimationType(TurretAnimation::eShoot);
	}
	else if (GetAsyncKeyState('I'))
	{
		myAnimationHandler->ChangeAnimationType(TurretAnimation::eDeath);
	}*/

	

	if (myAnimationHandler->DeathAnimationFinish())
	{
		this->GetOwner()->SetActive(true);
		
		SetActive(false);
	}

	CheckIfDied(aScene);

}

void TurretComponent::ImguiUpdate()
{
#ifdef _DEBUG
	if (ImGui::BeginTabItem("Enemy"))
	{
		ImGui::SeparatorText("Stats");
		ImGui::SliderFloat("Health", &myHealthPoint, 0, myMaxHealth);
		ImGui::DragFloat("Max Health", &myMaxHealth, 1, 1);
		ImGui::SliderFloat("AttackCooldown", &myAttackCooldown, 4, 50);
		ImGui::DragFloat("AttackDamage", &myAttackDamage, 1, 1);
		ImGui::DragFloat("My Attack Range", &myRange, 1, 1);

		ImGui::SeparatorText("All Enemies");
		if (ImGui::Button("Apply all enemies"))
		{
			MainSingleton::GetInstance().GetEnemyPool().NewStatesForTheTurret(myMaxHealth, myHealthPoint, myAttackCooldown, myAttackDamage, myRange);
		}

		ImGui::SeparatorText("Data");
		if (ImGui::Button("Save"))
		{
			SaveData();
		}

		if (ImGui::Button("Load"))
		{
			LoadData();
		}

		ImGui::EndTabItem();

	}
#endif // _DEBUG

}

void TurretComponent::Reset()
{
	EnemyComponent::Reset();
	this->GetOwner()->SetActive(true);
	SetActive(true);
	myPhysicsComponent->SetActive(true);
	const auto& children = this->GetOwner()->GetChildren();
	for (GameObject* child : children)
	{
		child->SetActive(true);
	}
	myHealthPoint = myMaxHealth;
	myIsAlive = true;
	ChangeAnimation(TurretAnimation::eIdle);
	myAnimationHandler->AnimationReset();

}

void TurretComponent::ChangeAnimation(TurretAnimation aAnimationType)
{
	myAnimationHandler->ChangeAnimationType(aAnimationType);
}

bool TurretComponent::IsTheAnimationFinish(TurretAnimation aAnimationType)
{
	if (myAnimationHandler->GetWhatAnimationIsPlaying() != aAnimationType)
		return true;

	return myAnimationHandler->IsTheAnimationFinish(aAnimationType);
}

TurretAnimation TurretComponent::whatAnimationIam()
{
	return TurretAnimation(myAnimationHandler->GetWhatAnimationIsPlaying());
}

void TurretComponent::AddDontChangeToIdleAnimation(TurretAnimation aAnimationType)
{
	myAnimationHandler->AddDontChangrToIdleAnimation(aAnimationType);
}

void TurretComponent::RemoveFromDontChangeToIdleAnimation(TurretAnimation aAnimationType)
{
	myAnimationHandler->RemoveFromDontChangrToIdleAnimation(aAnimationType);
}

void TurretComponent::CheckIfDied(Scene& aScene)
{
	if (!myIsAlive)
		return;
	if (myHealthPoint <= 0)
	{
		aScene.GetSoundManager()->EditParameter(3, "AmbienceSound", 1);
		aScene.GetSoundManager()->UpdatePosition(3, GetPosition());
		aScene.GetSoundManager()->PlayEvent(3);
		myPhysicsComponent->SetActive(false);
		ChangeAnimation(TurretAnimation::eDeath);
		myIsAlive = false;
		AddCardToScene(aScene);

		const auto& children = this->GetOwner()->GetChildren();

		for (GameObject* child : children)
		{
			child->SetActive(false);
		}
	}
}

void TurretComponent::SaveData()
{
	nlohmann::json json;

	json["MaxHealth"] = myMaxHealth;
	json["AttackDamage"] = myAttackDamage;
	json["AttackCooldown"] = myAttackCooldown;
	json["Range"] = myRange;

	std::ofstream file;

	file.open(STATIC_ASSET_PATH "json/TurretsSettings.json");

	if (file.is_open())
	{
		file << json.dump(3);
	}
	file.close();
}

void TurretComponent::LoadData()
{

	std::ifstream readJson;

	readJson.open(STATIC_ASSET_PATH "json/TurretsSettings.json");

	if (readJson.is_open())
	{
		nlohmann::json json;
		readJson >> json;

		myAttackCooldown = json["AttackCooldown"];
		myAttackDamage = json["AttackDamage"];
		myMaxHealth = json["MaxHealth"];
		myHealthPoint = myMaxHealth;
		myRange = json["Range"];
	}

	readJson.close();

}

void TurretComponent::OnCollision(physx::PxShape* aShape)
{
	physx::PxFilterData filterData = aShape->getQueryFilterData();

	if (filterData.word0 & PhysXManager::FilterGroup::ePLAYERATTACK)
	{
		float damageToTake = 0;
		try { damageToTake = static_cast<PhysicsComponent*>(aShape->userData)->myValue; }
		catch (const std::exception& e) { std::cout << e.what() << std::endl; }

		TakeDamage(damageToTake);
	}
}
