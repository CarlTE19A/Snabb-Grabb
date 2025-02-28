#include "stdafx.h"

#include "ApplicationWindow.h"

ApplicationWindow::ApplicationWindow(void)
	:myWndProcCallback(nullptr)
{}

ApplicationWindow::~ApplicationWindow(void)
{}
HCURSOR g_CustomCursor = NULL;
//HANDLE g_CustomCursorHandle = NULL;
bool ApplicationWindow::Init(const EngineConfiguration& aWindowConfig, HINSTANCE& aHInstanceToFill, HWND*& aHwnd)
{
	g_CustomCursor = reinterpret_cast<HCURSOR>(LoadImageW(NULL, L"../assets/Cursor.cur", IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE | LR_DEFAULTCOLOR));
	if (g_CustomCursor == NULL)
	{
		MessageBox(myWindowHandle, L"Failed to load cursor.", L"Error", MB_ICONERROR | MB_OK);
		// Handle the error (e.g., return or exit)
	}
	//SetClassLongPtr(myWindowHandle, GCLP_HCURSOR, (LONG_PTR)g_CustomCursor);  // new cursor 



	myWndProcCallback = aWindowConfig.myWinProcCallback;
	HINSTANCE instance = GetModuleHandle(NULL);
	aHInstanceToFill = instance;

	ZeroMemory(&myWindowClass, sizeof(WNDCLASSEX));
	myWindowClass.cbSize = sizeof(WNDCLASSEX);
	myWindowClass.style = CS_HREDRAW | CS_VREDRAW;
	myWindowClass.lpfnWndProc = WindowProc;
	myWindowClass.hInstance = instance;
	myWindowClass.hCursor = g_CustomCursor;
	myWindowClass.hbrBackground = (HBRUSH)COLOR_WINDOW;
	myWindowClass.lpszClassName = L"WindowClass1";
	//myWindowClass.hIcon = ::LoadIcon(instance, MAKEINTRESOURCE(IDI_ICON1));
	//myWindowClass.hIconSm = LoadIcon(instance, MAKEINTRESOURCE(IDI_ICON1));
	RegisterClassEx(&myWindowClass);

	const auto& windowSize = aWindowConfig.myWindowSize;

	RECT wr = { 0, 0, static_cast<long>(windowSize.x), static_cast<long>(windowSize.y) };    // set the size, but not the position
	//AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);    // adjust the size

	DWORD windowStyle = 0;
	switch (aWindowConfig.myWindowSetting)
	{
	case WindowSetting::Overlapped:
		windowStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
		break;
	case WindowSetting::Borderless:
		windowStyle = WS_VISIBLE | WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
		break;
	default:
		break;
	}

	if (!aHwnd)
	{
		myWindowHandle = CreateWindowW(
			L"WindowClass1",    // name of the window class
			aWindowConfig.myApplicationName.c_str(),    // title of the window
			windowStyle,    // window style
			CW_USEDEFAULT,    // x-position of the window
			CW_USEDEFAULT,    // y-position of the window
			wr.right - wr.left,    // width of the window
			wr.bottom - wr.top,    // height of the window
			NULL,    // we have no parent window, NULL
			NULL,    // we aren't using menus, NULL
			instance,    // application handle
			NULL);    // used with multiple windows, NULL



		ShowWindow(myWindowHandle, true);
		aHwnd = &myWindowHandle;
	}
	else
	{
		myWindowHandle = *aHwnd;
	}

	SetWindowLongPtr(myWindowHandle, GWLP_USERDATA, (LONG_PTR)this);




	// Fix to set the window to the actual resolution as the borders will mess with the resolution wanted
	myResolution = windowSize;
	myResolutionWithBorderDifference = myResolution;
	//if (aWindowConfig.myWindowSetting == WindowSetting::Overlapped)
	//{
	//	RECT r;
	//	GetClientRect(myWindowHandle, &r); //get window rect of control relative to screen
	//	int horizontal = r.right - r.left;
	//	int vertical = r.bottom - r.top;

	//	int diffX = (windowSize.x - horizontal);
	//	int diffY = windowSize.y - vertical;

	//	SetResolution(windowSize + Vector2<int>(diffX, diffY));
	//	myResolutionWithBorderDifference = windowSize + Vector2<int>(diffX, diffY);
	//}

	//g_CustomCursor = LoadCursorFromFile(L"../assets/Cursor.cur");

	return true;
}

void ApplicationWindow::SetResolution(Vector2<int> aResolution)
{
	::SetWindowPos(myWindowHandle, 0, 0, 0, aResolution.x, aResolution.y, SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);
}

void ApplicationWindow::Close()
{
	DestroyWindow(myWindowHandle);
}

#ifdef _DEBUG
//Imgui
#include <imgui\imgui_impl_win32.h>
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif // DEBUG

LRESULT ApplicationWindow::LocWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
#ifdef _DEBUG
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
		return S_OK;
#endif
	if (myWndProcCallback)
	{
		return myWndProcCallback(hWnd, message, wParam, lParam);
	}
	return S_OK;
}


LRESULT ApplicationWindow::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	ApplicationWindow* windowsClass = (ApplicationWindow*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	if (windowsClass)
	{
		LRESULT result = windowsClass->LocWindowProc(hWnd, message, wParam, lParam);
		if (result)
		{
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}

	switch (message)
	{
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	} break;

	case WM_SIZE:
	{
		if (Engine::GetInstance())
			Engine::GetInstance()->SetWantToUpdateSize();
		break;
	}
	case WM_SETCURSOR:
	{
		// Check if the cursor needs to be set to the custom cursor
		//if (LOWORD(lParam) == HTCLIENT && HIWORD(lParam) == WM_MOUSEMOVE)
		//{
		//	if (g_CustomCursor != NULL)
		//	{
		//		//SetCursor(g_CustomCursor);
		//		return TRUE; // Indicate that you've handled the message
		//	}
		//	else
		//	{
		//		MessageBox(hWnd, L"Custom cursor not loaded.", L"Error", MB_ICONERROR | MB_OK);
		//		// Handle the error (e.g., return or exit)
		//	}
		//}

		break;
	}
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}
