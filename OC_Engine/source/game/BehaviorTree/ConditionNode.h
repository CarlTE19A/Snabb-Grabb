#pragma once
#include "BehaviorNode.h"

class ConditionNode : public BehaviorNode
{
public:

	 ConditionNode(EnemyComponent* aActor,BehaviorNode*aChild);
	 virtual ~ConditionNode() { delete myChild; };

	virtual bool CheckCondition(Scene& aScene)const = 0;
	virtual bool Execute(const float aDeltaTime, Scene& aScene) =0;

protected:

	BehaviorNode* myChild;

	
};
