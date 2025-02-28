#pragma once
#define WIN32_LEAN_AND_MEAN 
#define NOMINMAX 
#include <windows.h>
#include "Engine.h"

class ApplicationWindow
{
public:
	ApplicationWindow(void);
	~ApplicationWindow(void);
	bool Init(const EngineConfiguration& aWndCfg, HINSTANCE& aHInstanceToFill, HWND*& aHwnd);
	HWND& GetWindowHandle() { return myWindowHandle; }
	void SetResolution(Vector2<int> aResolution);
	void Close();
	int GetWidth()
	{
		return myResolution.x;
	}
	int  GetHeight()
	{
		return myResolution.y;
	}

private:
	LRESULT LocWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	HWND myWindowHandle;
	WNDCLASSEX myWindowClass;
	callback_function_wndProc myWndProcCallback;
	Vector2<int> myResolution;
	Vector2<int> myResolutionWithBorderDifference;
};