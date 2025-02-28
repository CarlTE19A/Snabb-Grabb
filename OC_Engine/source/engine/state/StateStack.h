#pragma once
#include <vector>
class State;
enum class eState;
class MenuState;
class GameState;
class OptionsState;
class PauseState;
class LevelSelectState;
class CreditsState;
class SplashScreenState;
class EditorState;
class LoadingState;
class WinState;
class DeathState;
class HowToPlayState;

class StartState;


class StateStack
{
public:
	StateStack();
	StateStack(const StateStack&) = delete;
	~StateStack();
	void PushState(eState aState);
	void PopState();
	bool UpdateState(float aTimeDelta);
	void RenderState();


	State* GetCurrentState();
private:
	std::vector<State*> myStates;
	std::vector<State*> myStateStack;
	MenuState* myMenuState;
	GameState* myIngameState;
	OptionsState* myOptionsState;
	PauseState* myPauseState;
	LevelSelectState* myLevelSelectState;
	CreditsState* myCreditsState;
	SplashScreenState* mySplashScreenState;
	EditorState* myEditorState;
	LoadingState* myLoadingState;
	HowToPlayState* myHowToPlayState;
	
	WinState* myWinState;
	DeathState* myDeathState;
	StartState* myStartState;
};