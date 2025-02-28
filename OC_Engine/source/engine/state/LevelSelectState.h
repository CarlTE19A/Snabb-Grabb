#pragma once

#include "State.h"
#include "events/inputMapper/InputObserver.h"
#include <vector>

class UIHandler;
class UIElement;

class LevelSelectState : public State, public InputObserver
{
public:
	LevelSelectState(StateStack& aStateStack);
	~LevelSelectState() override;

	void Init() override;
	void UnInit() override;
	bool Update(float aDeltaTime) override;
	void Render() override;
	void ImGuiUpdate() override;

	eState GetType() override { return eState::eLevelSelect; }

	void RecieveEvent(const eInputEvent aEvent, std::any aValue) override;
	void TestFunction();
private:

	UIHandler* myUIHandler;

	UIElement* myButton;
	UIElement* myLvlButton;
	std::vector<UIElement*> myButtons;
	UIElement* myLeaderboardElement;


	std::vector<UIElement*> myLevelButtons;
	int mySelectedButtonIndex;
	int mySelectedLevelButtonIndex;
	int mySelectedLevelIndex;

	bool myShowTopPlayers;
	std::vector<std::string> myTopPlayers;
	std::vector<std::string> myPlayerTop;

	void LoadLevel(int aIndex, eState aState);
};