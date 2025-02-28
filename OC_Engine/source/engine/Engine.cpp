#include "stdafx.h"

#include "Engine.h"
#include <windows.h>
#include <ApplicationWindow\ApplicationWindow.h>
#include "graphics/GraphicsEngine.h"
#include "math/Timer.h"
#include <ApplicationWindow\ApplicationWindow.h>
#include <mainSingelton\MainSingelton.h>
#include <tools\lineDrawer\LineDrawer.h>


#ifdef _DEBUG
#include "../external/imgui/imgui.h"
#include "../external/imgui/imgui_impl_win32.h"
#include "../external/imgui/imgui_impl_dx11.h"
#endif // DEBUG

#ifndef _RETAIL
#include "Script-Base/Imgui/ImGuiInterface.h"
#endif // !_RETAIL


Engine* Engine::myInstance = nullptr;
Engine::Engine(const EngineConfiguration& aWindowConfiguration)
	:myTimer(nullptr)
	, myGraphicsEngine(nullptr)
	, myShouldExit(false)
	, myWindowConfiguration(aWindowConfiguration)
{
#ifndef _RETAIL
	myImguiInterFace = std::make_unique< ImGuiInterface>();
#endif // !_RETAIL
}

Engine::~Engine() {}

bool Engine::InternalStart()
{
	myWindow = std::make_unique<ApplicationWindow>();
	if (!myWindow->Init(myWindowConfiguration, myWindowConfiguration.myHInstance, myWindowConfiguration.myHwnd))
	{
		LogError("Window failed to be created!");
		return false;
	}
	myGraphicsEngine = GraphicsEngine::GetInstance();
	if (!myGraphicsEngine->Initialize(myWindow->GetWidth(), myWindow->GetHeight(), myWindow->GetWindowHandle()))
	{
		LogError("GraphicsEngine failed to be created!");
		myWindow->Close();
		return false;
	}
	myTimer = std::make_unique<Timer>();
	if (myWindowConfiguration.myStartInFullScreen)
	{
		SetFullScreen(true);
	}
#ifdef _DEBUG
	//imgui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(myWindow->GetWindowHandle());
	ImGui_ImplDX11_Init(myGraphicsEngine->GetDevice().Get(), myGraphicsEngine->GetContext());

#endif // DEBUG

#ifndef _RETAIL
	myImguiInterFace->Init();
#endif // !_RETAIL

	return true;
}

Engine* Engine::GetInstance()
{
	if (myInstance == nullptr)
	{
		LogError("Error, forgot to create engine");
	}
	return myInstance;
}

void Engine::ShutdownEngine()
{
	GraphicsEngine::GetInstance()->DestroyInstance();
	if (myInstance)
	{
		DestroyInstance();
	}
}

void Engine::SetResolution(const Vector2<int>& aResolution, bool aAlsoSetWindowSize, bool aAlsoUpdateGraphicsResolution)
{
	myWindowConfiguration.myWindowSize = aResolution;
	if (aAlsoSetWindowSize)
	{
		myWindow->SetResolution(aResolution);
	}
	if (aAlsoUpdateGraphicsResolution)
	{
		myGraphicsEngine->Resize(aResolution);
	}
	myGraphicsEngine->myBackBuffer.SetAsActiveTarget(&myGraphicsEngine->myDepthBuffer);
}

void Engine::SetFullScreen(bool aFullScreen)
{
	myFullScreen = aFullScreen;
	if (myGraphicsEngine)
	{
		myGraphicsEngine->SetFullScreen(aFullScreen);
	}
}

bool Engine::StartEngine(const EngineConfiguration& aWindowConfiguration)
{
	if (!myInstance)
	{
		myInstance = new Engine(aWindowConfiguration);
		return myInstance->InternalStart();
	}
	return true;
}



bool Engine::BeginFrame()
{
	if (myShouldExit) { return false; }

	MSG msg = { 0 };
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		if (msg.message == WM_QUIT)
		{
			myShouldExit = true;
			return false;
		}
	}

	myGraphicsEngine->BeginFrame();



