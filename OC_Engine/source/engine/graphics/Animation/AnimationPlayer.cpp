#include "stdafx.h"

#include "AnimationPlayer.h"
#include "../model/AnimatedMesh.h"
#include "Animation.h"

AnimationPlayer::AnimationPlayer()
{
}

//AnimationPlayer::AnimationPlayer(const AnimationPlayer& anAnimationPlayer)
//{
//	myModel = anAnimationPlayer.myModel;
//	myAnimation = nullptr;
//	if (anAnimationPlayer.myAnimation != nullptr)
//	{
//		myAnimation = std::make_shared<Animation>(*anAnimationPlayer.myAnimation);
//		myFPS = anAnimationPlayer.myAnimation->FramesPerSecond;
//	}
//	myTime = 0;
//
//	myState = AnimationState::NotStarted;
//}

void AnimationPlayer::Init(const std::shared_ptr<Animation> animation, const AnimatedMesh* model)
{
	myModel = model;
	myAnimation = animation;
	myFPS = animation->FramesPerSecond;
}

void AnimationPlayer::Update(float aDeltaTime)
{
	if (myState == AnimationState::Playing)
	{
		if (GetAnimation() == nullptr)
		{
			LogWarning("AnimationPlayer::Update: Animation is nullptr");
			return;
		}
		myTime += aDeltaTime;
		if (myTime >= GetAnimation()->Duration)
		{
			if (myIsLooping)
			{
				while (myTime >= GetAnimation()->Duration)
					myTime -= GetAnimation()->Duration;
			}
			else
			{
				myTime = GetAnimation()->Duration;
				myState = AnimationState::Finished;
			}
		}

		const float frameRate = 1.0f / myFPS;
		const float result = myTime / frameRate;
		const size_t frame = static_cast<size_t>(std::floor(result));// Which frame we're on
		const float delta = result - static_cast<float>(frame); // How far we have progressed to the next frame.

		size_t nextFrame = frame + 1;
		if (myState == AnimationState::Finished)
		{
			nextFrame = frame;
		}
		else if (nextFrame > GetAnimation()->Length)
			nextFrame = 0;

		// Update all animations
		const Skeleton* skeleton = myModel->GetSkeleton();
		for (size_t i = 0; i < skeleton->Bones.size(); i++)
		{
			const Transform& currentFrameJointXform = myAnimation->Frames[frame].LocalTransforms.find(skeleton->Bones[i].Name)._Ptr->_Myval.second;
			DirectX::XMMATRIX jointXform = currentFrameJointXform.GetMatrix();
			if (myIsInterpolating)
			{
				const Transform& nextFrameJointXform = myAnimation->Frames[nextFrame].LocalTransforms.find(skeleton->Bones[i].Name)._Ptr->_Myval.second;
				DirectX::FXMVECTOR currentFrameRotation = DirectX::XMQuaternionRotationMatrix(currentFrameJointXform.GetMatrix());
				DirectX::FXMVECTOR nextFrameRotation = DirectX::XMQuaternionRotationMatrix(nextFrameJointXform.GetMatrix());

				// Interpolate between the frames
				const Vector3f T = Vector3f::Lerp(currentFrameJointXform.GetPosition(), nextFrameJointXform.GetPosition(), delta);
				const  DirectX::XMVECTOR R = DirectX::XMQuaternionSlerp(currentFrameRotation, nextFrameRotation, delta);
				const Vector3f S = Vector3f::Lerp(currentFrameJointXform.GetScale(), nextFrameJointXform.GetScale(), delta);
				jointXform = DirectX::XMMatrixScaling(S.x, S.y, S.z) * DirectX::XMMatrixRotationQuaternion(R) * DirectX::XMMatrixTranslation(T.x, T.y, T.z);
			}

			DirectX::XMMATRIX Result = jointXform;
			myLocalSpacePose.JointTransforms[i] = Result;
		}
		myLocalSpacePose.Count = skeleton->Bones.size();
	}
}

void AnimationPlayer::UpdatePose()
{
	const size_t frame = GetFrame();// Which frame we're on
	// Update all animations
	const Skeleton* skeleton = myModel->GetSkeleton();
	for (size_t i = 0; i < skeleton->Bones.size(); i++)
	{
		const Transform& currentFrameJointXform = myAnimation->Frames[frame].LocalTransforms.find(skeleton->Bones[i].Name)._Ptr->_Myval.second;
		DirectX::XMMATRIX Result = currentFrameJointXform.GetMatrix();
		myLocalSpacePose.JointTransforms[i] = Result;
	}
	myLocalSpacePose.Count = skeleton->Bones.size();
}
