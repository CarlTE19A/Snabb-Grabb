#pragma once

#include "AnimationType.h"
#include <vector>

class AnimatedMeshComponent;

template<typename AnimationType>
class AnimationHandler
{
public:
	AnimationHandler(AnimatedMeshComponent* aAnimationMeshComponent);

	~AnimationHandler();

	void Update();

	void ChangeAnimationType(AnimationType aAnimationType);

	void AddSpecialAnimation(AnimationType aAnimationType);
	bool DeathAnimationFinish();
	bool IsTheAnimationFinish(AnimationType aAnimationType);
	void AddDontChangrToIdleAnimation(AnimationType aAnimationType);
	void RemoveFromDontChangrToIdleAnimation(AnimationType aAnimationType);
	void AnimationReset();

	AnimationType GetWhatAnimationIsPlaying();

private:

	void CanChangeAnimation();
	void HasTheAnimationStopChangeToIdle();


	AnimatedMeshComponent* myAnimationMeshComponent;

	AnimationType myAnimationState = {};
	AnimationType myNextAnimationState = {};
	bool myUseNextAnimationState = false;
	bool myAnimationHasChange = true;
	//This is meant to look if the animation can't change instant
	std::vector<AnimationType> mySpecialAnimation;
	std::vector<AnimationType> myAnimationDontChangeToIdle;
};