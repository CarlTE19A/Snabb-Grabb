
#include "stdafx.h"

#include "InputWrapper.h"

#include <cassert>
#include <Windowsx.h>

#include "mainSingelton/MainSingelton.h"
#include "events/EventSystem.h"

InputWrapper::InputWrapper()
{
	myWindowFocusState[2] = 1;
}

InputWrapper::InputWrapper(HWND aWindowHandle)
{
	Init(aWindowHandle);
}

void InputWrapper::Init(HWND aWindowHandle)
{
	myOwnersHWND = aWindowHandle;

#ifndef HID_USAGE_PAGE_GENERIC
#define HID_USAGE_PAGE_GENERIC		((USHORT) 0x01)
#endif // !HID_USAGE_PAGE_GENERIC
#ifndef HID_USAGE_GENERIC_MOUSE
#define HID_USAGE_GENERIC_MOUSE		((USHORT) 0x02)
#endif // !HID_USAGE_GENERIC_MOUSE


	RAWINPUTDEVICE rid[1];
	rid[0].usUsage = HID_USAGE_PAGE_GENERIC;
	rid[0].usUsage = HID_USAGE_GENERIC_MOUSE;
	rid[0].dwFlags = RIDEV_INPUTSINK;
	rid[0].hwndTarget = myOwnersHWND;
	RegisterRawInputDevices(rid, 1, sizeof(rid[0]));

	myTentativeMousePosition = { 0,0 };
	myPreviousMousePosition = { 0,0 };
	myCurrentMousePosition = { 0,0 };
	myMouseDelta = { 0,0 };

	myTentativeMouseWheelDelta = 0;
	myMouseWheelDelta = 0;
}

bool InputWrapper::IsKeyHeld(const int& aKeyCode) const
{
	return myCurrentState[aKeyCode] && myPreviousState[aKeyCode];
}

bool InputWrapper::IsKeyPressed(const int& aKeyCode) const
{
	return myCurrentState[aKeyCode] && !myPreviousState[aKeyCode];
}

bool InputWrapper::IsKeyReleased(const int& aKeyCode) const
{
	return !myCurrentState[aKeyCode] && myPreviousState[aKeyCode];
}

POINT InputWrapper::GetMouseDelta() const
{
	//Log("mDel:" + std::to_string(myMouseDelta.x)+ ", " + std::to_string(myMouseDelta.y) + 
	//	" |mPosTen: " + std::to_string(myTentativeMousePosition.x) + ", " + std::to_string(myTentativeMousePosition.y) +
	//	" |mPosCur: " + std::to_string(myCurrentMousePosition.x) + ", " + std::to_string(myCurrentMousePosition.y) +
	//	" |mPosPre: " + std::to_string(myPreviousMousePosition.x) + ", " + std::to_string(myPreviousMousePosition.y));
	return myMouseDelta;
}

POINT InputWrapper::GetMousePosition() const
{
	return myCurrentMousePosition;
}

void InputWrapper::SetFocus(const bool aShow, const bool aCaptured)
{
	if (aShow)
	{
		ShowMouse();
		UnlockMousePosition();
	}
	else
	{
		HideMouse();
		LockMousePosition();
	}
	if (aCaptured)
	{
		CaptureMouse();
	}
	else
	{
		ReleaseMouse();
	}
}

void InputWrapper::ShowMouse() const
{
	while (ShowCursor(true) < 0);
}

void InputWrapper::HideMouse() const
{
	while (ShowCursor(false) >= 0);
}

void InputWrapper::CaptureMouse() const
{
	RECT clipRect;

	GetClientRect(myOwnersHWND, &clipRect);

	POINT upperLeft;
	upperLeft.x = clipRect.left;
	upperLeft.y = clipRect.top;

	POINT lowerRight;
	lowerRight.x = clipRect.right;
	lowerRight.y = clipRect.bottom;

	MapWindowPoints(myOwnersHWND, nullptr, &upperLeft, 1);
	MapWindowPoints(myOwnersHWND, nullptr, &lowerRight, 1);

	clipRect.left = upperLeft.x;
	clipRect.top = upperLeft.y;
	clipRect.right = lowerRight.x;
	clipRect.bottom = lowerRight.y;

	ClipCursor(&clipRect);
}

void InputWrapper::ReleaseMouse() const
{
	ClipCursor(nullptr);
}

void InputWrapper::LockMousePosition() 
{
	myIsMouseLocked = true;
	POINT center;
	center.x = (GetSystemMetrics(SM_CXSCREEN) / 2);
	center.y = (GetSystemMetrics(SM_CYSCREEN) / 2);

	myPreviousMousePosition = center;
	myCurrentMousePosition = center;
	myTentativeMousePosition = center;
	ClientToScreen(myOwnersHWND, &center);
	SetCursorPos(center.x, center.y);
}

