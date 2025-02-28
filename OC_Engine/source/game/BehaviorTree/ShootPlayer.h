#pragma once

#include "ActionNode.h"

class ShootPlayer : public ActionNode
{
public:

	ShootPlayer(EnemyComponent* aActor);
	~ShootPlayer()override;


	virtual bool Execute(const float aDeltaTime, Scene& aScene)override;

private:
	bool shoot = false;
};
