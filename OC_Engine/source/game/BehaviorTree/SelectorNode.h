#pragma once

#include "BehaviorNode.h"
#include <vector>


class SelectorNode : public BehaviorNode
{
public:
	SelectorNode(EnemyComponent* aActor,const std::vector<BehaviorNode*>&aChildren);
	~SelectorNode()override;
	virtual bool Execute(const float aDeltaTime, Scene& aScene)override;

private:

	std::vector<BehaviorNode*>myChildren;

};

