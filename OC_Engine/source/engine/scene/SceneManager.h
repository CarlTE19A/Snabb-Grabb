#pragma once
#include <unordered_map>
#include "sceneData.h"
class Scene;

class SceneManager
{
public:
	SceneManager();
	~SceneManager();

	Scene* GetActiveScene();
	//deletes current scene and loads new scene, only use for gamestate not editorstate.
	void SetActiveScene(const std::string aSceneName);
	void SetActiveSceneByOrderIndex(const int aSceneIndex);
	void SetActiveSceneByID(const int aSceneID);
	void UnactivateAllScenes();


	std::vector<SceneData> GetFolderScenesData();
	void LoadAllScenes();
	Scene* LoadSceneFromName(std::string aSceneName);
	Scene* LoadSceneFromPath(std::string aScenePath);
	inline std::vector<Scene*>& GetScenes() { return myScenes; }
	//Scene* GetResetScene(const std::string aScenePath);
	void ResetScene();
	//DO NOT USE THIS FUNCTION, it resets the scene if ResetScene() is called.
	void SceneRestetSwitch();
	bool GetIsResetting() { return myIsResettingScene; }
	const bool GetRequestedSceneChange() { return myRequestedSceneChange; }
	inline const std::string GetToBeLoadedSceneName() const { return myToBeLoadedScene; }

	

private:
	int myActiveSceneIndex;
	bool myRequestedSceneChange;
	std::string myToBeLoadedScene;
	std::vector<Scene*> myScenes;
	std::unordered_map<std::string, int> mySceneNameToIndex;
	bool myIsResettingScene;
};