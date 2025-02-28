#include "stdafx.h"

#include "OptionsState.h"
#include "State.h"
#include "StateStack.h"

#include <events/inputMapper/InputMapper.h>
#include <events/EventSystem.h>
#include <mainSingelton/MainSingelton.h>
#include <graphics/GraphicsEngine.h>
#include <graphics/sprite/SpriteDrawer.h>
#include <component/CameraComponent.h>
#include <gameObject/GameObject.h>
#include <graphics/ui/UIHandler.h>
#include <graphics/ui/UIElement.h>
#include <graphics/ui/UIButton.h>
#include <graphics/ui/UIText.h>

OptionsState::OptionsState(StateStack& aStateStack)
{
	myStateStack = &aStateStack;
	Engine& engine = *Engine::GetInstance();
	myFullScreen = engine.GetFullScreen();
	myWindowSize = engine.GetWindowSize();
	myUIHandler = new UIHandler();
	myUIHandler->Init("settings");
	myUIHandler->ReadUILayout();
	myUIHandler->SetVisible(true);


	myResolutionIndex = 0;
	myResolutionOptions[0] = Vector2<int>(1920, 1080);
	myResolutionOptions[1] = Vector2<int>(1280, 720);
	myResolutionOptions[2] = Vector2<int>(640, 360);


	std::vector<UIElement*> elements = myUIHandler->GetElements();
	myButtons.resize(4);
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
		else if (name == "leftArrow")
		{
			myButtons[1] = element;
		}
		else if (name == "rightArrow")
		{
			myButtons[2] = element;
		}
		else if (name == "resText")
		{
			myButtons[3] = element;
		}
		//else if (name == "htp")
		//{
		//	myButtons[2] = element;
		//}
		//else if (name == "settings")
		//{
		//	myButtons[3] = element;
		//}
		//else if (name == "credits")
		//{
		//	myButtons[4] = element;
		//}
	}

	for (size_t i = 0; i < myButtons.size() -1; i++)
	{
		myButtons[i]->GetComponent<UISprite>()->SetShaderType(ShaderType::eDefault);
		myButtons[i]->GetComponent<UIButton>()->SetShouldRender(false);
	}

	myButtons[mySelectedButtonIndex]->GetComponent<UISprite>()->SetShaderType(ShaderType::eGlitch);

}

OptionsState::~OptionsState()
{
}

void OptionsState::Init()
{
	MainSingleton::GetInstance().GetInputMapper().GetInputWrapper().SetFocus(true, false);
	//mySelectedButton = 0;
	myIsActive = true;
	Engine& engine = *Engine::GetInstance();
	myFullScreen = engine.GetFullScreen();
	myWindowSize = engine.GetWindowSize();
	mySelectedButtonIndex = 0;

	//myBackground.SetPosition({ 0.5f * (float)myWindowSize.x, 0.5f * (float)myWindowSize.y, 0.0f });
	Vector2f tempButtonPos = { 0.5f * (float)myWindowSize.x, 0.5f * (float)myWindowSize.y };

	tempButtonPos = { 0.5f * (float)myWindowSize.x, 0.7f * (float)myWindowSize.y };
	//myExitButton.GetSprite().SetPosition(tempButtonPos);
	MainSingleton::GetInstance().GetInputMapper().AddObserver(eInputEvent::eLeftMouseClick, this);
	GraphicsEngine::GetInstance()->GetCamera()->GetOwner()->SetPosition(Vector3f(0, 0, 0));
}

void OptionsState::UnInit()
{
	myIsActive = false;
	MainSingleton::GetInstance().GetInputMapper().RemoveObserver(eInputEvent::eLeftMouseClick, this);
}

bool OptionsState::Update(float aDeltaTime)
{
	aDeltaTime;
	myUIHandler->Update(aDeltaTime);

	for (size_t i = 0; i < myButtons.size() - 1; i++)
	{
		if (myButtons[i]->GetComponent<UIButton>()->GetIsHighlighted())
		{
			if (mySelectedButtonIndex != i)
			{
				myButtons[mySelectedButtonIndex]->GetComponent<UISprite>()->SetShaderType(ShaderType::eDefault);
				myButtons[i]->GetComponent<UISprite>()->SetShaderType(ShaderType::eGlitch);
				//myButton->SetPosition(Vector2f(myButtons[i]->GetPosition().x, myButtons[i]->GetPosition().y));
			}
			mySelectedButtonIndex = (int)i;
		}
	}
	return myIsActive;
}

void OptionsState::Render()
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

void OptionsState::RecieveEvent(const eInputEvent aEvent, std::any /*aValue*/)
{
	/*if (aEvent == eInputEvent::eResetFullScreen)
	{
		Engine::GetInstance()->SetFullScreen(myFullScreen);
		Engine::GetInstance()->SetResolution(myResolutionOptions[myResolutionIndex], false);
	}*/
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
			case eButtonFunction::eQuit:
				myStateStack->PopState();
				break;
			case eButtonFunction::eGame:
			{
				if (myResolutionIndex > 0)
				{
					myResolutionIndex--;
					myButtons[3]->GetComponent<UIText>()->SetText(std::to_string(myResolutionOptions[myResolutionIndex].x) + "x" + std::to_string(myResolutionOptions[myResolutionIndex].y));
				}
				if (Engine::GetInstance()->GetFullScreen())
				{
					Engine::GetInstance()->SetResolution(myResolutionOptions[myResolutionIndex], false);
				}
				else
				{
					Engine::GetInstance()->SetResolution(myResolutionOptions[myResolutionIndex]);
				}
				break;
			}
			case eButtonFunction::eLevelSelect:
			{
				if (myResolutionIndex < 2)
				{
					myResolutionIndex++;
					myButtons[3]->GetComponent<UIText>()->SetText(std::to_string(myResolutionOptions[myResolutionIndex].x) + "x" + std::to_string(myResolutionOptions[myResolutionIndex].y));
				}
				if (Engine::GetInstance()->GetFullScreen())
				{
					Engine::GetInstance()->SetResolution(myResolutionOptions[myResolutionIndex], false);
				}
				else
				{
					Engine::GetInstance()->SetResolution(myResolutionOptions[myResolutionIndex]);
				}
				break;
			}
		}
		break;
	}

	default:
		break;
	}
}


void OptionsState::ImGuiUpdate()
{

}