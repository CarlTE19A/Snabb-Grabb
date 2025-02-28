#pragma once

#include "BehaviorNode.h"
#include <vector>

class SequenceNode : public BehaviorNode
{
public:
	
	SequenceNode(EnemyComponent* aActor, const std::vector<BehaviorNode*>& aChildren);
	~SequenceNode()override;
	virtual bool Execute(const float aDeltaTime, Scene& aScene)override;

private:

	std::vector<BehaviorNode*>myChirldren;

};
