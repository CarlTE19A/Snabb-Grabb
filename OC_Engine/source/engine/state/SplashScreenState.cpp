#include "stdafx.h"

#include "SplashScreenState.h"
#include "StateStack.h"

#include "graphics/GraphicsEngine.h"
#include "graphics\sprite\SpriteDrawer.h"

#include "gameObject/GameObject.h"
#include "component/CameraComponent.h"

#include <iostream>

#include "mainSingelton/MainSingelton.h"
#include "events/inputMapper/InputMapper.h"
#include "events/inputMapper/InputWrapper.h"

SplashScreenState::SplashScreenState(StateStack& aStateStack)
{
	myStateStack = &aStateStack;
}

SplashScreenState::~SplashScreenState()
{
	//for (size_t i = myLogos.size() - 1; i > 0 ; i--)
	for (Logo logo : myLogos)
	{
		delete logo.mySprite;
		logo.mySprite = nullptr;
	}
	{
		//delete myLogos[i].mySprite;
	}
	delete myBackground;
	myBackground = nullptr;

	delete myCamera;
	myCamera = nullptr;

	myLogos.clear();
}

void SplashScreenState::Init()
{
	MainSingleton::GetInstance().GetInputMapper().GetInputWrapper().SetFocus(false, false);
	GraphicsEngine* graphicsEngine = GraphicsEngine::GetInstance();
	Vector2f myScreenCenter = { (float)graphicsEngine->GetWindowSize().x / 2.f, (float)graphicsEngine->GetWindowSize().y / 2.f };
	Vector2f tempSpriteSize = { 0.3f,0.3f };
	myBackground = new Sprite();
	if (!myBackground->Init(STATIC_ASSET_PATH "SplashScreen/splash_background.dds"))
	{
		std::cout << "Failed to create BackGround for Splahscreen" << std::endl;
		return;
	}
	Vector2f backgroundsize = { 5.f, 5.f };
	myBackground->SetPosition(myScreenCenter);
	myBackground->SetSize(backgroundsize);

	Logo tgaLogo;
	myLogos.push_back(tgaLogo);
	myLogos.back().mySprite = new Sprite();
	/*	if (!myLogos.back().mySprite->Init("SplashScreen/tge_logo_w.dds"))
		{
			std::cout << "Failed to create sprite in tge_logo_w splash screen" << std::endl;
		}*/
	if (!myLogos.back().mySprite->Init(STATIC_ASSET_PATH "SplashScreen/tge_logo_w.dds"))
	{
		std::cout << "Failed to create sprite in tge_logo_w splash screen" << std::endl;
	}
	myLogos.back().mySprite->SetSize(tempSpriteSize);
	myLogos.back().mySprite->SetPosition(myScreenCenter);
	myLogos.back().myDuration = 2.f;

	Logo groupLogo;
	myLogos.push_back(groupLogo);
	myLogos.back().mySprite = new Sprite();
	if (!myLogos.back().mySprite->Init(STATIC_ASSET_PATH "SplashScreen/rotate_around_mittens.dds"))
	{
		std::cout << "Failed to create sprite in StateStack splash screen" << std::endl;
	}
	myLogos.back().mySprite->SetSize(tempSpriteSize);
	myLogos.back().mySprite->SetPosition(myScreenCenter);
	myLogos.back().myDuration = 2.f;

	Logo fmodLogo;
	myLogos.push_back(fmodLogo);
	myLogos.back().mySprite = new Sprite();
	if (!myLogos.back().mySprite->Init(STATIC_ASSET_PATH "SplashScreen/FmodWhiteSplash.dds"))
	{
		std::cout << "Failed to create sprite in StateStack splash screen" << std::endl;
	}
	tempSpriteSize = { 0.5f,0.5f };
	myLogos.back().mySprite->SetSize(tempSpriteSize);
	myLogos.back().mySprite->SetPosition(myScreenCenter);
	myLogos.back().myDuration = 2.f;



	myCamera = new GameObject();
	myCamera->AddComponent(new CameraComponent());
	CameraComponent* cameraComponent = myCamera->GetComponent<CameraComponent>();

	graphicsEngine->SetCamera(cameraComponent);
	myCamera->myName = "Splash Screen Camera";
	myCamera->SetPosition({ 0, 0, 0 });
	cameraComponent->SetPerspectiveProjection(90.f, (float)graphicsEngine->GetWindowSize().x, (float)graphicsEngine->GetWindowSize().y, 0.1f, 100000.f);

	LogInfo("Init SplashScreen");

	myIsActive = true;
	myCurrentLogo = 0;
	myLogos[myCurrentLogo].myTimer = 0;
}

void SplashScreenState::UnInit()
{
	LogInfo("UnInit SplashScreen");
}

bool SplashScreenState::Update(float aDeltaTime)
{
	//TODO the time from startup is long as everything is loading
	if (aDeltaTime > 1)
	{
		aDeltaTime = 0;
	}
	myLogos[myCurrentLogo].myTimer += aDeltaTime;
	if (myLogos[myCurrentLogo].myTimer > myLogos[myCurrentLogo].myDuration)
	{

		myCurrentLogo++;
		if (myCurrentLogo >= myLogos.size())
		{
			myIsActive = false;
			myStateStack->PopState();
			myStateStack->PushState(eState::eMenu);
		}
		else
		{
			myLogos[myCurrentLogo].myTimer = 0;
		}
	}
	aDeltaTime;
	return myIsActive;
}

void SplashScreenState::Render()
{
	if (myIsActive)
	{
		SpriteDrawer& spriteDrawer = GraphicsEngine::GetInstance()->GetSpriteDrawer();

		spriteDrawer.DrawSprite(*myBackground);
		if (myCurrentLogo < myLogos.size())
		{
			spriteDrawer.DrawSprite(*myLogos[myCurrentLogo].mySprite);
		}
	}
}


void SplashScreenState::ImGuiUpdate()
{

}
