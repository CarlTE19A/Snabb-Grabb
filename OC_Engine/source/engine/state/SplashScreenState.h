#pragma once
#include "State.h"
//#include "graphics\sprite\Sprite.h"
#include "graphics\sprite\SpriteDrawer.h"

class Sprite;
class GameObject;

struct Logo
{
	Sprite* mySprite;
	float myDuration;
	float myTimer;
};

class SplashScreenState : public State
{
public:
	SplashScreenState(StateStack& aStateStack);
	~SplashScreenState() override;

	void Init() override;
	void UnInit() override;
	bool Update(float aDeltaTime) override;
	void Render() override;
	void ImGuiUpdate() override;

	eState GetType() override { return eState::eGame; }

private:

	Sprite* myBackground;
	std::vector<Logo> myLogos;
	GameObject* myCamera;

	int myCurrentLogo = 0;
};