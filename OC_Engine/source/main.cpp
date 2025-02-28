#include "Engine.h"
#include <ApplicationWindow\ApplicationWindow.h>
#include "graphics/GraphicsEngine.h"
#include "math/Timer.h"
#include "engine/settings/settings.h"
#include "game/Game.h"

#include "mainSingelton/MainSingelton.h"
#include "events/inputMapper/InputMapper.h"
#include "sound/SoundManager.h"
#include "scene/SceneManager.h"
#include "game/GameManager/GameManager.h"

#include <stdlib.h>
#include <shellapi.h>

#include <filesystem>
#include "events\EventSystem.h"
#include <fstream>
#include <Paths.h>

#include "nlohmann\json.hpp"

#include "Log.h"

#include "tools/memoryTracker/MemoryTracker.h"

#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"
#include <imgui\imgui.h>
#include "tools\dropManager\DropManager.h"


#include "Script-Base/ExampleNodes.h"
#include "Script-Base/Game/GameNodes.h"
#include "Script-Base/Script/Editor/ScriptEditor.h"
#include "Script-Base/Script/Nodes/CommonNodes.h"
#include "Script-Base/Script/ScriptNodeTypeRegistry.h"
#include "Script-Base/Imgui/ImGuiInterface.h"

#include <graphics\model\ModelFactory.h>

#include "../game/EnemyComponent/EnemyPool.h"

#include <urlmon.h>

bool setBackScreen = false;

LRESULT CALLBACK WndProc(HWND /*hWnd*/, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CHAR:
	{
		int ascii = int(wParam);
		if (ascii > 32 && ascii < 127 || ascii == 8 || ascii == 13)
		{
			MainSingleton::GetInstance().GetEventSystem().SendMsg(Message(Message::Type::InputKeyPressed, char(wParam)));
		}
		break;
	}
	case WM_ACTIVATE:
	{
		if ((wParam == WA_ACTIVE || wParam == WA_CLICKACTIVE) && setBackScreen && Engine::GetInstance()->GetFullScreen())
		{
			Engine::GetInstance()->SetFullScreen(true);
			setBackScreen = false;
		}
		else if (wParam == WA_INACTIVE && !setBackScreen)
		{
			setBackScreen = true;
		}
		break;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		if (MainSingleton::IsInitiated())
		{
			MainSingleton::GetInstance().GetInputMapper().Update(message, wParam, lParam);
		}
		return 0;
	}
	return 0;
}

//! A Input Event should NOT be mapped multiple times
void InitInputMapper()
{
	auto& inputMapper = MainSingleton::GetInstance().GetInputMapper();
	//Used for player movement
	inputMapper.MapEvent(eInputAction::eKey_W_Held, eInputEvent::eMoveUp);
	inputMapper.MapEvent(eInputAction::eKey_A_Held, eInputEvent::eMoveLeft);
	inputMapper.MapEvent(eInputAction::eKey_S_Held, eInputEvent::eMoveDown);
	inputMapper.MapEvent(eInputAction::eKey_D_Held, eInputEvent::eMoveRight);
	inputMapper.MapEvent(eInputAction::eKey_Space, eInputEvent::eSpace);
	inputMapper.MapEvent(eInputAction::eKey_F, eInputEvent::eKey_F_Pressed);
	inputMapper.MapEvent(eInputAction::eKey_Q, eInputEvent::eKey_Q_Pressed);
	inputMapper.MapEvent(eInputAction::eMouseLeft, eInputEvent::eLeftMouseClick);
	inputMapper.MapEvent(eInputAction::eMouseLeftHold, eInputEvent::eLeftMouseHold);
	inputMapper.MapEvent(eInputAction::eMouseRight, eInputEvent::eRightMouseClick);

	//Used to test player
	inputMapper.MapEvent(eInputAction::eKey_1, eInputEvent::eKey_1_Pressed);
	inputMapper.MapEvent(eInputAction::eKey_2, eInputEvent::eKey_2_Pressed);
	inputMapper.MapEvent(eInputAction::eKey_3, eInputEvent::eKey_3_Pressed);
	inputMapper.MapEvent(eInputAction::eKey_4, eInputEvent::eKey_4_Pressed);
	inputMapper.MapEvent(eInputAction::eKey_5, eInputEvent::eKey_5_Pressed);
	inputMapper.MapEvent(eInputAction::eKey_6, eInputEvent::eKey_6_Pressed);
	inputMapper.MapEvent(eInputAction::eKey_7, eInputEvent::eKey_7_Pressed);
	inputMapper.MapEvent(eInputAction::eKey_8, eInputEvent::eKey_8_Pressed);
	inputMapper.MapEvent(eInputAction::eKey_9, eInputEvent::eKey_9_Pressed);
	inputMapper.MapEvent(eInputAction::eKey_0, eInputEvent::eKey_0_Pressed);

	//Used for menu navigation
	inputMapper.MapEvent(eInputAction::eKey_Escape, eInputEvent::ePause);
	inputMapper.MapEvent(eInputAction::eKey_Enter, eInputEvent::eAccept);

	//Used for window focus
	inputMapper.MapEvent(eInputAction::eFocusBegin, eInputEvent::eFocusBegin);
	inputMapper.MapEvent(eInputAction::eFocusEnd, eInputEvent::eFocusEnd);

}

