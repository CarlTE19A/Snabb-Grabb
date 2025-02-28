#pragma once
#include "State.h"
#include "events/inputMapper/InputObserver.h"
//#include "graphics\ui\UIButton.h"

class StateStack;
class UIHandler;
class UIElement;

class OptionsState : public State, public InputObserver
{
public:
	OptionsState(StateStack& aStateStack);
	~OptionsState() override;

	void Init() override;
	void UnInit() override;
	bool Update(float aDeltaTime) override;
	void Render() override;
	void ImGuiUpdate() override;

	eState GetType() override { return eState::eOptions; }
	
	void RecieveEvent(const eInputEvent aEvent, std::any aValue) override;
private:
	bool myFullScreen;
	Vector2<int> myWindowSize;
	//Sprite myBackground;
	//UIButton myExitButton;
	Vector2<int> myResolutionOptions[3];
	int myResolutionIndex;


	UIHandler* myUIHandler;

	UIElement* myButton;
	std::vector<UIElement*> myButtons;
	UIElement* myResolutionUI[3];

	int mySelectedButtonIndex;

	//void CreateButtons();
};
