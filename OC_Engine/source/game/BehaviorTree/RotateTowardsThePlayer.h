#pragma once

#include "ActionNode.h"

class RotateTowardsThePlayer : public ActionNode
{
public:

	RotateTowardsThePlayer(EnemyComponent* aActor);
	~RotateTowardsThePlayer()override;


	virtual bool Execute(const float aDeltaTime, Scene& aScene)override;

};
