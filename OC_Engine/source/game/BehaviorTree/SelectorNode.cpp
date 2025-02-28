#include "SelectorNode.h"


SelectorNode::SelectorNode(EnemyComponent* aActor, const std::vector<BehaviorNode*>& aChildren)
    :BehaviorNode(aActor)
    ,myChildren(aChildren)
{
}

SelectorNode::~SelectorNode()
{
    for (BehaviorNode* children : myChildren)
    {
        children->~BehaviorNode();
        children = nullptr;
    }
    myChildren.clear();
}


bool SelectorNode::Execute(const float aDeltaTime, Scene& aScene)
{
    for (BehaviorNode* child : myChildren)
    {
        if (child->Execute(aDeltaTime,aScene))
        {
            return true;
        }
    }
    return false;
}
