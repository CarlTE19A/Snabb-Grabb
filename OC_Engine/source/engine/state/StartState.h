#pragma once

#include "State.h"
#include "events/inputMapper/InputObserver.h"
//#include "graphics\ui\UIButton.h"

class StateStack;
class UIHandler;
class UIElement;


class StartState : public State, public InputObserver
{
public:
	StartState(StateStack& aStateStack);
	~StartState() override;

	void Init() override;
	void UnInit() override;
	bool Update(float aDeltaTime) override;
	void Render() override;
	void ImGuiUpdate() override;

	eState GetType() override { return eState::eStart; }

	void RecieveEvent(const eInputEvent aEvent, std::any aValue) override;
	void TestFunction();
private:

	UIHandler* myUIHandler;

	UIElement* myLeaderboardElement;
	UIElement* myRankElement;
	UIElement* myLvlNameElement;

	std::vector<UIElement*> myRankTimes;

	bool myShowTopPlayers;
	std::vector<std::string> myTopPlayers;
	std::vector<std::string> myPlayerTop;
	std::vector<std::string> myRankTextures;
};
