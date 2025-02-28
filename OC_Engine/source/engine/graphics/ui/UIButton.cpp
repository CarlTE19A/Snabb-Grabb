#include "stdafx.h"
#include "UIButton.h"
#include "graphics\sprite\SpriteDrawer.h"
#include <Windows.h>
#include <iostream>
#include "../GraphicsEngine.h"
#include <Engine.h>

#include <d3d11.h>
#include <ApplicationWindow\ApplicationWindow.h>


void UIButton::Update()
{
	myIsHighlighted = IsSelected();
	Vector4f color = Vector4f(0.2f, 0.2f, 1.f, 0.5f);
	if (myIsHighlighted)
	{
		//mySpritePtr->GetSprite().SetColor(color);
	}
	else
	{
		//color = Vector4f(1.f, 1.f, 1.f, 1.f);
		//mySpritePtr->GetSprite().SetColor(color);
	}
}

void UIButton::Render()
{
	if (myShouldRender)
	{
		if (myIsHighlighted)
		{
			mySpritePtr->SetActive(false);
			mySprites[0]->Draw();
		}
		else
		{
			mySpritePtr->SetActive(true);
		}
	}
	//do nothing for now
}

bool UIButton::IsSelected()
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
