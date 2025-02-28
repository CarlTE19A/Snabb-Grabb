#pragma once
#include "State.h"
//#include "graphics\sprite\Sprite.h"
#include <atomic>

class GameObject;
class Sprite;

class LoadingState : public State
{
public:
	LoadingState(StateStack& aStateStack);
	~LoadingState() override;

	void Init() override;
	void UnInit() override;
	bool Update(float aDeltaTime) override;
	void Render() override;
	void ImGuiUpdate() override;

	eState GetType() override { return eState::eGame; }

	std::atomic<bool> myIsDone;
private:

	Sprite* myBackground;
	Sprite* myLoadSprite;
	GameObject* myCamera;
};