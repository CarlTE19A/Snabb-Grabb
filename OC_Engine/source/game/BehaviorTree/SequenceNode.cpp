#include "SequenceNode.h"
#include "ActionNode.h"
#include "ConditionNode.h"
#include <vector>

SequenceNode::SequenceNode(EnemyComponent* aActor, const std::vector<BehaviorNode*>& aChildren)
    :BehaviorNode(aActor)
    ,myChirldren(aChildren)
{
}

SequenceNode::~SequenceNode()
{
    for (BehaviorNode* child : myChirldren)
    {
        delete child;
    }
    myChirldren.clear();
}

bool SequenceNode::Execute(const float aDeltaTime, Scene& aScene)
{
    

    for (BehaviorNode* child : myChirldren)
    {
        if (!child->Execute(aDeltaTime,aScene))
        {
            return false;
        }
    }
   
    return true;
}
