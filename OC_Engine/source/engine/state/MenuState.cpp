#include "stdafx.h"

#include "MenuState.h"
#include "State.h"
#include "StateStack.h"
#include "events/inputMapper/InputMapper.h"
#include "events/EventSystem.h"
#include "../mainSingelton/MainSingelton.h"
#include "../game/GameManager/GameManager.h"
#include "../Sound/SoundManager.h"
#include <iostream>
#include "graphics\sprite\SpriteDrawer.h"

#include "gameObject/GameObject.h"
#include "component/CameraComponent.h"
#include "graphics\ui\UIHandler.h"
#include "graphics\ui\UIElement.h"
#include "graphics\ui\UIButton.h"
#include "graphics\ui\UITextField.h"

#include "scene/SceneManager.h"


MenuState::MenuState(StateStack& aStateStack) :
	mySelectedButtonIndex(0)
{
	myStateStack = &aStateStack;

	myUIHandler = new UIHandler();
	myUIHandler->Init("mainmenu");
	myUIHandler->ReadUILayout();
	myUIHandler->SetVisible(true);

	std::vector<UIElement*> elements = myUIHandler->GetElements();
	myButtons.resize(6);
	for (UIElement* element : elements)
	{
		std::string name = element->GetName();
		if (name == "button")
		{
			myButton = element;
		}
		else if (name == "start")
		{
			myButtons[0] = element;
		}
		else if (name == "levelselect")
		{
			myButtons[1] = element;
		}
		else if (name == "htp")
		{
			myButtons[2] = element;
		}
		else if (name == "settings")
		{
			myButtons[3] = element;
		}
		else if (name == "credits")
		{
			myButtons[4] = element;
		}
		else if (name == "quit")
		{
			myButtons[5] = element;
		}
		else if (name == "textfield")
		{
			myPlayerNameText = element->GetComponent<UITextField>();
		}
	}
	myPlayerNameText->SetActive(false);
	for (size_t i = 0; i < myButtons.size(); i++)
	{
		myButtons[i]->GetComponent<UISprite>()->SetShaderType(ShaderType::eDefault);
		myButtons[i]->GetComponent<UIButton>()->SetShouldRender(false);
	}

	myButtons[mySelectedButtonIndex]->GetComponent<UISprite>()->SetShaderType(ShaderType::eGlitch);

	
}

MenuState::~MenuState()
{
	delete myCamera;
	delete myUIHandler;
}

void MenuState::Init()
{
	MainSingleton::GetInstance().GetInputMapper().GetInputWrapper().SetFocus(true, false);
	GraphicsEngine* graphicsEngine = GraphicsEngine::GetInstance();

#ifdef _DEBUG
	LogInfo("Init MenuState");
#endif

	mySelectedButtonIndex = 0;
	if (!myIsActive)
	{
		auto& inputmapper = MainSingleton::GetInstance().GetInputMapper();

		inputmapper.AddObserver(eInputEvent::eLeftMouseClick, this);
	}

	myIsActive = true;

	myCamera = new GameObject();
	myCamera->AddComponent(new CameraComponent());
	CameraComponent* cameraComponent = myCamera->GetComponent<CameraComponent>();
	graphicsEngine->SetCamera(myCamera->GetComponent<CameraComponent>());
	myCamera->myName = "Menu Camera";
	myPlayerNameText->SetActive(true);
	myCamera->SetPosition({ 0, 0, 0 });
	cameraComponent->SetPerspectiveProjection(90.f, (float)graphicsEngine->GetWindowSize().x, (float)graphicsEngine->GetWindowSize().y, 0.1f, 100000.f);
	MainSingleton::GetInstance().GetSoundManager().PlayEvent(2);
	MainSingleton::GetInstance().GetSoundManager().StopEvent(1);
}

void MenuState::UnInit()
{
	MainSingleton::GetInstance().GetInputMapper().RemoveObserver(eInputEvent::eLeftMouseClick, this);
	myIsActive = false;
	myPlayerNameText->SetActive(false);
}

bool MenuState::Update(float aDeltaTime)
{
	aDeltaTime;
	myUIHandler->Update(aDeltaTime);

	for (size_t i = 0; i < myButtons.size(); i++)
	{
		if (myButtons[i]->GetComponent<UIButton>()->GetIsHighlighted())
		{
			if (mySelectedButtonIndex != i)
			{
				myButtons[mySelectedButtonIndex]->GetComponent<UISprite>()->SetShaderType(ShaderType::eDefault);
				myButtons[i]->GetComponent<UISprite>()->SetShaderType(ShaderType::eGlitch);
				myButton->SetPosition(Vector2f(myButton->GetPosition().x,myButtons[i]->GetPosition().y));
			}
			mySelectedButtonIndex = (int)i;
		}
	}

	return myIsActive;
}

void MenuState::Render()
{
	myUIHandler->Draw();
	//myMainMenu.Draw();
	//myCreditButton.Draw();
	//myOptionButton.Draw();
	//myLevelSelectButton.Draw();
}

void MenuState::RecieveEvent(const eInputEvent aEvent, std::any aValue)
{
	if (!myIsActive) { return; }
	aValue;
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
			MainSingleton::GetInstance().GetGameManager().SetPlayerName(myPlayerNameText->GetText());
			myPlayerNameText->SetIsCenterOfAttention(false);
			myUIHandler->SaveUILayout();
			myStateStack->PushState(eState::eGame);
			MainSingleton::GetInstance().GetSceneManager().SetActiveSceneByID(0);
			myStateStack->PushState(eState::eStart);
			break;
		case eButtonFunction::eLevelSelect:
			MainSingleton::GetInstance().GetGameManager().SetPlayerName(myPlayerNameText->GetText());
			myUIHandler->SaveUILayout();
			myStateStack->PushState(eState::eLevelSelect);
			break;
		case eButtonFunction::eHowToPlay:
			myStateStack->PushState(eState::eHowToPlay);
			break;
		case eButtonFunction::eSettings:
			myStateStack->PushState(eState::eOptions);
			break;
		case eButtonFunction::eCredits:
			myStateStack->PushState(eState::eCredits);
			break;
		case eButtonFunction::eQuit:
			myStateStack->PopState();
			break;
		case eButtonFunction::eCount:
			break;
		default:
			break;
		}
		break;
	}

	default:
		break;
	}
}

void MenuState::ImGuiUpdate()
{}
