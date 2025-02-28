#include "stdafx.h"

#include "StateStack.h"
#include "State.h"
#include <iostream>

//States
#include "GameState.h"
#include "OptionsState.h"
#include "MenuState.h"
#include "PauseState.h"
#include "LevelSelectState.h"
#include "CreditsState.h"
#include "SplashScreenState.h"
#include "EditorState.h"
#include "LoadingState.h"
#include "DeathState.h"
#include "WinState.h"
#include "StartState.h"
#include "HowToPlayState.h"

#include "mainSingelton\MainSingelton.h"

StateStack::StateStack()
{
	myMenuState = new MenuState(*this);
	myPauseState = new PauseState(*this);
	myIngameState = new GameState(*this);
	myEditorState = new EditorState(*this);
	myCreditsState = new CreditsState(*this);
	myOptionsState = new OptionsState(*this);
	myLoadingState = new LoadingState(*this);
	myLevelSelectState = new LevelSelectState(*this);
	mySplashScreenState = new SplashScreenState(*this);
	myHowToPlayState = new HowToPlayState(*this);

	myDeathState = new DeathState(*this);
	myWinState = new WinState(*this);
	myStartState = new StartState(*this);
}

StateStack::~StateStack()
{
	myEditorState->UnInit();
	delete myMenuState;
	delete myPauseState;
	delete myEditorState;
	delete myIngameState;
	delete myCreditsState;
	delete myOptionsState;
	delete myLoadingState;
	delete myLevelSelectState;
	delete mySplashScreenState;
	delete myHowToPlayState;

	delete myDeathState;
	delete myWinState;
	delete myStartState;
}

void StateStack::PushState(eState aState)
{
	if (GetCurrentState() != nullptr)
	{
		if (aState != eState::ePause)
		{
			GetCurrentState()->UnInit();
		}
	}
	switch (aState)
	{
	case eState::eMenu:
		myStateStack.push_back(myMenuState);
		break;
	case eState::eOptions:
		myStateStack.push_back(myOptionsState);
		break;
	case eState::eLevelSelect:
		myStateStack.push_back(myLevelSelectState);
		break;
	case eState::eGame:
		myStateStack.push_back(myIngameState);
		break;
	case eState::ePause:
		myStateStack.push_back(myPauseState);
		break;
	case eState::eCredits:
		myStateStack.push_back(myCreditsState);
		break;
	case eState::eSplashScreen:
		myStateStack.push_back(mySplashScreenState);
		break;
	case eState::eEditor:
		myStateStack.push_back(myEditorState);
		break;
	case eState::eLoading:
		myStateStack.push_back(myLoadingState);
		break;
	case eState::eDeath:
		myStateStack.push_back(myDeathState);
		break;
	case eState::eWin:
		myStateStack.push_back(myWinState);
		break;
	case eState::eStart:
		myStateStack.push_back(myStartState);
		break;
	case eState::eHowToPlay:
		myStateStack.push_back(myHowToPlayState);
		break;
	default:
		assert(false && "State not found");
		break;
	}

	GetCurrentState()->Init();
}

void StateStack::PopState()
{
	myStateStack[myStateStack.size() - 1]->UnInit();
	myStateStack.pop_back();
	if (myStateStack.size() > 0)
	{
		GetCurrentState()->Init();
	}
}

bool StateStack::UpdateState(float aTimeDelta)
{
	aTimeDelta;
	if (GetCurrentState() == nullptr)
	{
		return false;
	}
	if (GetCurrentState()->GetType() != eState::ePause && !(GetCurrentState()->GetType() == eState::eOptions /* && LevelManager::GetInstance()->GetCurrentSceneIndex() != 0*/))
	{
		//LevelManager::GetInstance()->GetCurrentScene().Update(aTimeDelta);
		//update level
	}
	if (GetCurrentState()->GetType() == eState::ePause)
	{
		//MainSingleton::GetInstance().GetSceneManager().ResetScene();
	}

	int a = static_cast<int>(myStateStack.size());
	auto b = myStateStack[a - 1];
	if (!GetCurrentState()->Update(aTimeDelta) && b == GetCurrentState())
	{
		PopState();
	}

	return myStateStack.size() > 0;
}

void StateStack::RenderState()
{
	if (GetCurrentState() == nullptr)
	{
		return;
	}

	if (GetCurrentState()->GetType() == eState::eStart || GetCurrentState()->GetType() == eState::ePause)
	{
		int a = static_cast<int>(myStateStack.size());
		auto b = myStateStack[a - 2];
		b->Render();
	}

	GetCurrentState()->Render();


}

State* StateStack::GetCurrentState()
{
	if (myStateStack.size() == 0)
	{
		return nullptr;
	}
	return myStateStack[myStateStack.size() - 1];
}
