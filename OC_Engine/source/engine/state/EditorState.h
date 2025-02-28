#pragma once
#include "State.h"
#include "fileHandling\FileHelper.h"

#include "events/inputMapper/InputObserver.h"
#include "events/EventSystem.h"
#include "scene\SceneManager.h"
#include "graphics\ui\UIHandler.h"
#include "graphics\ui\UIElement.h"

class Scene;
class GameObject;
class CameraComponent;
class ScriptEditor;
class PlayerUI;
//Filesystem
namespace std::filesystem
{
	class directory_entry;
}

enum class InspectorActive
{
	eNone,
	eGameObject,
	eDirectoryEntry
};

enum class UIInspectorActive
{
	eNone,
	eUIElement
};

struct PathInLevels
{
	std::string myStartPath;
	std::vector<std::string> myLevels;
	std::string Path()
	{
		std::string path = myStartPath;
		for (std::string level : myLevels)
		{
			path += "/";
			path += level;
		}
		return path;
	}
};

typedef int ImGuiWindowFlags;

class EditorState : public State, public InputObserver
{
public:
	EditorState(StateStack& aStateStack);
	~EditorState() override;

	void Init() override;
	void UnInit() override;
	bool Update(float aDeltaTime) override;
	void Render() override;
	void ImGuiUpdate() override;

	eState GetType() override { return eState::eGame; }

	void RecieveEvent(const eInputEvent aEvent, std::any aValue) override;
	//void ReceiveMessage(Message& aMessage);
	

	void HandleFileDrop(Message& aMessage);
	void HandleWonGame(Message& aMessage);
	void HandleLostGame(Message& aMessage);
private:

	std::string myFileName;

	GameObject* myDebugCamera;
	//GameObject* myPlayer;

	Observer* myFileObserver;
	Observer* myDeathObserver;
	Observer* myWinObserver;

	void ControlCamera(CameraComponent* aCamera, float aDeltaTime);

	void UpdateHierarchy(ImGuiWindowFlags window_flags);
	void UpdateHierarchyGameObject(GameObject* aGameObject, Scene* aScene);
	
	void DragDropTargetScene(Scene* aScene);
	
	void UpdateInspector(ImGuiWindowFlags window_flags);
	void UpdateInspectorGameObject();
	void UpdateUIEditorWindow(ImGuiWindowFlags window_flags);
	void UpdateUIInspector(ImGuiWindowFlags window_flags);
	void UpdateUIInspectorElement();
	void UpdateDeferredWindow(ImGuiWindowFlags window_flags);

	//void UpdateDebugWindow(ImGuiWindowFlags window_flags);

	void UpdateWorldSettings(ImGuiWindowFlags window_flags);

	void UpdateParticleWindow(ImGuiWindowFlags window_flags);

	void UpdateScriptWindow(ImGuiWindowFlags window_flags);

	void SaveEditorSettings();
	void LoadEditorSettings();

	//void TestFunction();


	bool myShowHierarchy = true;
	Vector2f myHierarchyPos = Vector2f(0, 20);
	Vector2f myHierarchySize = Vector2f(250, 600);

	bool myShowInspector = true;
	Vector2f myInspectorPos = Vector2f(1335, 20);
	Vector2f myInspectorSize = Vector2f(250, 600);

	bool myShowDeferred = true;
	Vector2f myDeferredPos = Vector2f(1335, 20);
	Vector2f myDeferredSize = Vector2f(200, 200);

	bool myShowUIEditorWindow = true;
	Vector2f myUIEditorPos = Vector2f(1335, 20);
	Vector2f myUIEditorSize = Vector2f(200, 200);

	bool myShowUIInspectorWindow = true;
	Vector2f myUIInspectorPos = Vector2f(1335, 20);
	Vector2f myUIInspectorSize = Vector2f(250, 600);

	bool myShowWorldSettings = true;
	Vector2f myWorldSettingsPos = Vector2f(1335, 20);
	Vector2f myWorldSettingsSize = Vector2f(200, 200);

	//bool myShowDebugWindow = true;
	//Vector2f myDebugPos = Vector2f(1335, 20);
	//Vector2f myDebugSize = Vector2f(200, 200);

	bool myShowParticleWindow = true;
	Vector2f myParticlePos = Vector2f(1335, 20);
	Vector2f myParticleSize = Vector2f(200, 200);

	bool myShowScript = true;

	Texture myDropTexture;

	Texture myDropParticleTexture;

	bool myShowDemoWindow = false;
	bool myShowDebugLines = true;

	bool layoutChanged = false;

	//bool pauseFPS;
	//static const int dCount = 240;
	//float latestFPS[dCount];
	//float updateRate = 1.f / 60.f;
	//float timeSinceUpdate = 0.0f;
	//float avgFPS = 0.0f;

	//For imgui
	static const int maxBufferLen = 64;
	char mySearchBuffer[maxBufferLen] = "";

	float physicsTimeScale = 1.0f;

	//For Inspector
	InspectorActive myInspectorActive = InspectorActive::eNone;
	GameObject*	mySelectedGameObject = nullptr;

	bool allowDragDrop = true;
	
	std::string overridePath = "";
	std::string overrideTargetPath = "";
	bool* overrideModal = false;

	SceneManager* sceneManager;
	GraphicsEngine* graphicsEngine;

	std::vector<UIHandler> myUIHandlers;
	UIElement* mySelectedElement = nullptr;
	UIInspectorActive myUIInspectorActive = UIInspectorActive::eNone;
	int mySelectedUIHandler = 0;

	PlayerUI* myPlayerUI;

	ScriptEditor* myScriptEditor;
	
};