void TestFunc()
{
	CoInitialize(NULL);
	//send in game=1&usr=123dfssserrbbx to flush the system completely  
	// https://www.graderad.se/highscore.php?game=100&usr=123dfssserrbbx

	//the file to download: https://www.graderad.se/highscore.php?game=1&lvl=1

	//add score https://www.graderad.se/highscore.php?game=1&lvl=1&usr=Frida&timems=11

	//example how to download a file, ?nd to just push a score to the server instead of L"test" use NULL
	HRESULT hr = URLDownloadToFileW(NULL, L"https://www.graderad.se/highscore.php?game=1&lvl=1", L"test", 0, NULL);

	if (SUCCEEDED(hr))
		std::cout << "Webpage updated successfully.\n";
	else
		std::cerr << "Failed to update webpage. Error code: " << hr << std::endl;

	CoUninitialize();
}

int APIENTRY wWinMain(_In_ HINSTANCE /*hInstance*/, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int /*nCmdShow*/)
{

#ifdef _DEBUG
	FILE* fp;
	AllocConsole();
	freopen_s(&fp, "conin$", "r", stdin);
	freopen_s(&fp, "conout$", "w", stdout);
	freopen_s(&fp, "conout$", "w", stderr);
	printf("Debugging Window:\n");
#endif

	EngineConfiguration winconf;
	winconf.myEnableVSync = false;
	winconf.myApplicationName = L"CrackEngine";
	winconf.myWinProcCallback = [](HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {return WndProc(hWnd, message, wParam, lParam); };

	RegisterCommonNodes();
	RegisterGameNodes();
	RegisterExampleNodes();

#ifdef _DEBUG



	//MemoryTracker::MemoryTrackingSettings memoryTrackingSettings;
	//memoryTrackingSettings.myShouldStoreStackTraces = true;
	//memoryTrackingSettings.myShouldTrackAllAllocations = false;
	//MemoryTracker::StartMemoryTracking(memoryTrackingSettings);

	HRESULT result = OleInitialize(NULL);	//ImGUI Drag and Drop
	if (result != S_OK)
	{
		LogError("OleInitialize failed");
	}

#endif
#ifndef _DEBUG
	winconf.myStartInFullScreen = true; 
#endif
	if (!Engine::StartEngine(winconf))
	{
		LogError("Engine coult not start!");
		return 0;
	}

	Engine* engine = Engine::GetInstance();
	if (engine->GetWantToUpdateWindowSize())
	{
		engine->UpdateWindowSizeChanges();
	}
#ifdef _DEBUG
	DropManager dm;
	RegisterDragDrop(engine->GetApplicationWindow().GetWindowHandle(), &dm);
#endif // _DEBUG
	MainSingleton::Create();
	MainSingleton& mainSingleton = MainSingleton::GetInstance();
	mainSingleton.Init();
	mainSingleton.GetInputMapper().Init();
	InitInputMapper();

	mainSingleton.GetModelFactory().LoadMaterial(RELATIVE_MATERIAL_PATH);
	mainSingleton.CreatePool();
	mainSingleton.GetEnemyPool().Init();
	auto& soundManager = mainSingleton.GetSoundManager();
	soundManager.Init();
	soundManager.InitNewInstance2D(SoundManager::eDescriptions::eMusic, 1);
	soundManager.InitNewInstance2D(SoundManager::eDescriptions::eMenu, 2);
	soundManager.InitNewInstance3D(SoundManager::eDescriptions::eAmbience, 3, Vector3f(0, 0, 0));
	soundManager.InitNewInstance2D(SoundManager::eDescriptions::eCharacterActions, 4/*, Vector3f(0, 0, 0)*/);
	soundManager.InitNewInstance2D(SoundManager::eDescriptions::eAmbience,5);
	//soundManager.SetEventVolume(1, 1.0f);
	/*soundManager.EditParameter(1, "Music", 0);
	soundManager.EditParameter(2, "Meny", 0);
	soundManager.EditParameter(3, "Ambience", 0);
	soundManager.EditParameter(4, "CharacterActions", 0);*/
	//soundManager.PlayEvent(1);

	//Load level
	//SceneData* sceneData = mainSingleton.GetGameManager().GetSceneData(0);
	//if(sceneData)
	//{
	//	mainSingleton.GetSceneManager().SetActiveScene(sceneData->mySceneName);
	//}
	//else
	//{
	//	mainSingleton.GetSceneManager().SetActiveScene("SampleScene");
	//}

	Vector3<float> aa = Vector3<float>(0, 0, 0);

	Timer* timer = engine->GetTimer();
	Game* game = new Game();
	game->Init();

	InputMapper& inputMapper = mainSingleton.GetInputMapper();

	float time = 0;

	//TestFunc();

	while (engine->BeginFrame())
	{
		time = timer->GetDeltaTime();

		inputMapper.Update();
		soundManager.Update();
		if (time <= 0 || time > 1.f)
		{
			LogWarning("DeltaTime was 0 or negative");
			engine->EndFrame();
			continue;
		}
		if (!game->Update(time))
		{
			engine->EndFrame();
			break;
		}
		game->Render();
		engine->EndFrame();
	}

	RevokeDragDrop(engine->GetApplicationWindow().GetWindowHandle());

	delete game;
	mainSingleton.Destroy();

	engine->ShutdownEngine();
	engine = nullptr;
#ifdef DEBUG
	OleUninitialize();
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
#endif // DEBUG


	//MemoryTracker::StopMemoryTrackingAndPrint();

	return 0;
}

