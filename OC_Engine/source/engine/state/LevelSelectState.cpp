#include "stdafx.h"

#include "LevelSelectState.h"
#include "State.h"
#include "StateStack.h"

#include "events/inputMapper/InputMapper.h"
#include "events/EventSystem.h"
#include "mainSingelton/MainSingelton.h"
#include <iostream>
#include "graphics\sprite\SpriteDrawer.h"
#include <graphics/ui/UIHandler.h>
#include <graphics/ui/UIElement.h>
#include <graphics/ui/UIButton.h>
#include <graphics/ui/UIText.h>

#include "../../game/GameManager/GameManager.h"
#include "scene\SceneManager.h"


#include <fstream>
#include <nlohmann/json.hpp>


LevelSelectState::LevelSelectState(StateStack& aStateStack)
{
	myStateStack = &aStateStack;
	myTopPlayers.resize(8);
	myPlayerTop.resize(8);
	myUIHandler = new UIHandler();
	myUIHandler->Init("levelSelect");
	myUIHandler->ReadUILayout();
	myUIHandler->SetVisible(true);
	myShowTopPlayers = false;
	mySelectedLevelButtonIndex = 0;
	std::vector<UIElement*> elements = myUIHandler->GetElements();
	myButtons.resize(2);
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
			element->SetShouldRender(true);
		}
		else if (name == "lb1")
		{
			myLeaderboardElement = element;
		}
		else if (name == "lvlbutton")
		{
			myLvlButton = element;
		}

		if (element->GetComponent<UIButton>() != nullptr)
		{
			if (element->GetComponent<UIButton>()->GetFunction() == eButtonFunction::eLevelSelect)
			{
				myLevelButtons.push_back(element);
				element->SetShouldRender(false);
			}
			else if (element->GetComponent<UIButton>()->GetFunction() == eButtonFunction::eGame)
			{
				myButtons[1] = element;
			}
		}
	}

	for (size_t i = 0; i < myButtons.size(); i++)
	{
		myButtons[i]->GetComponent<UISprite>()->SetShaderType(ShaderType::eDefault);
		myButtons[i]->GetComponent<UIButton>()->SetShouldRender(false);
	}

	myButtons[mySelectedButtonIndex]->GetComponent<UISprite>()->SetShaderType(ShaderType::eGlitch);
}

LevelSelectState::~LevelSelectState()
{
	delete myUIHandler;
}


void LevelSelectState::Init()
{
	LogInfo("Init LevelSelectState");
	mySelectedButtonIndex = 0;
	if (!myIsActive)
	{
		auto& inputmapper = MainSingleton::GetInstance().GetInputMapper();

		inputmapper.AddObserver(eInputEvent::eLeftMouseClick, this);
	}
	myIsActive = true;
}

void LevelSelectState::UnInit()
{
	MainSingleton::GetInstance().GetInputMapper().RemoveObserver(eInputEvent::eLeftMouseClick, this);
	myIsActive = false;
}

bool LevelSelectState::Update(float aDeltaTime)
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
				//myButton->SetPosition(Vector2f(myButtons[i]->GetPosition().x, myButtons[i]->GetPosition().y));
			}
			mySelectedButtonIndex = (int)i;
		}
	}
	bool isHighlighted = false;
	for (size_t i = 0; i < myLevelButtons.size(); i++)
	{
		if (myLevelButtons[i]->GetComponent<UIButton>()->GetIsHighlighted())
		{
			isHighlighted = true;
			mySelectedLevelButtonIndex = (int)i;
		}
	}	

	if (myLeaderboardElement->GetComponent<UIButton>()->GetIsHighlighted())
	{
		if (!myShowTopPlayers)
		{
			std::vector<UIElement*> elements = myUIHandler->GetElements();
			std::vector<UIElement*> numbElements;
			numbElements.resize(8);
			for (size_t i = 0; i < elements.size(); i++)
			{
				for (size_t j = 0; j < 9; j++)
				{
					if (elements[i]->GetName() == std::to_string(j + 1))
					{
						numbElements[j] = elements[i];
					}
				}
			}
			for (size_t i = 0; i < numbElements.size(); i++)
			{
				numbElements[i]->GetComponent<UIText>()->SetText(myTopPlayers[i]);
			}
			myShowTopPlayers = true;
		}
	}
	else
	{
		if (myShowTopPlayers)
		{
			std::vector<UIElement*> elements = myUIHandler->GetElements();
			std::vector<UIElement*> numbElements;
			numbElements.resize(8);
			for (size_t i = 0; i < elements.size(); i++)
			{
				for (size_t j = 0; j < 9; j++)
				{
					if (elements[i]->GetName() == std::to_string(j + 1))
					{
						numbElements[j] = elements[i];
					}
				}
			}
			for (size_t i = 0; i < numbElements.size(); i++)
			{
				numbElements[i]->GetComponent<UIText>()->SetText(myPlayerTop[i]);
			}
			myShowTopPlayers = false;
		}
	}


	return myIsActive;
}

