#include "stdafx.h"

#include "StartState.h"
#include "State.h"
#include "StateStack.h"
#include "events/inputMapper/InputMapper.h"
#include "events/EventSystem.h"
#include "../mainSingelton/MainSingelton.h"
#include <iostream>
#include "imgui/imgui.h"
#include "graphics/ui/UIHandler.h"
#include "graphics/ui/UIElement.h"
#include "graphics/ui/UIButton.h"
#include "graphics/ui/UISprite.h"
#include "graphics/ui/UIText.h"
#include "scene/SceneManager.h"
#include "../game/GameManager/GameManager.h"
#include "scene/Scene.h"
#include <nlohmann/json.hpp>
#include <fstream>

StartState::StartState(StateStack& aStateStack)
{
	myStateStack = &aStateStack;
	myUIHandler = new UIHandler();
	myUIHandler->Init("startScreen");
	myUIHandler->ReadUILayout();
	myUIHandler->SetVisible(true);

	std::vector<UIElement*> elements = myUIHandler->GetElements();
	myRankTimes.resize(3);
	for (UIElement* element : elements)
	{
		std::string name = element->GetName();

		if (name == "rank")
		{
			myRankElement = element;
		}
		else if (name == "lb")
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
		else if (name == "lvlname")
		{
			myLvlNameElement = element;
		}
	}


	myRankTextures.resize(5);
	myRankTextures[0] = RELATIVE_ASSET_ASSET_PATH "UI_Assets/Trophies/ui_trophyDEV.dds";
	myRankTextures[1] = RELATIVE_ASSET_ASSET_PATH "UI_Assets/Trophies/ui_trophyS.dds";
	myRankTextures[2] = RELATIVE_ASSET_ASSET_PATH "UI_Assets/Trophies/ui_trophyA.dds";
	myRankTextures[3] = RELATIVE_ASSET_ASSET_PATH "UI_Assets/Trophies/ui_trophyB.dds";
	myRankTextures[4] = RELATIVE_ASSET_ASSET_PATH "UI_Assets/Trophies/ui_trophyPass.dds";
}

StartState::~StartState() {}

void StartState::Init()
{
	MainSingleton::GetInstance().GetInputMapper().GetInputWrapper().SetFocus(true, false);
	if (!myIsActive)
	{
		InputMapper& inputmapper = MainSingleton::GetInstance().GetInputMapper();

		inputmapper.AddObserver(eInputEvent::eSpace, this);
	}

	int id = MainSingleton::GetInstance().GetSceneManager().GetActiveScene()->GetSceneID();
	GameManager& gameManager = MainSingleton::GetInstance().GetGameManager();
	gameManager.LoadScore(id);
	// find my player time in the json file
	std::ifstream file;
	file.open(STATIC_SETTINGS_PATH "../test");

	if (file.is_open())
	{
		nlohmann::json json;
		file >> json;
		try
		{
			for (size_t i = 0; i < json["score"].size(); i++)
			{
				if (json["score"][i]["usr"] == MainSingleton::GetInstance().GetGameManager().GetPlayerName())
				{
					int iterator = 0;
					for (iterator; iterator < 4; iterator++)
					{
						if (json["score"][i]["usr"] <= gameManager.GetSceneData(id)->mySceneTime[iterator])
						{
							myRankElement->GetComponent<UISprite>()->GetSprite().SetTexture(myRankTextures[iterator]);
							break;
						}
					}
					break;
				}
			}
		}
		catch (const std::exception&)
		{
			std::cout << "Could not find player in json file" << std::endl;
		}
	}
	file.close();

	myLvlNameElement->GetComponent<UIText>()->SetText(gameManager.GetSceneData(id)->mySceneName);
	myRankTimes[0]->GetComponent<UIText>()->SetText(std::to_string(gameManager.GetSceneData(id)->mySceneTime[1]));
	myRankTimes[1]->GetComponent<UIText>()->SetText(std::to_string(gameManager.GetSceneData(id)->mySceneTime[2]));
	myRankTimes[2]->GetComponent<UIText>()->SetText(std::to_string(gameManager.GetSceneData(id)->mySceneTime[3]));
	TestFunction();
	myIsActive = true;
}

void StartState::UnInit()
{
	MainSingleton::GetInstance().GetInputMapper().RemoveObserver(eInputEvent::eSpace, this);
	myIsActive = false;
}

bool StartState::Update(float aDeltaTime)
{
	myUIHandler->Update(aDeltaTime);
	aDeltaTime;
#ifdef _DEBUG
	ImGuiUpdate();
#endif
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
					if (elements[i]->GetName() == std::to_string(j))
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
					if (elements[i]->GetName() == std::to_string(j))
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

void StartState::Render()
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

void StartState::RecieveEvent(const eInputEvent aEvent, std::any aValue)
{
	if (!myIsActive) { return; }
	aValue;
	switch (aEvent)
	{
	case eInputEvent::eSpace:
		myStateStack->PopState();
		break;
	default:
		break;
	}
}



void StartState::ImGuiUpdate()
{
	if (ImGui::Begin("Start Screen"))
	{
		ImGui::SetWindowPos(ImVec2(0, 0));
		ImGui::SetWindowSize(ImVec2(200, 100));
		ImGui::Text("Start :)");
		ImGui::Text("Press SPACE to start");
	}
	ImGui::End();
}

void StartState::TestFunction()
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
					if (elements[i]->GetName() == std::to_string(j))
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