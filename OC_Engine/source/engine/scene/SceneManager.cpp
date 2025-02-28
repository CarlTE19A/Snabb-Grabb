#include "stdafx.h"
#include <filesystem>

#include "SceneManager.h"
#include "Scene.h"
#include "fileHandling/JsonSceneImporter.h"
#include <fstream>
#include "nlohmann/json.hpp"

#include "mainSingelton/MainSingelton.h"
#include "PhysXCode/PhysXManager.h"

#include "../game/GameManager/GameManager.h"

SceneManager::SceneManager()
	:myActiveSceneIndex(-1)
	, myRequestedSceneChange(false)
{
	myToBeLoadedScene = "";
}

SceneManager::~SceneManager()
{
	for (size_t i = 0; i < myScenes.size(); i++)
	{
		delete myScenes[i];
	}
	myScenes.clear();
}

Scene* SceneManager::GetActiveScene()
{
	if (myActiveSceneIndex > -1)
	{
		return myScenes[myActiveSceneIndex];
	}
	
	//Load level if none exists
	MainSingleton& mainSingleton = MainSingleton::GetInstance();
	SceneData* sceneData = mainSingleton.GetGameManager().GetSceneData(0);
	if (sceneData)
	{
		mainSingleton.GetSceneManager().SetActiveScene(sceneData->mySceneName);
	}
	else
	{
		mainSingleton.GetSceneManager().SetActiveScene("SampleScene");
	}
	if (myActiveSceneIndex > -1)
	{
		return myScenes[myActiveSceneIndex];
	}
	return nullptr;
}

void SceneManager::SetActiveScene(const std::string aSceneName)
{
	if (myScenes.size() > 0 && !myRequestedSceneChange)
	{
		myRequestedSceneChange = true;
		myToBeLoadedScene = aSceneName;
		myActiveSceneIndex = -1;
	}
	else
	{
		if (myRequestedSceneChange)
		{
			MainSingleton::GetInstance().GetPhysXManager().ResetPhysXScene();
			myScenes[0]->myActive = false;
			delete myScenes[0];
			myScenes.clear();
			myScenes.push_back(LoadSceneFromName(myToBeLoadedScene));
			myScenes[0]->SetActive(true);
			myToBeLoadedScene = "";
			myRequestedSceneChange = false;
		}
		else
		{
			myScenes.push_back(LoadSceneFromName(aSceneName));
		}
		LogInfo("Setting active scene to name: " + aSceneName);
	}
}

void SceneManager::SetActiveSceneByOrderIndex(const int aSceneIndex)
{
	if (myActiveSceneIndex > -1)
	{
		myScenes[myActiveSceneIndex]->SetActive(false);
		myScenes[myActiveSceneIndex]->Reset();
	}
	myActiveSceneIndex = aSceneIndex;
	myScenes[myActiveSceneIndex]->SetActive(true);

	LogInfo("Setting active scene to index: " + std::to_string(aSceneIndex));
}

void SceneManager::SetActiveSceneByID(const int aSceneID)
{
	SceneData* sceneData = MainSingleton::GetInstance().GetGameManager().GetSceneData(aSceneID);
	if(sceneData == nullptr)
	{
		LogWarning("Could not find scene with ID: " + std::to_string(aSceneID));
		return;
	}
	SetActiveScene(sceneData->mySceneName);
}

void SceneManager::UnactivateAllScenes()
{
	for (size_t i = 0; i < myScenes.size(); i++)
	{
		myScenes[i]->SetActive(false);
	}
	myActiveSceneIndex = -1;
}

