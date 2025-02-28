#include "stdafx.h"

#include "PauseState.h"
#include "State.h"
#include "StateStack.h"
#include "events/inputMapper/InputMapper.h"
#include "events/EventSystem.h"
#include "../mainSingelton/MainSingelton.h"
#include <iostream>

#include <graphics/ui/UIHandler.h>
#include <graphics/ui/UIElement.h>
#include <graphics/ui/UIButton.h>

PauseState::PauseState(StateStack& aStateStack)
{
	myStateStack = &aStateStack;
	mySelectedButtonIndex = 0;
	myUIHandler = new UIHandler();
	myUIHandler->Init("pause");
	myUIHandler->ReadUILayout();
	myUIHandler->SetVisible(true);
	std::vector<UIElement*> elements = myUIHandler->GetElements();
	myButtons.resize(3);
	for (UIElement* element : elements)
	{
		std::string name = element->GetName();
		if (name == "button")
		{
			myButton = element;
		}
		else if (name == "back")
		{
			myButtons[0] = element;
		}
		else if (name == "settings")
		{
			myButtons[1] = element;
		}
		else if (name == "quit")
		{
			myButtons[2] = element;
		}
	}

	for (size_t i = 0; i < myButtons.size(); i++)
	{
		myButtons[i]->GetComponent<UISprite>()->SetShaderType(ShaderType::eDefault);
		myButtons[i]->GetComponent<UIButton>()->SetShouldRender(false);
	}

	myButtons[mySelectedButtonIndex]->GetComponent<UISprite>()->SetShaderType(ShaderType::eGlitch);

}

PauseState::~PauseState() {}


void PauseState::Init()
{
	MainSingleton::GetInstance().GetInputMapper().GetInputWrapper().SetFocus(true, false);

	mySelectedButtonIndex = 0;
	if (!myIsActive)
	{
		MainSingleton::GetInstance().GetInputMapper().AddObserver(eInputEvent::eLeftMouseClick, this);
	}
	myIsActive = true;
}

void PauseState::UnInit()
{
	myIsActive = false;
	MainSingleton::GetInstance().GetInputMapper().RemoveObserver(eInputEvent::eLeftMouseClick, this);
}

bool PauseState::Update(float aDeltaTime)
{
	myUIHandler->Update(aDeltaTime);

	for (size_t i = 0; i < myButtons.size(); i++)
	{
		if (myButtons[i]->GetComponent<UIButton>()->GetIsHighlighted())
		{
			if (mySelectedButtonIndex != i)
			{
				myButtons[mySelectedButtonIndex]->GetComponent<UISprite>()->SetShaderType(ShaderType::eDefault);
				myButtons[i]->GetComponent<UISprite>()->SetShaderType(ShaderType::eGlitch);
				myButton->SetPosition(Vector2f(myButtons[i]->GetPosition().x, myButtons[i]->GetPosition().y));
			}
			mySelectedButtonIndex = (int)i;
		}
	}
	return myIsActive;
}

void PauseState::Render()
{
	if (myStateStack->GetCurrentState() != this)
	{
		myIsActive = false;
	}
	if (myIsActive)
	{
		myUIHandler->Draw();
	}
}

void PauseState::RecieveEvent(const eInputEvent aEvent, std::any /*aValue*/)
{
	if (!myIsActive) { return; }
	switch (aEvent)
	{
	case eInputEvent::eLeftMouseClick:
	{
		if (!myButtons[mySelectedButtonIndex]->GetComponent<UIButton>()->GetIsHighlighted())
		{
			return;
		}
		eButtonFunction func = myButtons[mySelectedButtonIndex]->GetComponent<UIButton>()->GetFunction();
		switch (func)
		{
		case eButtonFunction::eGame:
			myStateStack->PopState();
			break;
		case eButtonFunction::eSettings:
			myStateStack->PushState(eState::eOptions);
			break;
		case eButtonFunction::eQuit:
			myStateStack->PopState();
			myStateStack->PopState();
			break;
		}
		break;
	}

	default:
		break;
	}
}



void PauseState::ImGuiUpdate()
{

}