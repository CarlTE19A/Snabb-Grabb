#pragma once

#include "ConditionNode.h"

class CanIShoot : public ConditionNode
{
public:

	CanIShoot(EnemyComponent* aActor, BehaviorNode* aChild);

	virtual ~CanIShoot()override;

	virtual bool Execute(const float aDeltaTime, Scene& aScene)override;
	virtual bool CheckCondition(Scene& aScene) const override;


};