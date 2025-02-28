#include "DoorComponent.h"
#include "Windows.h"
#include "gameObject/GameObject.h"
#include "component/PhysicsComponent.h"
#include <PhysXCode/PhysXManager.h>
#include "../engine/sound/SoundManager.h"
#include "../engine/scene/Scene.h"
#include "mainSingelton/MainSingelton.h"
#include "scene/SceneManager.h"
DoorComponent::DoorComponent()
{

}

DoorComponent::~DoorComponent()
{
}

void DoorComponent::OnCollision(physx::PxShape* aShape)
{
	aShape;
	DestroyDoor();
}

void DoorComponent::Update(const float aDeltaTime, Scene& aScene)
{
	aDeltaTime;
	aScene;

	/*if (GetAsyncKeyState('K'))
	{
		bool activeOrNot = myParent->IsActive();
		myParent->SetActive(!activeOrNot);

		for (GameObject* child : myParent->GetChildren())
		{
			child->SetActive(activeOrNot);
			PhysicsComponent* physicsComponent = child->GetComponent<PhysicsComponent>();
			physicsComponent->SetActive(false);
		}
	}*/
	if (myDoorBreakSound)
	{
		myDoorBreakSound = false;
		aScene.GetSoundManager()->EditParameter(3, "AmbienceSound", 0);
		aScene.GetSoundManager()->PlayEvent(3);
		aScene.GetSoundManager()->UpdatePosition(3,myParent->GetPosition());

		GetOwner()->SetActive(false);
	}
}

void DoorComponent::ImguiUpdate()
{
#ifdef _DEBUG
	if (ImGui::BeginTabItem("Door"))
	{
		ImGui::SeparatorText("Function");
		if (ImGui::Button("Destoy"))
		{
			DestroyDoor();
		}
		ImGui::EndTabItem();
	}
#endif // _DEBUG

}

void DoorComponent::Reset()
{
	myDoorBreakSound = false;
	GetOwner()->SetActive(true);
	PhysicsComponent* parentPhysicsComponent = GetOwner()->GetComponent<PhysicsComponent>();
	parentPhysicsComponent->SetActive(true);
	int size = (int)GetOwner()->GetChildren().size();
	for (int i = 0; i < size; i++)
	{
		GetOwner()->GetChildren()[i]->SetActive(false);

		GetOwner()->GetChildren()[i]->Reset();
		PhysicsComponent* physicsComponent = GetOwner()->GetChildren()[i]->GetComponent<PhysicsComponent>();

		if (physicsComponent)
		{
			physicsComponent->SetObjectsPosition();
			physicsComponent->SetActive(false);
		}
	}
}

void DoorComponent::DestroyDoor()
{
	myDoorBreakSound = true;
	//const Vector3f position = myParent->GetPosition();
	//MainSingleton::GetInstance().GetSoundManager().EditParameter(5, "AmbienceSound", 0);
	//MainSingleton::GetInstance().GetSoundManager().PlayEvent(5);
	////MainSingleton::GetInstance().GetSoundManager().UpdatePosition(3, position);
	PhysicsComponent* parentPhysicsComponent = GetOwner()->GetComponent<PhysicsComponent>();
	parentPhysicsComponent->SetActive(false);
	int size = (int)GetOwner()->GetChildren().size();
	for (int i = 0; i < size; i++)
	{
		GetOwner()->GetChildren()[i]->SetActive(true);

		PhysicsComponent* physicsComponent = GetOwner()->GetChildren()[i]->GetComponent<PhysicsComponent>();
		if (physicsComponent)
		{
			physicsComponent->SetObjectsPosition();
			physicsComponent->SetActive(true);
		}
	}

}
