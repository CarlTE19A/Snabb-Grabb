#include "EnemyComponent.h"
#include "../PollingStation.h"
#include "../BehaviorTree/BehaviorNode.h"
#include "gameObject/GameObject.h"
#include "math/Transform.h"
#include "scene/Scene.h"
#include "component/PhysicsComponent.h"
#include "../CardSystem/CardComponent.h"
#include "Paths.h"

#include "graphics/model/ModelFactory.h"
#include "mainSingelton/MainSingelton.h"
#include "component/mesh/MeshComponent.h"

EnemyComponent::EnemyComponent(PollingStation* aPollingStation)
	:myPollingStation(aPollingStation)
{

}

EnemyComponent::~EnemyComponent()
{
}


void EnemyComponent::SetHealthPoint(const float aHealthPoint)
{
	myHealthPoint = aHealthPoint;
}

void EnemyComponent::SetMaxHealthPoint(const float aMaxHealthPoint)
{
	myMaxHealth = aMaxHealthPoint;
}

void EnemyComponent::SetAttackDamage(const float aAttackDamage)
{
	myAttackDamage = aAttackDamage;
}

void EnemyComponent::SetAttackCooldown(const float aAttackCooldown)
{
	myAttackCooldown = aAttackCooldown;
}

void EnemyComponent::SetRange(const float aRange)
{
	myRange = aRange;
}

void EnemyComponent::SetPosition(const Vector3f aPosition)
{
	myPosition = aPosition;
}

void EnemyComponent::SetDirection(const Vector3f aDirection)
{
	myDirection = aDirection;

}

void EnemyComponent::SetRotation(const float aRotation)
{
	myParent->GetTransform()->RotateAroundWorldY(aRotation);
}

void EnemyComponent::SetPositionAndRotationToScene(const Vector3f aPosition, const float angle)
{
	if (this->GetOwner()->GetChildren().empty())
	{

		this->GetOwner()->GetTransform()->RotateAroundY(angle);
		this->GetOwner()->GetTransform()->SetPosition(aPosition);

	}
	else
	{
		Vector3f position = aPosition;
		/*position.y -= 151.f;*/
		for (size_t i = 0; i < this->GetOwner()->GetChildren().size(); i++)
		{
			this->GetOwner()->GetChildren()[i]->SetPosition(position);
			this->GetOwner()->GetChildren()[i]->GetTransform()->RotateAroundY(angle);

		}
		Vector3f offsetPosition = position;
		offsetPosition.y += 146;
		this->GetOwner()->SetPosition(offsetPosition);
		this->GetOwner()->GetTransform()->RotateAroundY(angle);

	}

}

void EnemyComponent::SetAttackTimer()
{
	myAttackTimer = myAttackCooldown;
}

void EnemyComponent::ReactToHit(const float aDamageTaken)
{
	myHealthPoint -= aDamageTaken;

	GameObject* gameObject = this->GetOwner();

	gameObject->SetActive(false);

	if (!gameObject->GetChildren().empty())
	{
		for (GameObject* child : gameObject->GetChildren())
		{
			child->SetActive(false);
		}
	}
}

void EnemyComponent::TakeDamage(const float aDamageTaken)
{
	myHealthPoint -= aDamageTaken;
}

void EnemyComponent::SetPoolIndex(const int aIndex)
{
	myPoolIndex = aIndex;
}

int EnemyComponent::GetPoolIndex() const
{
	return myPoolIndex;
}

void EnemyComponent::SetEnemyTyp(const EnemyType aEnemyType)
{
	myEnemyInformation.myEnemyType = aEnemyType;
}

EnemyType EnemyComponent::GetEnemyTyp() const
{
	return myEnemyInformation.myEnemyType;
}

EnemyInfo EnemyComponent::GetEnemyInfo() const
{
	return myEnemyInformation;
}

float EnemyComponent::GetAttackRange() const
{
	return myRange;
}

float EnemyComponent::GetAttackTimer() const
{
	return myAttackTimer;
}

float EnemyComponent::GetHealth() const
{
	return myHealthPoint;
}

void EnemyComponent::SetBehaviorNode(BehaviorNode* aBehaviorNode)
{
	myBehaviorNode = aBehaviorNode;
}

