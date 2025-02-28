#pragma once

#include "State.h"
//#include "graphics\ui\UIButton.h"
#include "events/inputMapper/InputObserver.h"

class StateStack;
class GameObject;
class UIHandler;
class UIElement;

class CreditsState : public State, public InputObserver
{
public:
	CreditsState(StateStack& aStateStack);
	~CreditsState() override;

	void Init() override;
	void UnInit() override;
	bool Update(float aDeltaTime) override;
	void Render() override;
	void ImGuiUpdate() override;

	eState GetType() override { return eState::eCredits; }

	void RecieveEvent(const eInputEvent aEvent, std::any aValue) override;

private:

	float myButtonOffset;
	float myButtonPosition;
	int mySelectedButton;

	Sprite myCreditsBackGround;
	UIHandler* myUIHandler;
	UIElement* myButton;
};