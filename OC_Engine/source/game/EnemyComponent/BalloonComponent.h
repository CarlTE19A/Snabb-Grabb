#pragma once

#include "EnemyComponent.h"
#include "../game/EnemyAnimation/AnimationType.h"

#include "PhysX/PxShape.h"

template<typename AnimationType>
class AnimationHandler;

class BalloonComponent : public EnemyComponent
{
public:
	BalloonComponent(PollingStation* aPollingStation);

	~BalloonComponent()override;

	void Init()override;
	virtual void Update(const float aDeltaTime,Scene&)override;


	virtual void ImguiUpdate()override;
	virtual void Reset()override;

	virtual void CheckIfDied(Scene& aScene)override;

	virtual void ChangeAnimation(BalloonAnimation aAnimationType)override;

	void OnCollision(physx::PxShape* aShape);
private:
	
	AnimationHandler<BalloonAnimation>* myAnimationHandler;

};