std::vector<SceneData> SceneManager::GetFolderScenesData()
{
	std::vector<SceneData> sceneData;
	for (const auto& entry : std::filesystem::directory_iterator(RELATIVE_SCENE_PATH))
	{
		if (entry.path().extension() != ".json")
		{
			continue;
		}
		std::ifstream file(entry.path().string());
		assert(file.is_open() && "Could not open file");
		nlohmann::json levelJson = nlohmann::json::parse(file);
		file.close();

		SceneData data;
		data.mySceneName = levelJson["sceneName"].get<std::string>();
		data.mySceneID = levelJson["sceneBuildIndex"].get<int>();
		data.myScenePath = entry.path().string();
		try
		{
			data.mySceneTime[0] = levelJson["spawnPlayer"]["developer"] * 1000.f;
			data.mySceneTime[1] = levelJson["spawnPlayer"]["ace"] * 1000.f;
			data.mySceneTime[2] = levelJson["spawnPlayer"]["gold"] * 1000.f;
			data.mySceneTime[3] = levelJson["spawnPlayer"]["silver"] * 1000.f;
		}
		catch(std::exception&)
		{
			data.mySceneTime[0] = 1000;
			data.mySceneTime[1] = 2000;
			data.mySceneTime[2] = 3000;
			data.mySceneTime[3] = 4000;
		}
		//sort insert by index in sceneData
		auto it = sceneData.begin();
		sceneData.push_back(data);
		
		//std::sort(sceneData.begin(), sceneData.end(), [](const SceneData& a, const SceneData& b) { return a.mySceneID < b.mySceneID; });

	}
	return sceneData;
}

void SceneManager::LoadAllScenes()
{
	std::vector<std::string> scenePathsToLoad;
#if USE_THREADING
	std::vector<std::future<Scene*>> futures;
	//std::atomic<int> counter;
#endif
		//Checks if the directory exists
	if (!std::filesystem::exists(RELATIVE_SCENE_PATH))
	{
		assert(false && "Scene directory does not exist");
		myScenes.clear();
	}
	//Checks if the directory is empty
	if (std::filesystem::is_empty(RELATIVE_SCENE_PATH))
	{
		LogWarning("Scene directory is empty");
		//assert(false && "Scene directory is empty");
		myScenes.clear();
	}
	for (const auto& entry : std::filesystem::directory_iterator(RELATIVE_SCENE_PATH))
	{
		if (entry.path().extension() != ".json")
		{
			continue;
		}		if (entry.path().string().find("_override") != std::string::npos)
		{
			continue;
		}
		scenePathsToLoad.push_back(entry.path().string());
	}
#if USE_THREADING
	//std::vector<std::thread> sceneLoadThreads;
	for (std::string path : scenePathsToLoad)
	{
		//sceneLoadThreads.push_back()
		futures.push_back(std::async(std::launch::async, LoadSceneFromPath, path));
	}
	for (int i = 0; i < futures.size(); i++)
	{
		myScenes.push_back(futures[i].get());
	}

#endif
#if !USE_THREADING
	int index = 0;
	if (scenePathsToLoad.size() > 0)
	{
		for (std::string path : scenePathsToLoad)
		{
			myScenes.push_back(LoadSceneFromPath(path));
			mySceneNameToIndex.emplace(myScenes.back()->mySceneName, index);
			index++;
		}
	}
	else
	{
		myScenes.push_back(new Scene());
		myScenes[0]->CreateCustomScene("NewScene");
		myScenes[0]->Init();
	}

#endif
}

Scene* SceneManager::LoadSceneFromName(std::string aSceneName)
{
	LogInfo("Loading scene: " + aSceneName);
	std::string sceneFullpath = RELATIVE_SCENE_PATH + aSceneName + ".json";
	return LoadSceneFromPath(sceneFullpath);
}

Scene* SceneManager::LoadSceneFromPath(std::string aScenePath)
{
	JsonSceneImporter jsonSceneImporter;

	//So it does not matter if the path is with / or \
	
	std::replace(aScenePath.begin(), aScenePath.end(), '\\', '/');
	Scene* scene = jsonSceneImporter.ImportScene(aScenePath.c_str());

	scene->Init();
	scene->SetActive(true);
	myActiveSceneIndex = 0;
	return scene;
}

void SceneManager::ResetScene()
{
	myIsResettingScene = true;

}

void SceneManager::SceneRestetSwitch()
{
	if (!myIsResettingScene)
		return;
	if (myActiveSceneIndex > -1)
	{
		myScenes[myActiveSceneIndex]->Reset();
		MainSingleton::GetInstance().GetPhysXManager().SetSkipFrame();
		//Scene* newScene = LoadSceneFromName(myScenes[myActiveSceneIndex]->mySceneName);
		//delete myScenes[myActiveSceneIndex];
		//myScenes[myActiveSceneIndex] = newScene;
	}
	myIsResettingScene = false;
}


//Scene* SceneManager::GetResetScene(const std::string aScenePath)
//{
//	return LoadSceneFromName(aScenePath);
//}
