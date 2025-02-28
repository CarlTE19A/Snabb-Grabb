#include "CanIShoot.h"
#include "../game/PollingStation.h"
#include "../game/Ammo/AmmoPool.h"
#include "../game/Ammo/AmmoComponent.h"
#include "component/Component.h"
#include "../game/EnemyComponent/EnemyComponent.h"
#include "gameObject/GameObject.h"
#include "../game/EnemyAnimation/AnimationType.h"
CanIShoot::CanIShoot(EnemyComponent* aActor, BehaviorNode* aChild)
	:ConditionNode(aActor, aChild)
{
}

CanIShoot::~CanIShoot()
{
}

bool CanIShoot::Execute(const float aDeltaTime, Scene& aScene)
{
	if (CheckCondition(aScene))
	{
		return myChild->Execute(aDeltaTime, aScene);
	}
	return false;
}

bool CanIShoot::CheckCondition(Scene& aScene) const
{
	aScene;
	if (!myActor->IsTheAnimationFinish(TurretAnimation::eShoot))
	{
		return false;
	}
	myActor->ChangeAnimation(TurretAnimation::eWindup);
	if (!myActor->GetPollingStation()->GetAmmoPool()->DoIHaveAmmo())
	{
		return false;
	}
	else if (myActor->GetAttackTimer() > 0)
	{
		myActor->AddDontChangeToIdleAnimation(TurretAnimation::eWindup);
		return false;
	}
	if (!myActor->IsTheAnimationFinish(TurretAnimation::eWindup))
	{
		return false;
	}

	myActor->RemoveFromDontChangeToIdleAnimation(TurretAnimation::eWindup);

	return true;
}
