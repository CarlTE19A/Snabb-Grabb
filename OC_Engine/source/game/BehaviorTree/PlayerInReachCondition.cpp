#include "PlayerInReachCondition.h"
#include "../game/EnemyComponent/EnemyComponent.h"
#include "../PollingStation.h"
#include "../Player/PlayerComponent.h"
#include "gameObject/GameObject.h"
#include "component/Component.h"
PlayerInReachCondition::PlayerInReachCondition(EnemyComponent* aActor, BehaviorNode* aChild)
	:ConditionNode(aActor, aChild)
{
}

PlayerInReachCondition::~PlayerInReachCondition()
{
}

bool PlayerInReachCondition::CheckCondition(Scene& aScene) const
{

	aScene;

	
	float playersDistance = GetDistance(myActor->GetPosition(), myActor->GetPollingStation()->GetPlayer()->GetPosition());

		if (playersDistance < 0)
		{
			playersDistance *= -1;
		}

	if (myActor->GetAttackRange() > playersDistance)
	{
		//std::cout << "Player in reach" << std::endl;
		return true;
	}
	//std::cout << "Player not reach" << std::endl;
	myActor->ChangeAnimation(TurretAnimation::eIdle);
	return false;
}

float PlayerInReachCondition::GetDistance(const Vector3f aStart, const Vector3f aTarget)const
{
	float x = aTarget.x - aStart.x;
	float y = aTarget.y - aStart.y;
	float z = aTarget.z - aStart.z;

	return std::sqrtf(x * x + y * y + z * z);
}

bool PlayerInReachCondition::Execute(const float aDeltaTime, Scene& aScene)
{
	if (CheckCondition(aScene))
	{
		return myChild->Execute(aDeltaTime, aScene);
	}
	return false;
}