void InputWrapper::UnlockMousePosition()
{
	myCurrentMousePosition = myTentativeMousePosition;
	myIsMouseLocked = false;
}

float InputWrapper::GetMouseWheelDelta() const
{
	return myMouseWheelDelta;
}

LPARAM InputWrapper::UpdateEvents(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_KEYDOWN:
		myTentativeState[wParam] = true;
		break;

	case WM_KEYUP:
		myTentativeState[wParam] = false;
		break;

		// LMB
	case WM_LBUTTONDOWN:
		myTentativeState[VK_LBUTTON] = true;
		break;
	case WM_LBUTTONUP:
		myTentativeState[VK_LBUTTON] = false;
		break;

		// RMB
	case WM_RBUTTONDOWN:
		myTentativeState[VK_RBUTTON] = true;
		break;
	case WM_RBUTTONUP:
		myTentativeState[VK_RBUTTON] = false;
		break;

		// MMB
	case WM_MBUTTONDOWN:
		myTentativeState[VK_MBUTTON] = true;
		break;
	case WM_MBUTTONUP:
		myTentativeState[VK_MBUTTON] = false;
		break;

		// System Keys:
	case WM_SYSKEYDOWN:
		myTentativeState[wParam] = true;
		break;
	case WM_SYSKEYUP:
		myTentativeState[wParam] = false;
		break;

		// XBTN
	case WM_XBUTTONDOWN:
		{
			if (GET_XBUTTON_WPARAM(wParam) == 1)
			{
				myTentativeState[VK_XBUTTON1] = true;
			}
			else
			{
				myTentativeState[VK_XBUTTON2] = true;
			}
			break;
		}
	case WM_XBUTTONUP:
		{
			if (GET_XBUTTON_WPARAM(wParam) == 1)
			{
				myTentativeState[VK_XBUTTON1] = false;
			}
			else
			{
				myTentativeState[VK_XBUTTON2] = false;
			}
			break;
		}

		// Mouse Wheel
	case WM_MOUSEWHEEL:
		myTentativeMouseWheelDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		break;


		// This is only used for when you want X/Y coordinates.
		// Reason being is that it's clunky to rely on for delta
		// movements of the mouse. ClipRect and SetMousePos all
		// cause their own problems for input which are easily
		// solved by registering for the raw HID data and listening
		// for WM_INPUT instead.
	case WM_MOUSEMOVE:
		{
			myTentativeMousePosition.x = GET_X_LPARAM(lParam);
			myTentativeMousePosition.y = GET_Y_LPARAM(lParam);
			break;
		}

		// Handles mouse delta, used in 3D navigation etc.
		//Does not Work, there for outcommented
	case WM_INPUT:
		{
			//UINT dwSize = sizeof(RAWINPUT);
			//static BYTE lpb[sizeof(RAWINPUT)];
			//
			//GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER));
			//
			//RAWINPUT* raw = (RAWINPUT*)lpb;
			//
			//if (raw->header.dwType == RIM_TYPEMOUSE)
			//{
			//	myTentativeMouseDelta.x = raw->data.mouse.lLastX;
			//	myTentativeMouseDelta.y = raw->data.mouse.lLastY;
			//}

			break;
		}
		case WM_SETFOCUS:
		{
			myWindowFocusState[2] = 1;
			
			break;
		}
		case WM_KILLFOCUS:
		{
			myWindowFocusState[2] = 0;
			//Log("Window Lost Focus");
			break;
		}
	default:
		return false;
	}

	return true;
}

void InputWrapper::Update()
{
	myPreviousMousePosition = myCurrentMousePosition;
	myCurrentMousePosition = myTentativeMousePosition;

	if (myIsMouseLocked)
	{
		POINT center;
		center.x = (GetSystemMetrics(SM_CXSCREEN) / 2);
		center.y = (GetSystemMetrics(SM_CYSCREEN) / 2);
		
		myPreviousMousePosition = center;
		ClientToScreen(myOwnersHWND, &center);
		SetCursorPos(center.x, center.y);
	}

	myMouseDelta = { myCurrentMousePosition.x - myPreviousMousePosition.x, myCurrentMousePosition.y - myPreviousMousePosition.y };
	//myTentativeMouseDelta = { 0, 0 };

	myMouseWheelDelta = myTentativeMouseWheelDelta / abs(myTentativeMouseWheelDelta);
	myTentativeMouseWheelDelta = 0;

	myPreviousState = myCurrentState;
	myCurrentState = myTentativeState;
	myWindowFocusState >>= 1;
	myWindowFocusState[2] = myWindowFocusState[1];
	//myWindowFocusState[0] = myWindowFocusState[1];
	//myWindowFocusState[1] = myTentativeState[2];
	//myPreviousIsWindowFocused = myIsWindowFocused;
	//myIsWindowFocused = myTentativeIsWindowFocused;
}