PollingStation* EnemyComponent::GetPollingStation()
{
	return myPollingStation;
}

void EnemyComponent::RotateTowardThePlayer(const Vector3f& aPosition, const float aRotationSpeed)
{
	/*myParent->GetTransform()->RotateTowardsAroundY(aPosition, aRotationSpeed);
	myParent->GetTransform()->RotateTowardsAroundX(aPosition, aRotationSpeed);*/
	myParent->GetTransform()->RotateTowards(aPosition, aRotationSpeed);
}

void EnemyComponent::CheckIfDied(Scene& aScene)
{
	aScene;
}

bool EnemyComponent::IsAlive() const
{
	return myIsAlive;
}

void EnemyComponent::AddCardToScene(Scene& aScene)
{
	GameObject* newCard = CreateGameObject();

	if (newCard)
	{
		aScene.AddNewCardObject(newCard);
	}
}

GameObject* EnemyComponent::CreateGameObject()
{
	CardTypeEnum cardType = myEnemyInformation.myDropCardType;
	std::string cardPath = RELATIVE_ASSET_ASSET_PATH "card_meshes/";

	if (cardType == CardTypeEnum::Sword)
	{
		return nullptr;
	}
	physx::PxSphereGeometry sphereGeo(125);

	GameObject* newCard = new GameObject(true);

	newCard->SetPosition(myParent->GetPosition());

	CardComponent* cardComponent = new CardComponent(CardTypeEnum::COUNT);
	PhysicsComponent* physicsComponent = new PhysicsComponent();
	physicsComponent->AddGeometry(sphereGeo, true);
	physicsComponent->SetFilter(PhysXManager::FilterGroup::eCARD, PhysXManager::FilterGroup::ePLAYER);
	std::string name;
	switch (cardType)
	{
	case CardTypeEnum::Sword:

		break;
	case CardTypeEnum::DoubleJump:
		cardComponent->SetCardType(CardTypeEnum::DoubleJump);
		cardPath += "sm_doubleJumpCard.fbx";
		name = "DoubleJumpCard";
		break;
	case CardTypeEnum::Dash:
		cardComponent->SetCardType(CardTypeEnum::Dash);
		cardPath += "sm_dashCard.fbx";
		name = "DashCard";
		break;
	case CardTypeEnum::Stomp:
		cardComponent->SetCardType(CardTypeEnum::Stomp);
		cardPath += "sm_stompCard.fbx";
		name = "StompCard";
		break;
	default:
		break;
	}

	newCard->AddComponent<CardComponent>(cardComponent);
	cardComponent->SetActive(true);

	newCard->AddComponent<PhysicsComponent>(physicsComponent);
	physicsComponent->SetObjectsPosition();
	physicsComponent->SetActive(false);

	ModelFactory& modelFactory = MainSingleton::GetInstance().GetModelFactory();

	std::vector<Mesh*> cardMesh = modelFactory.Load(cardPath);

	MeshComponent* meshComponent = new MeshComponent(cardMesh);

	meshComponent->SetPath(cardPath);
	meshComponent->SetIsHighLighted(true);
	cardComponent->SetNewCardInTheScene();
	newCard->AddComponent<MeshComponent>(meshComponent);
	newCard->myName = name;
	return newCard;
}



void EnemyComponent::SetEnemyDropCard(const CardTypeEnum aDropCard)
{
	myEnemyInformation.myDropCardType = aDropCard;
}

Vector3f EnemyComponent::GetPosition()
{
	return myParent->GetPosition();
}

Vector3f EnemyComponent::GetRotation()
{
	return myParent->GetRotation();
}

void EnemyComponent::Update(const float aDeltaTime, Scene& aScene)
{
	aScene;
	//Update(aDeltaTime, aScene);
	if (myDoKillAfterTime)
	{
		myKillAfterTime -= aDeltaTime;
		if (myKillAfterTime < 0)
		{
			myHealthPoint = 0;
		}
	}
}

void EnemyComponent::ImguiUpdate()
{
}

void EnemyComponent::Reset()
{
	myDoKillAfterTime = false;
	myKillAfterTime = 0.f;
}
