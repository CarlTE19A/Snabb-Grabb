#pragma once

enum class eState
{
	eMenu,
	eLevelSelect,
	eOptions,
	eGame,
	ePause,
	eCredits,
	eSplashScreen,
	eEditor,
	eLoading,
	eWin,
	eDeath,
	eStart,
	eHowToPlay,
};

class StateStack;

class State
{
public:
	State(StateStack& aStateStack);
	State();
	virtual ~State();
	virtual void Init() = 0;
	virtual void UnInit() = 0;
	virtual bool Update(float aDeltaTime) = 0;
	virtual void Render() = 0;
	virtual void ImGuiUpdate() = 0;
	virtual eState GetType() = 0;

protected:
	StateStack* myStateStack;
	bool myIsActive;
};