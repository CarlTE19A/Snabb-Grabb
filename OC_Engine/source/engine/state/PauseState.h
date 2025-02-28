#pragma once

#include "State.h"
#include "events/inputMapper/InputObserver.h"
//#include "graphics\ui\UIButton.h"

class StateStack;
class UIHandler;
class UIElement;

class PauseState : public State, public InputObserver
{
public:
	PauseState(StateStack& aStateStack);
	~PauseState() override;

	void Init() override;
	void UnInit() override;
	bool Update(float aDeltaTime) override;
	void Render() override;
	void ImGuiUpdate() override;

	eState GetType() override { return eState::ePause; }

	void RecieveEvent(const eInputEvent aEvent, std::any aValue) override;

private:
	int mySelectedButtonIndex;

	UIElement* myButton;
	std::vector<UIElement*> myButtons;
	//UIButton mySettingsButton;
	UIHandler* myUIHandler;
};
