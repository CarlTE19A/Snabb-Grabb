#pragma once
#include "AnimationHandler.h"
#include "component/mesh/AnimatedMeshComponent.h"
#include "AnimationType.h"

#include <iostream>

#include <Windows.h>

template<typename AnimationType>
inline AnimationHandler<AnimationType>::AnimationHandler(AnimatedMeshComponent* aAnimationMeshComponent)
	:myAnimationMeshComponent(aAnimationMeshComponent)
{
}

template<typename AnimationType>
AnimationHandler<AnimationType>::~AnimationHandler()
{

}

template<typename AnimationType>
void AnimationHandler<AnimationType>::Update()
{
	if (myUseNextAnimationState)
	{
		CanChangeAnimation();
	}
	else
	{
		HasTheAnimationStopChangeToIdle();
	}

	if (myAnimationHasChange)
	{
		myAnimationMeshComponent->PlayAnimation((int)myAnimationState);
		myAnimationHasChange = false;
	}

}

template<typename AnimationType>
void AnimationHandler<AnimationType>::ChangeAnimationType(AnimationType aAnimationType)
{
	if (myAnimationState == aAnimationType)
	{
		return;
	}

	for (size_t i = 0; i < mySpecialAnimation.size(); i++)
	{
		if (myAnimationState == mySpecialAnimation[i])
		{
			myUseNextAnimationState = true;
			myNextAnimationState = aAnimationType;
			return;
		}
	}
	myAnimationState = aAnimationType;
	myAnimationMeshComponent->ResetAnimation((int)myAnimationState);
	myAnimationHasChange = true;

}

template<typename AnimationType>
void AnimationHandler<AnimationType>::CanChangeAnimation()
{

	if (AnimationState::Finished == myAnimationMeshComponent->GetAnimationState((int)myAnimationState))
	{
		myAnimationState = myNextAnimationState;
		myUseNextAnimationState = false;
		myAnimationHasChange = true;
	}
	else
	{
		std::cout << "The animation is not finish" << std::endl;
	}
}

template<typename AnimationType>
void AnimationHandler<AnimationType>::HasTheAnimationStopChangeToIdle()
{

	for (size_t i = 0; i < myAnimationDontChangeToIdle.size(); ++i)
	{
		if ((int)myAnimationDontChangeToIdle[i] == (int)myAnimationState)
		{
			return;
		}
	}

	AnimationState debugState = myAnimationMeshComponent->GetAnimationState((int)myAnimationState);


	if (AnimationState::Finished == debugState)
	{

		myAnimationState = AnimationType::eIdle;
		myUseNextAnimationState = false;
		myAnimationHasChange = true;

	}


}

template<typename AnimationType>
bool AnimationHandler<AnimationType>::DeathAnimationFinish()
{
	if ((int)AnimationType::eDeath == (int)myAnimationState)
	{
		myUseNextAnimationState = false;
		myAnimationHasChange = false;
		return IsTheAnimationFinish(AnimationType::eDeath);
	}

	return false;
}

template<typename AnimationType>
bool AnimationHandler<AnimationType>::IsTheAnimationFinish(AnimationType aAnimationType)
{
	AnimationState debugState = myAnimationMeshComponent->GetAnimationState((int)aAnimationType);
	if ((int)aAnimationType == (int)TurretAnimation::eDeath)
	{
		int a;
		a = 0;
	}
	if (AnimationState::Finished == debugState || AnimationState::Stopped == debugState)
	{
		return true;
	}
	return false;
}

template<typename AnimationType>
void AnimationHandler<AnimationType>::AddDontChangrToIdleAnimation(AnimationType aAnimationType)
{
	myAnimationDontChangeToIdle.push_back(aAnimationType);
}

template<typename AnimationType>
void AnimationHandler<AnimationType>::RemoveFromDontChangrToIdleAnimation(AnimationType aAnimationType)
{
	for (size_t i = 0; i < myAnimationDontChangeToIdle.size(); i++)
	{
		if ((int)myAnimationDontChangeToIdle[i] == (int)aAnimationType)
		{
			myAnimationDontChangeToIdle.erase(myAnimationDontChangeToIdle.begin() + i);
			break;
		}
	}
}

template<typename AnimationType>
void AnimationHandler<AnimationType>::AnimationReset()
{
	myAnimationHasChange = true;
	myUseNextAnimationState = false;
	myAnimationState = AnimationType::eIdle;
	myNextAnimationState = AnimationType::eIdle;
	
}

template<typename AnimationType>
AnimationType AnimationHandler<AnimationType>::GetWhatAnimationIsPlaying()
{
	return myAnimationState;
}

template<typename AnimationType>
void AnimationHandler<AnimationType>::AddSpecialAnimation(AnimationType aAnimationType)
{
	mySpecialAnimation.push_back(aAnimationType);
}