void LevelSelectState::LoadLevel(int aIndex, eState aState)
{
	aIndex;
	aState;

	myIsActive = false;
}

void LevelSelectState::Render()
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

void LevelSelectState::RecieveEvent(const eInputEvent aEvent, std::any /*aValue*/)
{
	if (!myIsActive) { return; }
	switch (aEvent)
	{
	case eInputEvent::eLeftMouseClick:
	{
		if (myButtons[mySelectedButtonIndex]->GetComponent<UIButton>()->GetIsHighlighted())
		{
			eButtonFunction func = myButtons[mySelectedButtonIndex]->GetComponent<UIButton>()->GetFunction();
			switch (func)
			{
			case eButtonFunction::eGame:
				myStateStack->PopState();
				myStateStack->PushState(eState::eGame);
				MainSingleton::GetInstance().GetSceneManager().SetActiveSceneByID(mySelectedLevelIndex);
				myStateStack->PushState(eState::eStart);
				break;
			case eButtonFunction::eQuit:
				myStateStack->PopState();
				break;
			}
		}
		else if (myLevelButtons[mySelectedLevelButtonIndex]->GetComponent<UIButton>()->GetIsHighlighted())
		{
			eButtonFunction func = myLevelButtons[mySelectedLevelButtonIndex]->GetComponent<UIButton>()->GetFunction();
			switch (func)
			{
			case eButtonFunction::eLevelSelect:
			{
				std::string name = myLevelButtons[mySelectedLevelButtonIndex]->GetName();
				myLvlButton->SetPosition(Vector2f(myLvlButton->GetPosition().x, myLevelButtons[mySelectedLevelButtonIndex]->GetPosition().y));
				name = name.back();
				mySelectedLevelIndex = std::stoi(name);
				MainSingleton::GetInstance().GetGameManager().LoadScore(mySelectedLevelIndex);
				TestFunction();
				break;
			}
			}
		}
		break;
	}
	}
}

void LevelSelectState::ImGuiUpdate()
{

}

void LevelSelectState::TestFunction()
{
	//gamemanager load score from right scene

	// read a JSON file
	std::ifstream file;
	file.open(STATIC_SETTINGS_PATH "../test");

	myTopPlayers.clear();
	myPlayerTop.clear();
	myTopPlayers.resize(8);
	myPlayerTop.resize(8);

	if (file.is_open())
	{
		nlohmann::json json;
		file >> json;
		try
		{
			bool found = false;
			int indexOfPlayer = -1;
			for (size_t i = 0; i < json["score"].size(); i++)
			{
				if (i < 8)
				{
					myTopPlayers[i] += std::to_string(i);
					myTopPlayers[i] += "        ";
					myTopPlayers[i] += std::to_string((int)json["score"][i]["timems"]);
					myTopPlayers[i] += "ms                 ";
					myTopPlayers[i] += json["score"][i]["usr"];
					if (json["score"][i]["usr"] == MainSingleton::GetInstance().GetGameManager().GetPlayerName())
					{
						found = true;
					}
				}
				if (i >= 8)
				{
					if (found)
					{
						myPlayerTop = myTopPlayers;
						break;
					}
					else
					{
						if (json["score"][i]["usr"] == MainSingleton::GetInstance().GetGameManager().GetPlayerName())
						{
							indexOfPlayer = (int)i;
							break;
						}
					}
				}
			}

			if (found)
			{
				myPlayerTop = myTopPlayers;
			}

			if (!found && indexOfPlayer != -1)
			{
				int index = -3;
				for (size_t i = 0; i < 8; i++)
				{
					if (json["score"].size() > indexOfPlayer + index)
					{
						myPlayerTop[i] += std::to_string(indexOfPlayer + index);
						myPlayerTop[i] += "        ";
						myPlayerTop[i] += std::to_string((int)json["score"][indexOfPlayer + index]["timems"]);
						myPlayerTop[i] += "ms                 ";
						myPlayerTop[i] += json["score"][indexOfPlayer + index]["usr"];
						index++;
					}
				}
			}

			std::vector<UIElement*> elements = myUIHandler->GetElements();
			std::vector<UIElement*> numbElements;
			numbElements.resize(8);
			for (size_t i = 0; i < elements.size(); i++)
			{
				for (size_t j = 0; j < 9; j++)
				{
					if (elements[i]->GetName() == std::to_string(j + 1))
					{
						numbElements[j] = elements[i];
					}
				}
			}
			for (size_t i = 0; i < numbElements.size(); i++)
			{
				numbElements[i]->GetComponent<UIText>()->SetText(myPlayerTop[i]);
			}

		}
		catch (const std::exception&)
		{
			LogError("Failed to load hierarchy editor settings");
		}
	}
	file.close();

}