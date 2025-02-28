#include "stdafx.h"

#include "GameState.h"
#include "State.h"
#include "StateStack.h"
#include "events/inputMapper/InputMapper.h"
#include "events/EventSystem.h"
#include "mainSingelton/MainSingelton.h"
#include "scene\SceneManager.h"
#include "graphics/GraphicsEngine.h"
#include <iostream>
#include "../game/GameManager/GameManager.h"

#include"imgui/imgui.h"

#include "scene/Scene.h"
#include "gameObject/GameObject.h"

#include "graphics\light\AmbientLight.h"

#include "graphics\sprite\SpriteDrawer.h"
#include "../game/Player/PlayerComponent.h"
#include "component\CameraComponent.h"

#include "../game/Player/PlayerUI.h"

GameState::GameState(StateStack& aStateStack)
{
	AmbientLight ambLight;
	std::wstring cubeMap = L"../assets/dikhololo_night_8k_cube_specular.dds";
	ambLight.SetCubemap(cubeMap);
	GraphicsEngine::GetInstance()->SetAmbientLight(ambLight);
	myIsActive = false;
	myStateStack = &aStateStack;
	myPlayerUI = new PlayerUI();
}

GameState::~GameState()
{
	//delete myResetSprite;
	//myResetSprite = nullptr;

}

void GameState::Init()
{
	MainSingleton::GetInstance().GetInputMapper().GetInputWrapper().SetFocus(false, true);
	if (!myIsActive)
	{

		//GraphicsEngine* graphicsEngine = GraphicsEngine::GetInstance();

		//graphicsEngine->SetCamera(myPlayer->GetComponent<PlayerComponent>()->GetCameraGameObject()->GetComponent<CameraComponent>());

		auto& inputmapper = MainSingleton::GetInstance().GetInputMapper();
		inputmapper.AddObserver(eInputEvent::ePause, this);


		sceneManager = &MainSingleton::GetInstance().GetSceneManager();
		sceneManager->ResetScene();
		Scene* activeScene = sceneManager->GetActiveScene();
		activeScene->SetActive(true);
		myPlayerUI->Init();

		Log("Init GameState");
		myIsActive = true;

		myDeathObserver = new Observer(Message::Type::LostGame, std::bind(&GameState::HandleLostGame, this, std::placeholders::_1));
		myWinObserver = new Observer(Message::Type::WonGame, std::bind(&GameState::HandleWonGame, this, std::placeholders::_1));
		myLostFocusObserver = new Observer(Message::Type::LostFocus, std::bind(&GameState::HandleLostFocus, this, std::placeholders::_1));


	}
}

void GameState::UnInit()
{
	delete myDeathObserver;
	myDeathObserver = nullptr;

	delete myWinObserver;
	myWinObserver = nullptr;

	delete myLostFocusObserver;
	myLostFocusObserver = nullptr;
	//sceneManager = nullptr;

	auto& inputmapper = MainSingleton::GetInstance().GetInputMapper();
	inputmapper.RemoveObserver(eInputEvent::ePause, this);

	myIsActive = false;
}

bool GameState::Update(float aDeltaTime)
{

#ifdef _DEBUG
	ImGuiUpdate();
#endif // _DEBUG
	MainSingleton& mainSingleton = MainSingleton::GetInstance();
	mainSingleton.GetSceneManager().SceneRestetSwitch();

	Scene* scene = sceneManager->GetActiveScene();
	if (scene == nullptr)
		return myIsActive;
	mainSingleton.GetPhysXManager().Update(aDeltaTime);
	scene->Update(aDeltaTime);
	myPlayerUI->Update(aDeltaTime);
	return myIsActive;
}

void GameState::Render()
{
	if (sceneManager != nullptr)
	{
		Scene* activeScene = sceneManager->GetActiveScene();
		if (activeScene)
		{
			activeScene->RenderDeferred();
		}
		myPlayerUI->Render();
	}
	//if (mainSingleton.GetSceneManager().GetIsResetting())
	//{
	//	SpriteDrawer& spriteDrawer = GraphicsEngine::GetInstance()->GetSpriteDrawer();
	//
	//	spriteDrawer.DrawSprite(*myResetSprite);
	//}
}

void GameState::RecieveEvent(const eInputEvent aEvent, std::any aValue)
{
	if (!myIsActive) { return; }
	if (myStateStack->GetCurrentState() != this) { return; }
	aValue;
	if (aEvent == eInputEvent::ePause)
	{
		myStateStack->PushState(eState::ePause);
	}
}


void GameState::ImGuiUpdate()
{}


void GameState::HandleWonGame(Message&)
{
	//aMessage;
	myStateStack->PushState(eState::eStart);
	myStateStack->PushState(eState::eWin);
}

void GameState::HandleLostGame(Message& aMessage)
{
	bool isFast = std::any_cast<bool>(aMessage.myData);
	myStateStack->PushState(eState::eStart);
	if (!isFast)
	{
		myStateStack->PushState(eState::eDeath);
	}
}

void GameState::HandleLostFocus(Message&)
{
	if (myStateStack->GetCurrentState() != this) { return; }
	myStateStack->PushState(eState::ePause);
}