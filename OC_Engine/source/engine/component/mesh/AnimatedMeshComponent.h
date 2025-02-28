#pragma once
#include "../Component.h"
#include "graphics\Animation\AnimationPlayer.h"

#include <string>
#include <vector>

class AnimatedMesh;
class ModelShader;

class AnimatedMeshComponent : public Component
{
public:
	AnimatedMeshComponent(std::vector<AnimatedMesh*> aMesh);
	AnimatedMeshComponent(AnimatedMeshComponent& aAnimatedMeshComponent);
	~AnimatedMeshComponent() override;

	void Update(const float aDeltaTime, Scene& aScene) override;

	void ImguiUpdate();

	void Render(ModelShader& aModelShader);

	inline std::vector<AnimatedMesh*>& GetMesh() { return myMeshes; }

	inline void SetPath(std::string aPath) { path = aPath; }
	inline std::string GetPath() { return path; }

	//push back animation in vector
	void AddAnimation(std::string aFilePath, bool aShouldInterPolate, bool aShouldLoop);
	//indexed like vector, meaning first animation starts at 0
	void PlayAnimation(const int anAnimationIndex);

	const AnimationState GetAnimationState(const int anAnimationIndex) const;

	void ResetAnimation(const int anAnimationIndex);
	void SetIsHighLighted(const bool aBool) { myIsHighLighted = aBool; }
	void Reset() override
	{
		//Does not need reset
	}
	AnimatedMeshComponent* Copy()
	{
		return new AnimatedMeshComponent(*this);
	}

private:
	std::vector<AnimatedMesh*> myMeshes;
	std::string path;
	std::vector<AnimationPlayer> myAnimations;
	int myActiveAnimation;
	bool myIsHighLighted;
};