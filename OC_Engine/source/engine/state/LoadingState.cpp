#include "stdafx.h"

#include "LoadingState.h"
#include "StateStack.h"

#include "graphics/GraphicsEngine.h"
#include "graphics\sprite\SpriteDrawer.h"

#include "gameObject/GameObject.h"
#include "component/CameraComponent.h"

#include <iostream>

LoadingState::LoadingState(StateStack& aStateStack)
{
	myStateStack = &aStateStack;
}

LoadingState::~LoadingState()
{
	delete myBackground;
	myBackground = nullptr;
	delete myLoadSprite;
	myLoadSprite = nullptr;
	delete myCamera;
	myCamera = nullptr;
}

void LoadingState::Init()
{
	GraphicsEngine* graphicsEngine = GraphicsEngine::GetInstance();
	Vector2f myScreenCenter = { (float)graphicsEngine->GetWindowSize().x / 2.f, (float)graphicsEngine->GetWindowSize().y / 2.f };

	myBackground = new Sprite();
	myBackground->Init("LoadingScreen/loading_background.dds");
	myBackground->SetPosition(myScreenCenter);

	myLoadSprite = new Sprite();
	myLoadSprite->Init("LoadingScreen/tge_logo_w.dds");
	myLoadSprite->SetPosition(myScreenCenter);

	myCamera = new GameObject();
	myCamera->AddComponent(new CameraComponent());
	CameraComponent* cameraComponent = myCamera->GetComponent<CameraComponent>();

	graphicsEngine->SetCamera(cameraComponent);
	myCamera->myName = "Loading Camera";
	myCamera->SetPosition({ 0, 0, 0 });
	cameraComponent->SetPerspectiveProjection(90.f, (float)graphicsEngine->GetWindowSize().x, (float)graphicsEngine->GetWindowSize().y, 0.1f, 100000.f);

	LogInfo("Init Loading Screen");

	myIsActive = true;
}

void LoadingState::UnInit()
{
	LogInfo("UnInit Loading Screen");
	myIsActive = false;
}

bool LoadingState::Update(float aDeltaTime)
{
	if (myIsDone)
	{
		//myStateStack->PopState();
		return false;
	}
	aDeltaTime;
	return myIsActive;
}

void LoadingState::Render()
{
	if (myIsActive)
	{
		SpriteDrawer& spriteDrawer = GraphicsEngine::GetInstance()->GetSpriteDrawer();

		spriteDrawer.DrawSprite(*myLoadSprite);
		spriteDrawer.DrawSprite(*myBackground);
	}
}


void LoadingState::ImGuiUpdate()
{

}
