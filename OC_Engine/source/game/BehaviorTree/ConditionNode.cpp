#include "ConditionNode.h"

ConditionNode::ConditionNode(EnemyComponent* aActor, BehaviorNode* aChild)
	:BehaviorNode(aActor)
	, myChild(aChild)
{
}


