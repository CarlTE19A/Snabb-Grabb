#pragma once

#include "State.h"
#include "events/inputMapper/InputObserver.h"
#include "events/EventSystem.h"

class StateStack;
class UIHandler;
class UIElement;

class WinState : public State, public InputObserver
{
public:
	WinState(StateStack& aStateStack);
	~WinState() override;

	void Init() override;
	void UnInit() override;
	bool Update(float aDeltaTime) override;
	void Render() override;
	void ImGuiUpdate() override;
	void TestFunction();

	eState GetType() override { return eState::ePause; }

	void RecieveEvent(const eInputEvent aEvent, std::any aValue) override;

	void Input(Message aMessage);
private:
	std::string playerName;
	std::string playerID;
	float playerTime;

	UIHandler* myUIHandler;

	UIElement* myButton;
	std::vector<UIElement*> myButtons;
	UIElement* myLeaderboardElement;
	UIElement* myTimeElement;
	UIElement* myRankElement;

	std::vector<UIElement*> myRankTimes;

	bool myShowTopPlayers;
	std::vector<std::string> myTopPlayers;
	std::vector<std::string> myPlayerTop;

	int mySelectedButtonIndex;

	std::vector<std::string> myRankTextures;

	Observer* myTextObserver;
};
