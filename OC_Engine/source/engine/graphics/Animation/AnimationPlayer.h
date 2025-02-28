#pragma once
//#include <memory>
#include "Pose.h"

#include <memory>


enum class AnimationState : uint8_t
{
	NoAnimation,
	NotStarted,
	Playing,
	Stopped,
	Paused,
	Finished,
};

class AnimatedMesh;
struct Animation;

class AnimationPlayer
{
public:
	AnimationPlayer();
	//AnimationPlayer(const AnimationPlayer& anAnimationPlayer);

	void Init(const std::shared_ptr<Animation> animation, const AnimatedMesh* model);

	/**
	 * Updates the animation and writes a new pose.
	 */
	void Update(float aDeltaTime);
	void UpdatePose();
	/**
	 * Instructs the animation to play.
	 */
	void AnimationPlayer::Play()
	{
		myState = AnimationState::Playing;
	}

	/**
	 * Pauses the animation at the current frame.
	 */
	void AnimationPlayer::Pause()
	{
		myState = AnimationState::Paused;
	}

	/**
	 * Stops and rewinds the current animation to the beginning.
	 */
	void AnimationPlayer::Stop()
	{
		myState = AnimationState::Stopped;
		myTime = 0;
	}

	bool GetIsLooping() const { return myIsLooping; }
	void AnimationPlayer::SetIsLooping(bool shouldLoop) { myIsLooping = shouldLoop; }

	bool GetIsInterpolating() const { return myIsInterpolating; }
	void AnimationPlayer::SetIsInterpolating(bool shouldInterpolate) { myIsInterpolating = shouldInterpolate; }

	float GetFramesPerSecond() const { return myFPS; }
	void AnimationPlayer::SetFramesPerSecond(float someFPS)
	{
		myFPS = someFPS;
	}

	float GetTime() const { return myTime; }
	void AnimationPlayer::SetTime(float time)
	{
		myTime = time;
	}

	void AnimationPlayer::SetFrame(unsigned int frame)
	{
		myTime = frame / myFPS;
	}

	const LocalSpacePose& GetLocalSpacePose() const { return myLocalSpacePose; }
	std::shared_ptr<Animation> GetAnimation() const { return myAnimation; }
	AnimationState GetState() const { return myState; }

	bool IsValid() const { return myModel ? true : false; }

	unsigned int GetFrame() { return static_cast<unsigned int>(myTime * myFPS); }


	const AnimatedMesh* myModel = nullptr;
private:
	std::shared_ptr<Animation> myAnimation = nullptr;
	float myTime = 0;
	bool myIsLooping = false;
	bool myIsInterpolating = true;
	float myFPS = 0.f;

	AnimationState myState = AnimationState::NotStarted;

	LocalSpacePose myLocalSpacePose;
};