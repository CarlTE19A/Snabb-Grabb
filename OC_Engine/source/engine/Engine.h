#pragma once
#define WIN32_LEAN_AND_MEAN 
#define NOMINMAX 
#include <windows.h>
#include <functional>
#include "math/Vector.h"
#include <string>
#include <memory>
class GraphicsEngine;
class Timer;

#ifndef _RETAIL
class ImGuiInterface;
#endif // !_RETAIL

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

using callback_function_wndProc = std::function<LRESULT(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)>;
using callback_function_log = std::function<void(std::string)>;
using callback_function_error = std::function<void(std::string)>;

enum class WindowSetting
{
	Overlapped,
	Borderless,
};

struct EngineConfiguration
{
	EngineConfiguration()
	{
		myHwnd = nullptr;
		myHInstance = nullptr;
		myWindowSize = { 1920, 1080 };
		myEnableVSync = false;
		myRenderSize = myWindowSize;
		//myErrorFunction = nullptr;
		//myLogFunction = nullptr;
		myWinProcCallback = nullptr;
		myStartInFullScreen = false;
		myWindowSetting = WindowSetting::Overlapped;
		myTargetSize = myWindowSize;
		//myClearColor = CLEAR_COLOR;
	}
	~EngineConfiguration()
	{
		myHwnd = nullptr;
		myHInstance = nullptr;
		myWinProcCallback = nullptr;
	}

	callback_function_wndProc myWinProcCallback;
	//callback_function_log myLogFunction;
	//callback_function_error myErrorFunction;

	/* How big should the window be? */
	Vector2<int> myWindowSize;

	/* What resolution should we render everything in?*/
	Vector2<int> myRenderSize;

	/* In what resolution are the graphic artist working?*/
	Vector2<int> myTargetSize;

	//Vector4f myClearColor;
	HWND* myHwnd;
	HINSTANCE myHInstance;
	std::wstring myApplicationName;
	bool myEnableVSync;
	bool myStartInFullScreen;
	WindowSetting myWindowSetting;
};

class Engine
{
	friend class ApplicationWindow;
public:
	Engine& operator =(const Engine& anOther) = delete;
	static Engine* GetInstance();
	
	static bool StartEngine(const EngineConfiguration& aWindowConfiguration = EngineConfiguration());
	static void ShutdownEngine();

	bool BeginFrame();
	void EndFrame();

	//remember to set aAlsoSetWindowSize to false when we are in fullscreen
	void SetResolution(const Vector2<int>& aResolution, bool aAlsoSetWindowSize = true, bool aAlsoUpdateGraphicsResolution = true);
	void SetFullScreen(bool aFullScreen);
	void SetWantToUpdateSize() { myWantToUpdateSize = true; }

	ApplicationWindow& GetApplicationWindow() const { return *myWindow; }
	Timer* GetTimer() const { return &*myTimer; }
	const bool GetFullScreen() const { return myFullScreen; }
	const Vector2<int> GetWindowSize() const { return myWindowConfiguration.myWindowSize; }
	const bool GetWantToUpdateWindowSize() { return myWantToUpdateSize; }
	void UpdateWindowSizeChanges(bool aIgnoreAutoUpdate = false);
	HWND* GetHWND()const { return myWindowConfiguration.myHwnd; }

	Vector2f myScaleRes;
private:
	Engine(const EngineConfiguration& aWindowConfiguration);
	~Engine();
	bool InternalStart();
	static void DestroyInstance();
	void SetViewPort(float aTopLeftX, float aTopLeftY, float aWidth, float aHeight, float aMinDepth = 0.0f, float aMaxDepth = 1.0f, bool aSetEngineResolution = false);

	bool myFullScreen;
	bool myWantToUpdateSize;
	static Engine* myInstance;
	std::unique_ptr<ApplicationWindow> myWindow;
	GraphicsEngine* myGraphicsEngine;
	std::unique_ptr<Timer> myTimer;
	EngineConfiguration myWindowConfiguration;
#ifndef _RETAIL
	std::unique_ptr<ImGuiInterface> myImguiInterFace = nullptr;
#endif // !_RETAIL


	bool myShouldExit;
};