#include "stdafx.h"

#include "WinState.h"
#include "State.h"
#include "StateStack.h"
#include "events/inputMapper/InputMapper.h"
#include "../mainSingelton/MainSingelton.h"
#include <iostream>
#include "imgui/imgui.h"
#include "../game/GameManager/GameManager.h"
#include "scene/SceneManager.h"
#include "scene/Scene.h"
#include "graphics/ui/UIHandler.h"
#include "graphics/ui/UIElement.h"
#include "graphics/ui/UIButton.h"
#include "graphics/ui/UISprite.h"
#include "graphics/ui/UIText.h"

#include <nlohmann/json.hpp>
#include <fstream>



WinState::WinState(StateStack& aStateStack)
{
	myStateStack = &aStateStack;

	myUIHandler = new UIHandler();
	myUIHandler->Init("winScreen");
	myUIHandler->ReadUILayout();
	myUIHandler->SetVisible(true);

	std::vector<UIElement*> elements = myUIHandler->GetElements();
	myButtons.resize(3);
	myRankTimes.resize(3);
	for (UIElement* element : elements)
	{
		std::string name = element->GetName();
		if (name == "button")
		{
			myButton = element;
		}
		else if (name == "nextlvl")
		{
			myButtons[0] = element;
		}
		else if (name == "restart")
		{
			myButtons[1] = element;
		}
		else if (name == "menu")
		{
			myButtons[2] = element;
		}
		else if (name == "time")
		{
			myTimeElement = element;
		}
		else if (name == "rank")
		{
			myRankElement = element;
		}
		else if (name == "leaderboard")
		{
			myLeaderboardElement = element;
		}
		else if (name == "rankS")
		{
			myRankTimes[0] = element;
		}
		else if (name == "rankA")
		{
			myRankTimes[1] = element;
		}
		else if (name == "rankB")
		{
			myRankTimes[2] = element;
		}
	}

	for (size_t i = 0; i < myButtons.size(); i++)
	{
		myButtons[i]->GetComponent<UISprite>()->SetShaderType(ShaderType::eDefault);
		myButtons[i]->GetComponent<UIButton>()->SetShouldRender(false);
		myButtons[i]->GetComponent<UISprite>()->SetShouldRender(false);
	}

	//myButtons[mySelectedButtonIndex]->GetComponent<UISprite>()->SetShaderType(ShaderType::eGlitch);

	myRankTextures.resize(5);
	myRankTextures[0] = RELATIVE_ASSET_ASSET_PATH "UI_Assets/Trophies/ui_trophyDEV.dds";
	myRankTextures[1] = RELATIVE_ASSET_ASSET_PATH "UI_Assets/Trophies/ui_trophyS.dds";
	myRankTextures[2] = RELATIVE_ASSET_ASSET_PATH "UI_Assets/Trophies/ui_trophyA.dds";
	myRankTextures[3] = RELATIVE_ASSET_ASSET_PATH "UI_Assets/Trophies/ui_trophyB.dds";
	myRankTextures[4] = RELATIVE_ASSET_ASSET_PATH "UI_Assets/Trophies/ui_trophyPass.dds";

	mySelectedButtonIndex = 0;

}

WinState::~WinState() {}

void WinState::Init()
{
	MainSingleton::GetInstance().GetInputMapper().GetInputWrapper().SetFocus(true, false);
	if (!myIsActive)
	{
		InputMapper& inputmapper = MainSingleton::GetInstance().GetInputMapper();

		inputmapper.AddObserver(eInputEvent::eKey_F_Pressed, this);
		inputmapper.AddObserver(eInputEvent::eLeftMouseClick, this);
		myTextObserver = new Observer(Message::Type::InputKeyPressed, std::bind(&WinState::Input, this, std::placeholders::_1));
	}
	myIsActive = true;
	GameManager& gameManager = MainSingleton::GetInstance().GetGameManager();
	playerName = gameManager.GetPlayerName();
	playerTime = gameManager.GetTime() * 1000.f;


	int id = MainSingleton::GetInstance().GetSceneManager().GetActiveScene()->GetSceneID();
	gameManager.SaveScore(id, playerTime);
	int iterator = 0;
	for (iterator; iterator < 4; iterator++)
	{
		if (playerTime <= gameManager.GetSceneData(id)->mySceneTime[iterator])
		{
			break;
		}
	}
	myRankElement->GetComponent<UISprite>()->GetSprite().SetTexture(myRankTextures[iterator]);
	myTimeElement->GetComponent<UIText>()->SetText(std::to_string(playerTime));

	mySelectedButtonIndex = 0;

	myRankTimes[0]->GetComponent<UIText>()->SetText(std::to_string(gameManager.GetSceneData(id)->mySceneTime[1]));
	myRankTimes[1]->GetComponent<UIText>()->SetText(std::to_string(gameManager.GetSceneData(id)->mySceneTime[2]));
	myRankTimes[2]->GetComponent<UIText>()->SetText(std::to_string(gameManager.GetSceneData(id)->mySceneTime[3]));


	gameManager.LoadScore(id);
	TestFunction();
}

