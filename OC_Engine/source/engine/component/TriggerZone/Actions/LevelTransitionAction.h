#pragma once
#include "Action.h"
class GameObject;

class LevelTransitionAction : public Action
{
public:
	~LevelTransitionAction() override;
	void Activate(Scene& aScene) override;
	void Reset() override;
	bool Update(float deltaTime, Scene& aScene) override;
	void SetNextSceneName(const std::string aNextSceneName) { nextSceneName = aNextSceneName; }
private:
	std::string nextSceneName;
};