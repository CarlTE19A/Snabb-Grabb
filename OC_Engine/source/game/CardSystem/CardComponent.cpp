#include "CardComponent.h"

#include "gameObject/GameObject.h"
#include "PhysXCode/PhysXManager.h"
#include "Log.h"

#include "mainSingelton/MainSingelton.h"
#include "scene/SceneManager.h"
#include "scene/Scene.h"
#include "../PollingStation.h"

#include "component/PhysicsComponent.h"

CardComponent::CardComponent(CardTypeEnum aCardType)
{
	myCardType = aCardType;
	physicsComponent = nullptr;
}

void CardComponent::Update(const float, Scene&)
{
	//Face towards camera
	//myParent->GetComponent<PhysicsComponent>()->SetRotation({ 0, 0, 0 });
	GameObject* camera = MainSingleton::GetInstance().GetPollingStation().GetPlayer();
	Vector3f cameraPos = camera->GetTransform()->GetPosition();
	Transform* transform = myParent->GetTransform();
	transform->RotateTowardsAroundY(cameraPos, 360.f);
	//transform->RotateAroundX(aDeltaTime * 100.f);
	//transform->RotateTowardsAroundX(cameraPos, 360.f);
	//transform->RotateTowardsAroundAxis(cameraPos, { 0, 1, 0 }, 360.f);
	myParent->GetComponent<PhysicsComponent>()->SetObjectsPosition();

	if (!isColliderActive)
	{
		physicsComponent->SetCollisionFunction(std::bind(&CardComponent::OnCollision, this, std::placeholders::_1));
		physicsComponent->SetActive(true);
		isColliderActive = true;
	}
}

void CardComponent::ImguiUpdate()
{
	if (ImGui::BeginTabItem("Card"))
	{
		ImGui::Text("Type: %d", myCardType);
		ImGui::EndTabItem();
	};
}

void CardComponent::Reset()
{
	//myParent->GetComponent<PhysicsComponent>()->SetSimulationDoPair(true);
	myParent->GetComponent<MeshComponent>()->SetActive(true);
}

void CardComponent::OnCollision(physx::PxShape* aShape)
{
	physx::PxFilterData filterData = aShape->getQueryFilterData();
	if (filterData.word0 & PhysXManager::ePLAYER)
	{
		//Log("Card hit by player");
		//myParent->GetComponent<PhysicsComponent>()->SetSimulationDoPair(false);
		myParent->GetComponent<MeshComponent>()->SetActive(false);
		//Hide card
	}
}

void CardComponent::SetNewCardInTheScene()
{
	myIAmFromEnemy = true;
	isColliderActive = false;
	physicsComponent = myParent->GetComponent<PhysicsComponent>();
}
