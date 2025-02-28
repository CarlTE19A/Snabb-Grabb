#include <iostream>
#include "GameManager.h"
#include "mainSingelton/MainSingelton.h"
#include "scene/SceneManager.h"
#include "scene/Scene.h"
#include <imgui/imgui.h>
#include <urlmon.h>
#include "StringHelper.h"
#include <fstream>
#include <wininet.h>

GameManager::GameManager()
{
}

GameManager::~GameManager()
{
}

void GameManager::Init()
{
	SceneManager& sceneManager = MainSingleton::GetInstance().GetSceneManager();
	scenesData = sceneManager.GetFolderScenesData();
	//sceneManager.LoadSceneFromName(scenesData[0].mySceneName);
	//sceneManager.SetActiveScene("SampleScene");
}

void GameManager::ImGuiShowScenes()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Scenes"))
		{
			for (size_t i = 0; i < scenesData.size(); i++)
			{
				if (ImGui::Button(scenesData[i].mySceneName.c_str()))
				{
					SceneManager& sm = MainSingleton::GetInstance().GetSceneManager();
					sm.GetActiveScene()->Reset();
					sm.SetActiveScene(scenesData[i].mySceneName);
				}
			}
			ImGui::EndMenu();
		}
	}
	ImGui::EndMainMenuBar();
}

void GameManager::ActivateNextScene()
{
	SceneManager& sm = MainSingleton::GetInstance().GetSceneManager();
	sm.GetActiveScene()->Reset();
	myCurrentSceneIndex++;
	if (myCurrentSceneIndex >= scenesData.size())
	{
		myCurrentSceneIndex = 0;
	}
	sm.SetActiveScene(scenesData[myCurrentSceneIndex].mySceneName);
}

void GameManager::ActivatePreviousScene()
{
	SceneManager& sm = MainSingleton::GetInstance().GetSceneManager();
	sm.GetActiveScene()->Reset();
	myCurrentSceneIndex--;
	if (myCurrentSceneIndex < 0)
	{
		myCurrentSceneIndex = (int)scenesData.size() - 1;
	}
	sm.SetActiveScene(scenesData[myCurrentSceneIndex].mySceneName);
}

std::string GameManager::GetPlayerName() const
{
	return playerName;
}

std::string GameManager::GetPrivatePlayerName() const
{
	return playerName+"#"+playerID;
}

SceneData* GameManager::GetSceneData(const int& index)
{
	for(size_t i = 0; i < scenesData.size(); i++)
	{
		if(scenesData[i].mySceneID == index)
		{
			return &scenesData[i];
		}
	}
	return nullptr;
}

void GameManager::SaveScore(const int& aSceneID, const float& aTime)
{
	CoInitialize(NULL);
	//send in game=1&usr=123dfssserrbbx to flush the system completely  
	// https://www.graderad.se/highscore.php?game=100&usr=123dfssserrbbx

	//the file to download: https://www.graderad.se/highscore.php?game=1&lvl=1

	//add score https://www.graderad.se/highscore.php?game=1&lvl=1&usr=Frida&timems=11

	//example how to download a file, ?nd to just push a score to the server instead of L"test" use NULL
	std::wstring time = std::to_wstring((int)aTime);
	std::wstring sceneID = std::to_wstring(aSceneID);
	std::string playerNamestring = GetPlayerName();

	if (GetPlayerName() == "")
	{
		playerNamestring = "testPlayer";
	}

	std::wstring pn = StringHelper::s2ws(playerNamestring);

	std::wstring url = L"https://www.graderad.se/highscore.php?game=1&lvl=" + sceneID + L"&usr=" + pn + L"&timems=" + time;

	//HRESULT hr = URLDownloadToFileW(NULL, L"https://www.graderad.se/highscore.php?game=1&lvl="+aSceneID+L"&usr="+GetPrivatePlayerName()+L"&timems="+time+L"", NULL, 0, NULL);

	HRESULT hr = URLDownloadToFileW(NULL, url.c_str(), NULL, 0, NULL);

	if (SUCCEEDED(hr))
		std::cout << "Webpage updated successfully.\n";
	else
		std::cerr << "Failed to update webpage. Error code: " << hr << std::endl;

	CoUninitialize();
}

void GameManager::LoadScore(const int& aSceneID)
{
	CoInitialize(NULL);
	//send in game=1&usr=123dfssserrbbx to flush the system completely  
	// https://www.graderad.se/highscore.php?game=100&usr=123dfssserrbbx

	//the file to download: https://www.graderad.se/highscore.php?game=1&lvl=1

	//add score https://www.graderad.se/highscore.php?game=1&lvl=1&usr=Frida&timems=11
	std::wstring url = L"https://www.graderad.se/highscore.php?game=1&lvl=";
	std::wstring id = std::to_wstring(aSceneID);
	url += id;
	DeleteUrlCacheEntry(url.c_str());
	//example how to download a file, ?nd to just push a score to the server instead of L"test" use NULL
	HRESULT hr = URLDownloadToFileW(NULL, url.c_str(), L"test", 0, NULL);
	SetFileAttributes(L"test", FILE_ATTRIBUTE_NORMAL);
	if (SUCCEEDED(hr))
		std::cout << "Webpage updated successfully.\n";
	else
		std::cerr << "Failed to update webpage. Error code: " << hr << std::endl;

	CoUninitialize();
}