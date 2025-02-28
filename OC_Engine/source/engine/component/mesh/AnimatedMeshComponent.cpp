#include "stdafx.h"

#include "AnimatedMeshComponent.h"
#include "gameObject/GameObject.h"
#include "graphics/model/AnimatedMesh.h"
#include "graphics\model\ModelShader.h"

#include "mainSingelton/MainSingelton.h"
#include "graphics\model\ModelFactory.h"

AnimatedMeshComponent::AnimatedMeshComponent(std::vector<AnimatedMesh*> aMesh) 
	: myMeshes(aMesh)
	, myIsHighLighted(false)
	, myActiveAnimation(-1)
{}

AnimatedMeshComponent::AnimatedMeshComponent(AnimatedMeshComponent& aAnimatedMeshComponent)
{
	myMeshes = aAnimatedMeshComponent.myMeshes;
	myActiveAnimation = -1;
	//myAnimations = aAnimatedMeshComponent.myAnimations;
}

AnimatedMeshComponent::~AnimatedMeshComponent()
{
	for (AnimatedMesh* mesh : myMeshes)
	{
		delete mesh;
	};
	myMeshes.clear();
};

void AnimatedMeshComponent::Update(const float aDeltaTime, Scene&)
{
	if (myActiveAnimation > -1)
	{
		myAnimations[myActiveAnimation].Update(aDeltaTime);
	}

	for (AnimatedMesh* mesh : myMeshes)
	{
		//mesh->SetTransform(myParent->MultiplyWithParent());
		if (myActiveAnimation > -1)
		{
			mesh->SetPose(myAnimations[myActiveAnimation]);
		}
	};
}

void AnimatedMeshComponent::ImguiUpdate()
{
	if (ImGui::BeginTabItem("Mesh"))
	{
		ImguiNativeUpdate();
		ImGui::EndTabItem();
	};
}

void AnimatedMeshComponent::Render(ModelShader& aModelShader) {
	if (!IsActive())
	{
		return;
	}
	for (AnimatedMesh* mesh : myMeshes)
	{
		mesh->Render(aModelShader, *myParent->GetTransform(), myIsHighLighted);
	};
}

void AnimatedMeshComponent::AddAnimation(std::string aFilePath, bool aShouldInterPolate, bool aShouldLoop)
{
	myAnimations.push_back(MainSingleton::GetInstance().GetModelFactory().GetAnimationPlayer(aFilePath, myMeshes[0]));
	myAnimations.back().SetIsLooping(aShouldLoop);
	myAnimations.back().SetIsInterpolating(aShouldInterPolate);
}

void AnimatedMeshComponent::PlayAnimation(const int anAnimationIndex)
{
	if (myActiveAnimation > -1)
	{
		if (/*myAnimations[myActiveAnimation].GetState() == AnimationState::Finished ||*/ myActiveAnimation != anAnimationIndex)
		{
			myAnimations[myActiveAnimation].Stop();
		}
	}

	myActiveAnimation = anAnimationIndex;
	myAnimations[myActiveAnimation].Play();
}

const AnimationState AnimatedMeshComponent::GetAnimationState(const int anAnimationIndex) const
{
	if (myActiveAnimation > -1)
	{
		return myAnimations[anAnimationIndex].GetState();
	}
	return AnimationState::NoAnimation;
}

void AnimatedMeshComponent::ResetAnimation(const int anAnimationIndex)
{
	myAnimations[anAnimationIndex].Stop();
}
