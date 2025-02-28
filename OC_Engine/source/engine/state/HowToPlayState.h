#pragma once
#include "State.h"
#include "events/inputMapper/InputObserver.h"
//#include "graphics\sprite\Sprite.h"

class GameObject;
class UIHandler;
class UIElement;

class HowToPlayState : public State, public InputObserver
{
public:
	HowToPlayState(StateStack& aStateStack);
	~HowToPlayState() override;

	void Init() override;
	void UnInit() override;
	bool Update(float aDeltaTime) override;
	void Render() override;
	void ImGuiUpdate() override;

	eState GetType() override { return eState::eHowToPlay; }
	void RecieveEvent(const eInputEvent aEvent, std::any aValue) override;

private:
	UIHandler* myUIHandler;
	UIElement* myButton;
};