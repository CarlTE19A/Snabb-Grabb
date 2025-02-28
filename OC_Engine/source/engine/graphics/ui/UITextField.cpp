#include "stdafx.h"
#include "UITextField.h"
#include <Windows.h>
#include <iostream>
#include "../GraphicsEngine.h"
#include <Engine.h>

#include <d3d11.h>
#include <ApplicationWindow\ApplicationWindow.h>
void UITextField::Update()
{
	if (myIsActive)
	{
		if (GetAsyncKeyState(VK_LBUTTON) & 0x01)
		{
			myIsCenterOfAttention = IsSelected();
		}
	}
}


void UITextField::Input(Message aMessage)
{
	if (!myIsActive) return;
	if (myIsCenterOfAttention)
	{
		char key = std::any_cast<char>(aMessage.myData);

		if (key == 8)
		{
			if (myText.GetText().size() > 0)
			{
				std::string text = myText.GetText();
				text.pop_back();
				myText.SetText(text);
			}
		}
		else
		{
			std::string text = myText.GetText();
			text += key;
			myText.SetText(text);
		}
	}
}

bool UITextField::IsSelected()
{
	POINT mousePos;
	GetCursorPos(&mousePos);
	//ScreenToClient(GraphicsEngine::GetInstance()->GetWindowHandle(), &mousePos);
	ScreenToClient(Engine::GetInstance()->GetApplicationWindow().GetWindowHandle(), &mousePos);
	Vector2f pos = mySpritePtr->GetSprite().GetPosition();
	RECT rec = mySpritePtr->GetSprite().GetRect();
	Vector2f size = mySpritePtr->GetSprite().GetSize();
	Vector2f res;
	if (Engine::GetInstance()->GetFullScreen())
	{
		//res = Vector2f((float)Engine::GetInstance()->GetWindowSize().x, (float)Engine::GetInstance()->GetWindowSize().y);
		res = Engine::GetInstance()->myScaleRes;
	}
	else
	{
		res = GraphicsEngine::GetInstance()->GetViewportDimensions();
	}
	/*Vector2f size = (mySpritePtr->GetSprite().GetSize().x / myScaleSize.x, mySpritePtr->GetSprite().GetSize().y / myScaleSize.y) * res;
	Vector2f pos = (mySpritePtr->GetSprite().GetPosition().x / myScaleSize.x, mySpritePtr->GetSprite().GetPosition().y / myScaleSize.y) * res;*/
	float left = ((pos.x - (rec.right / 2 * size.x)) / myScaleSize.x) * res.x;
	float right = ((pos.x + (rec.right / 2 * size.x)) / myScaleSize.x) * res.x;
	float top = ((pos.y - (rec.bottom / 2 * size.y)) / myScaleSize.y) * res.y;
	float bottom = ((pos.y + (rec.bottom / 2 * size.y)) / myScaleSize.y) * res.y;

	// Check if the mouse position is inside the button's bounding box
	return (mousePos.x >= left && mousePos.x <= right && mousePos.y >= top && mousePos.y <= bottom);
}
