#pragma once

#include "State.h"
//#include "World/UI/UISprite.h"
#include "events/inputMapper/InputObserver.h"
#include "events\EventSystem.h"

class SceneManager;
class Scene;
class GameObject;
class PlayerUI;

class GameState : public State, public InputObserver
{
public:
	GameState(StateStack& aStateStack);
	~GameState() override;

	void Init() override;
	void UnInit() override;
	bool Update(float aDeltaTime) override;
	void Render() override;
	void ImGuiUpdate() override;

	eState GetType() override { return eState::eGame; }

	void RecieveEvent(const eInputEvent aEvent, std::any aValue) override;
	void HandleWonGame(Message& aMessage);
	void HandleLostGame(Message& aMessage);
	void HandleLostFocus(Message&);

private:
	SceneManager* sceneManager;

	Observer* myDeathObserver;
	Observer* myWinObserver;
	Observer* myLostFocusObserver;
	PlayerUI* myPlayerUI;
};