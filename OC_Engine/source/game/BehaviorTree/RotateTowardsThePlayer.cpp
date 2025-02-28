#include "RotateTowardsThePlayer.h"
#include "../PollingStation.h"
#include "gameObject/GameObject.h"
#include "../game/EnemyComponent/EnemyComponent.h"
RotateTowardsThePlayer::RotateTowardsThePlayer(EnemyComponent* aActor)
    :ActionNode(aActor)
{
}

RotateTowardsThePlayer::~RotateTowardsThePlayer()
{
}

bool RotateTowardsThePlayer::Execute(const float aDeltaTime, Scene& aScene)
{
    aScene;
    const Vector3f& playerPosition = myActor->GetPollingStation()->GetPlayer()->GetPosition();
    myActor->GetOwner()->GetChildren()[1]->GetTransform()->RotateTowardsAroundY(playerPosition, aDeltaTime * 100);

    myActor->GetOwner()->GetTransform()->RotateTowardsAroundY(playerPosition, aDeltaTime * 100);
    //myActor->GetOwner()->GetTransform()->RotateTowardsAroundX(playerPosition, aDeltaTime * 100);
    //myActor->GetOwner()->GetTransform()->RotateTowards(playerPosition, aDeltaTime * 100);
    //myActor->GetOwner()->GetTransform()->RotateAroundAxis(Vector3f(0,0,1),1);
    //myActor->GetOwner()->GetTransform()->RotateTowardsAroundX(playerPosition, aDeltaTime * 100);
    
    return true;
}
