#pragma once

#include "BehaviorNode.h"

class ActionNode : public BehaviorNode
{
public:

	ActionNode(EnemyComponent* aActor);
	virtual ~ActionNode()=0;
	
	virtual bool Execute(const float aDeltaTime, Scene& aScene)=0;
	

private:

};
