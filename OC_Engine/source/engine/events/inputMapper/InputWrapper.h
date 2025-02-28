#pragma once
#include <bitset>
#include <Windows.h>

class InputWrapper
{
public:
	InputWrapper();
	InputWrapper(HWND aWindowHandle);

	void Init(HWND aWindowHandle);

	bool IsKeyHeld(const int& aKeyCode) const;
	bool IsKeyPressed(const int& aKeyCode) const;
	bool IsKeyReleased(const int& aKeyCode) const;

	POINT GetMouseDelta() const;
	POINT GetMousePosition() const;

	void SetFocus(const bool aShow, const bool aCaptured);

	void ShowMouse() const;
	void HideMouse() const;

	void CaptureMouse() const;
	void ReleaseMouse() const;

	void LockMousePosition();
	void UnlockMousePosition();

	inline bool GetWindowFocus() { return myWindowFocusState[1]; }
	inline bool GetBeginWindowFocus() { return myWindowFocusState[1] && !myWindowFocusState[0]; }
	inline bool GetEndWindowFocus() { return !myWindowFocusState[1] && myWindowFocusState[0]; }

	float GetMouseWheelDelta() const;

	LPARAM UpdateEvents(UINT message, WPARAM wParam, LPARAM lParam);
	void Update();

private:

	HWND myOwnersHWND;

	std::bitset<256> myTentativeState{};
	std::bitset<256> myPreviousState{};
	std::bitset<256> myCurrentState{};

	POINT myTentativeMousePosition{};
	POINT myPreviousMousePosition{};
	POINT myCurrentMousePosition{};

	POINT myMouseDelta{};

	float myTentativeMouseWheelDelta{};
	float myMouseWheelDelta{};

	bool myIsMouseLocked = false;
	
	//bool myTentativeIsWindowFocused = false;
	//bool myPreviousIsWindowFocused = false;
	//bool myIsWindowFocused = false;
	//previous, current, tentative
	std::bitset<3> myWindowFocusState{1};
};