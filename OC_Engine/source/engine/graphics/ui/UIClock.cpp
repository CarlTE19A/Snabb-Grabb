#include "stdafx.h"
//#include "UIClock.h"
//#include "UISprite.h"
////#include "../spirte/Sprite.h"
//
//bool UIClock::Init(const std::string aTexturePath)
//{
//	myIsOnCooldown = false;
//	for (size_t i = 0; i < 4; i++)
//	{
//		UISprite* sprite = new UISprite();
//		//if ((sprite->Init(aTexturePath)))
//		//{
//		//	mySprites.push_back(sprite);
//		//	myTextureRects.push_back(sprite->GetSprite().GetTextureRect());
//		//}
//		//else return false;
//	}
//	myValue = 0;
//	return true;
//}
//
//void UIClock::Update()
//{
//	if (!myIsOnCooldown) return;
//	if (myValue <= 1)
//	{
//		float m = 8; // Slope
//		float a = m * myValue;
//
//		if (myValue <= 0.125) //0x
//		{
//			CropSprite(Vector2f(0.0f, 0.0f), Vector2f(a, 0.0f));
//		}
//		else if (myValue <= 0.25f)//0y
//		{
//			CropSprite(Vector2f(0.0f, 0.0f), Vector2f(1, a - 1));
//		}
//		else if (myValue <= 0.375f)//1y
//		{
//			CropSprite(Vector2f(0.0f, 0.0f), Vector2f(0, a - 2));
//		}
//		else if (myValue <= 0.5f)//1x
//		{
//			CropSprite(Vector2f(1 - (a - 3), 0.0f), Vector2f(0, 1));
//		}
//		else if (myValue <= 0.625f)//2x
//		{
//			CropSprite(Vector2f(1 - (a - 4), 0.0f), Vector2f(0, 0.0f));
//		}
//		else if (myValue <= 0.75f)//2y
//		{
//			CropSprite(Vector2f(0.0001f, 1 - (a - 5)), Vector2f(0, 0.0f));
//		}
//		else if (myValue <= 0.875f)//3y
//		{
//			CropSprite(Vector2f(0, 1 - (a - 6)), Vector2f(0, 0.0f));
//		}
//		else if (myValue <= 1.f)//3x
//		{
//			CropSprite(Vector2f(0, 0.0001f), Vector2f(1 - (a - 7), 0));
//		}
//	}
//	else
//	{
//		myIsOnCooldown = false;
//	}
//}
//
//void UIClock::UpdateValue(float aValue)
//{
//	if (myIsOnCooldown)
//	{
//		myValue += aValue / myCooldownTime;
//	}	
//}
//
//void UIClock::SetPosition(const Vector3f& aPosition)
//{
//	aPosition;
//	//TODO: fix this, hårdkodat just nu, och tar inte hänsyn till storlek samt scale :c
//	
//	//float scale = mySprites[0]->GetScale().x;
//
//	//Vector3f test = Vector3f(aPosition.x * scale, aPosition.y * scale, 0);
//	//
//	//mySprites[0]->SetPosition(Vector3f(test.x, test.y, 0));
//	//test = Vector3f(aPosition.x * scale, aPosition.y * -scale, 0);
//
//	for (size_t i = 0; i < mySprites.size(); i++)
//	{
//		//mySprites[i]->SetAlpha(0.5f);
//		//mySprites[i]->SetColor(Vector4f(0.15f, 0.1f, 0.1f, 0.5f));
//	}
//
//	float scalex = 31.55f;
//	float scaley = 17.78f;
//	//mySprites[1]->SetPosition(Vector3f(test.x, test.y, 0));
//	//mySprites[0]->SetPosition(Vector3f(aPosition.x + scalex, aPosition.y - scaley, 0));
//	//mySprites[1]->SetPosition(Vector3f(aPosition.x + scalex, aPosition.y + scaley, 0));
//	//mySprites[2]->SetPosition(Vector3f(aPosition.x - scalex, aPosition.y + scaley, 0));
//	//mySprites[3]->SetPosition(Vector3f(aPosition.x - scalex, aPosition.y - scaley, 0));
//}
//
//void UIClock::Draw()
//{
//	if (!myIsOnCooldown) return;
//	for (size_t i = 0; i < 4; i++)
//	{
//		if (myValue >= .25 * (i + 1))
//		{
//			continue;
//		}
//		//GraphicsEngine::GetInstance()->GetSpriteDrawer().DrawSprite(mySprites[i]->GetSprite());
//	}
//}
//
//void UIClock::SetScale(const Vector2f& aScale)
//{
//	for (size_t i = 0; i < 4; i++)
//	{
//		//mySprites[i]->SetScale(aScale);
//	}
//}
//
//void UIClock::CropSprite(Vector2f aCropEnd, Vector2f aCropStart, bool /*setUV*/)
//{/*
//	TextureRect test;
//	int a = 0;
//	if (myValue <= 0.25f)
//	{
//		test = mySprites[0]->GetSprite().GetTextureRect();
//	}
//	else if (myValue <= 0.5f)
//	{
//		a = 1;
//		test = mySprites[1]->GetSprite().GetTextureRect();
//	}
//	else if (myValue <= 0.75)
//	{
//		a = 2;
//		test = mySprites[2]->GetSprite().GetTextureRect();
//	}
//	else
//	{
//		a = 3;
//		test = mySprites[3]->GetSprite().GetTextureRect();
//	}
//
//	if (aCropEnd.x == 0)
//	{
//		aCropEnd.x = 1;
//	}
//	if (aCropEnd.y == 0)
//	{
//		aCropEnd.y = 1;
//	}
//
//	test.myEndX = aCropEnd.x;
//	test.myEndY = aCropEnd.y;
//	test.myStartX = aCropStart.x;
//	test.myStartY = aCropStart.y;
//	mySprites[a]->GetSprite().SetTexturRect(&test);
//	mySprites[a]->GetSprite().SetCharacterUVClock(a == 0 || a == 2);*/
//}
//
//void UIClock::Reset()
//{
//	myValue = 0;
//	for (size_t i = 0; i < mySprites.size(); i++)
//	{
//		mySprites[i]->GetSprite().SetTexturRect(&myTextureRects[i]);
//		mySprites[i]->GetSprite().SetCharacterUV();
//	}
//}
