#include "stdafx.h"
#include "PlayerBulletComponent.h"
#include "gameObject/GameObject.h"

#include "mainSingelton/MainSingelton.h"
#include "scene/SceneManager.h"
#include "scene/Scene.h"

PlayerBulletComponent::PlayerBulletComponent()
{
}

PlayerBulletComponent::~PlayerBulletComponent()
{
}

float PlayerBulletComponent::Start(Vector3f aStartPos, Vector3f aEndPos, float aSpeed)
{
	aSpeed;
	myIsPlaying = true;

	Vector3f direction = (aEndPos - aStartPos).GetNormalized(); // Calculate direction
	movePerSecond = direction * aSpeed; // Calculate movement speed
	float distance = (aEndPos - aStartPos).Length();
	completionTime = distance / aSpeed; // Calculate time based on constant speed

	myParent->SetPosition(aStartPos);

	myParent->GetTransform()->RotateTowards(aEndPos, 1000.f);

	return completionTime;
}

void PlayerBulletComponent::Update(const float aDeltaTime, Scene& aScene)
{
	if (myIsPlaying)
	{
		completion += aDeltaTime;
		//LogInfo("Pos:" + std::to_string(myParent->GetPosition().x) + ", " + std::to_string(myParent->GetPosition().y) + ", " + std::to_string(myParent->GetPosition().z));
		myParent->SetPosition(myParent->GetPosition() + movePerSecond * aDeltaTime); // Move based on speed and time
		timer += aDeltaTime;
		if (completionTime < completion && timer > 2)
		{
			myIsPlaying = false;
			//LogSuccess("PlayerBullet Done");
			DeleteMe(aScene);
		}
	}
}

void PlayerBulletComponent::ImguiUpdate()
{
}

void PlayerBulletComponent::DeleteMe(Scene& aActiveScene)
{
	aActiveScene.RemoveGameObject(myParent);
	delete myParent;
}

void PlayerBulletComponent::Reset()
{
	Scene* scene = MainSingleton::GetInstance().GetSceneManager().GetActiveScene();
	DeleteMe(*scene);
}
