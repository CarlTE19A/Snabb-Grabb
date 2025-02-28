#pragma once
#include <string>
#include <vector>
#include "scene/sceneData.h"

class GameManager
{
public:
	GameManager();
	~GameManager();

	void Init();

	void ImGuiShowScenes();

	std::string GetPlayerName() const;
	std::string GetPrivatePlayerName() const;

	void SetPlayerName(const std::string& aName) { playerName = aName; }

	//SceneData& GetSceneData(const std::string& sceneName);
	SceneData* GetSceneData(const int& index);
	float GetTime() const { return myTime; }
	void SetTime(const float aTime) { myTime = aTime; }

	void SaveScore(const int& aSceneID, const float& aTime);
	void LoadScore(const int& aSceneID);

	void ActivateNextScene();
	void ActivatePreviousScene();

	private:

		std::string playerName;
		std::string playerID;
		std::vector<SceneData> scenesData;
		float myTime;
		int myCurrentSceneIndex;
};