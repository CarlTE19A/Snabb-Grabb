#pragma once

#include "EnemyComponent.h"

#include "../game/EnemyAnimation/AnimationType.h"
#include "PhysX/PxShape.h"

template<typename AnimationType>
class AnimationHandler;
class Scene;
class TurretComponent : public EnemyComponent
{
public:
	TurretComponent(PollingStation* aPollingStation);

	~TurretComponent()override;

	void Init() override;
	virtual void Update(const float aDeltaTime, Scene& aScene)override;

	
	virtual void ImguiUpdate()override;
	virtual void Reset()override;


	virtual void ChangeAnimation(TurretAnimation aAnimationType)override;

	virtual bool IsTheAnimationFinish(TurretAnimation aAnimationType)override;
	
	virtual TurretAnimation whatAnimationIam()override;

	virtual void AddDontChangeToIdleAnimation(TurretAnimation aAnimationType)override;

	virtual void RemoveFromDontChangeToIdleAnimation(TurretAnimation aAnimationType)override;

	virtual void CheckIfDied(Scene& aScene)override;

	void SaveData();

	void LoadData();

	void OnCollision(physx::PxShape* aShape);

private:

	AnimationHandler<TurretAnimation>* myAnimationHandler;
};
