#pragma once

#include "ConditionNode.h"
#include "math/Vector.h"
class PlayerInReachCondition : public ConditionNode
{
public:

	PlayerInReachCondition(EnemyComponent* aActor, BehaviorNode* aChild);
	
	virtual ~PlayerInReachCondition()override;

	virtual bool Execute(const float aDeltaTime, Scene& aScene)override;
	virtual bool CheckCondition(Scene& aScene) const override;

private:
	float GetDistance(const Vector3f aStart,const Vector3f aTarget)const;

	
};