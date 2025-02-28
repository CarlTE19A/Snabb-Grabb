#pragma once

//#include "graphics\ui\UIButton.h"
#include "State.h"
#include "events/inputMapper/InputObserver.h"

class StateStack;
class GameObject;
class UIHandler;
class UIElement;
class UITextField;

class MenuState : public State, public InputObserver
{
public:
	MenuState(StateStack& aStateStack);
	~MenuState() override;

	void Init() override;
	void UnInit() override;
	bool Update(float aDeltaTime) override;
	void Render() override;
	void ImGuiUpdate() override;

	eState GetType() override { return eState::eMenu; }

	void RecieveEvent(const eInputEvent aEvent, std::any aValue) override;

private:
	//float myButtonOffset;
	//float myButtonPosition;

	//Sprite myMainMenu;

	//UIButton myCreditButton;
	//UIButton myOptionButton;
	//UIButton myLevelSelectButton;


	//void CreateButtons();

	UIHandler* myUIHandler;

	UIElement* myButton;
	std::vector<UIElement*> myButtons;

	int mySelectedButtonIndex;

	UITextField* myPlayerNameText;

	GameObject* myCamera;
};