#ifdef _DEBUG
	// TODO: ImgUI stuff here
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	//if (ImGui::Begin("Debug Master"))
	//{
	//	if (ImGui::CollapsingHeader("Screen Settings"))
	//	{
	//		if (ImGui::TreeNode("Bools"))
	//		{
	//			ImGui::Columns();
	//			if (ImGui::Checkbox("Fullscreen", &myFullScreen))
	//			{
	//				SetFullScreen();
	//			}
	//			ImGui::TreePop();
	//		}
	//	}
	//}
	//ImGui::End();
#endif // DEBUG
	return true;
}


void Engine::EndFrame()
{
#ifdef _DEBUG
	//imgui
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif // DEBUG
	//#ifndef _RETAIL
	//	myImguiInterFace->Render();
	//#endif // !_RETAIL

	myTimer->Update();
	myGraphicsEngine->EndFrame();

	if (myWantToUpdateSize)
	{
		UpdateWindowSizeChanges();
		myWantToUpdateSize = false;
	}
}

void Engine::DestroyInstance()
{
	delete myInstance;
	myInstance = nullptr;
}

void GetDesktopResolution(int& horizontal, int& vertical, HWND aHwnd)
{
	RECT r;
	GetClientRect(aHwnd, &r); //get window rect of control relative to screen
	horizontal = r.right - r.left;
	vertical = r.bottom - r.top;
}

void Engine::UpdateWindowSizeChanges(bool aIgnoreAutoUpdate)
{
	if (aIgnoreAutoUpdate)
	{
		int horizontal = 0;
		int vertical = 0;
		GetDesktopResolution(horizontal, vertical, *myWindowConfiguration.myHwnd);

		// Both these values must be your real window size, so of course these values can't be static
		float screen_width = (float)horizontal;
		float screen_height = (float)vertical;

		// This is your target virtual resolution for the game, the size you built your game to
		auto& targetsize = myWindowConfiguration.myTargetSize;
		float virtual_width = (float)targetsize.x;
		float  virtual_height = (float)targetsize.y;

		float targetAspectRatio = virtual_width / virtual_height;

		// figure out the largest area that fits in this resolution at the desired aspect ratio
		float  width = screen_width;
		float  height = (float)(width / targetAspectRatio);

		if (height > screen_height)
		{
			//It doesn't fit our height, we must switch to pillarbox then
			height = screen_height;
			width = (float)(height * targetAspectRatio);
		}

		// set up the new viewport centered in the backbuffer
		float  vp_x = (screen_width / 2.0f) - (width / 2.0f);
		float  vp_y = (screen_height / 2.0f) - (height / 2.0f);

		static float lastWidth = 0.0f;
		static float lastHeight = 0.0f;

		{
			if (myWindowConfiguration.myTargetSize.x != width || myWindowConfiguration.myTargetSize.y != height)
			{
				LogError("Warning: Window is resized, it is not matching your native target resolution, there might be flickering on sprites that are close to one another");
			}

			lastWidth = width;
			lastHeight = height;
			SetViewPort(vp_x, vp_y, width, height, 0, 1, true);
		}

	}
	else
	{
		int horizontal = 0;
		int vertical = 0;
		GetDesktopResolution(horizontal, vertical, *myWindowConfiguration.myHwnd);
		Vector2<int> size = Vector2<int>(horizontal, vertical);
		myScaleRes = Vector2f((float)size.x, (float)size.y);
		SetResolution(size, false, false);
		SetViewPort(
			0, 0,
			static_cast<float>(size.x),
			static_cast<float>(size.y)
		);
	}
}

void Engine::SetViewPort(float aTopLeftX, float aTopLeftY, float aWidth, float aHeight, float aMinDepth, float aMaxDepth, bool aSetEngineResolution)
{
	if (myGraphicsEngine)
	{
		if (aSetEngineResolution)
		{
			SetResolution(Vector2<int>(static_cast<int>(aWidth), static_cast<int>(aHeight)), false);
		}

		D3D11_VIEWPORT viewport = {};
		viewport.TopLeftX = aTopLeftX;
		viewport.TopLeftY = aTopLeftY;
		viewport.Width = aWidth;
		viewport.Height = aHeight;
		viewport.MinDepth = aMinDepth;
		viewport.MaxDepth = aMaxDepth;
		myGraphicsEngine->myContext->RSSetViewports(1, &viewport);
	}
}