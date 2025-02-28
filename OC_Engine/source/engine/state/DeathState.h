#pragma once

#include "State.h"
#include "events/inputMapper/InputObserver.h"
//#include "graphics\ui\UIButton.h"

class StateStack;
class UIHandler;

class DeathState : public State, public InputObserver
{
public:
	DeathState(StateStack& aStateStack);
	~DeathState() override;

	void Init() override;
	void UnInit() override;
	bool Update(float aDeltaTime) override;
	void Render() override;
	void ImGuiUpdate() override;

	eState GetType() override { return eState::ePause; }

	void RecieveEvent(const eInputEvent aEvent, std::any aValue) override;

private:
	UIHandler* myUIHandler;
};