void WinState::UnInit()
{
	MainSingleton::GetInstance().GetInputMapper().RemoveObserver(eInputEvent::eKey_F_Pressed, this);
	InputMapper& inputmapper = MainSingleton::GetInstance().GetInputMapper();

	inputmapper.RemoveObserver(eInputEvent::eKey_F_Pressed, this);
	inputmapper.RemoveObserver(eInputEvent::eLeftMouseClick, this);

	delete myTextObserver;
	myIsActive = false;
}

bool WinState::Update(float aDeltaTime)
{
	aDeltaTime;
#ifdef _DEBUG
	ImGuiUpdate();
#endif
	myUIHandler->Update(aDeltaTime);

	for (size_t i = 0; i < myButtons.size(); i++)
	{
		if (myButtons[i]->GetComponent<UIButton>()->GetIsHighlighted())
		{
			if (mySelectedButtonIndex != i)
			{
				//myButtons[mySelectedButtonIndex]->GetComponent<UISprite>()->SetShaderType(ShaderType::eDefault);
				//myButtons[i]->GetComponent<UISprite>()->SetShaderType(ShaderType::eGlitch);
				myButton->SetPosition(Vector2f(myButtons[i]->GetPosition().x, myButtons[i]->GetPosition().y));
			}
			mySelectedButtonIndex = (int)i;
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

void WinState::Render()
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

void WinState::RecieveEvent(const eInputEvent aEvent, std::any aValue)
{
	if (!myIsActive) { return; }
	aValue;
	switch (aEvent)
	{
	case eInputEvent::eKey_F_Pressed:
		myStateStack->PopState();
		break;
	case eInputEvent::eLeftMouseClick:
	{
		if (myButtons[mySelectedButtonIndex]->GetComponent<UIButton>()->GetIsHighlighted())
		{
			eButtonFunction func = myButtons[mySelectedButtonIndex]->GetComponent<UIButton>()->GetFunction();
			switch (func)
			{
			case eButtonFunction::eGame:
			{
				myStateStack->PopState();
				int id = MainSingleton::GetInstance().GetSceneManager().GetActiveScene()->GetSceneID();
				id++;
				if (id < 10)
				{
					MainSingleton::GetInstance().GetSceneManager().SetActiveSceneByID(id);
				}
				else
				{
					myStateStack->PopState();
					myStateStack->PopState();
					myStateStack->PushState(eState::eCredits);
				}

				break;
			}
			case eButtonFunction::eQuit:
				myStateStack->PopState();
				myStateStack->PopState();
				myStateStack->PopState();
				break;

			case eButtonFunction::eHowToPlay:
				myStateStack->PopState();
				break;
			}
		}
		break;
	}

	default:
		break;
	}
}

void WinState::Input(Message aMessage)
{
	char key = std::any_cast<char>(aMessage.myData);

	if (key == 8)
	{
		if (playerName.size() > 0)
		{
			playerName.pop_back();
		}
		return;
	}
	if (key == 13)
	{
		//MainSingleton::GetInstance().GetGameManager().SaveScore(playerName, playerTime);
		myStateStack->PopState();
		return;
	}

	playerName += key;
}



void WinState::ImGuiUpdate()
{
	if (ImGui::Begin("Win Screen"))
	{
		ImGui::SetWindowPos(ImVec2(0, 0));
		ImGui::SetWindowSize(ImVec2(200, 100));
		ImGui::Text(("You won: " + playerName).c_str());
		ImGui::Text(("Your time: " + std::to_string(playerTime)).c_str());
		ImGui::Text("There will be buttons to select level");
		ImGui::Text("Press F pop state");
	}
	ImGui::End();
}


void WinState::TestFunction()
{
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
					myTopPlayers[i] += "     ";
					myTopPlayers[i] += std::to_string((int)json["score"][i]["timems"]);
					myTopPlayers[i] += "ms        ";
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
						myPlayerTop[i] += "     ";
						myPlayerTop[i] += std::to_string((int)json["score"][indexOfPlayer + index]["timems"]);
						myPlayerTop[i] += "ms        ";
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