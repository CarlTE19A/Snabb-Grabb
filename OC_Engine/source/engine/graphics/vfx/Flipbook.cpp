#include "stdafx.h"
//
//#include "Flipbook.h"
//#include "../GraphicsEngine.h"
//#include "component/CameraComponent.h"
//#include "graphics\sprite\SpriteDrawer.h"
//
//
//Flipbook::~Flipbook()
//{
//}
//
//bool Flipbook::Init(VFXName aName, std::string aTexturePath, int aRow, int aCol, int aMissing, VFXType aType)
//{
//	SetSprite(aTexturePath);
//	SetName(aName);
//	SetType(aType);
//	SetIsPlaying(true);
//	SetTimer(0.0f);
//	SetIntensity(1.0f);
//	myRow = aRow;
//	myCol = aCol;
//	myMissing = aMissing;
//	myIndex = 0;
//	const float addingUVX = 1.0f / aRow;
//	const float addingUVY = 1.0f / aCol;
//	for (int j = 0; j < 4; j++)
//	{
//		for (int i = 0; i < 4; i++)
//		{
//			if (j == aCol - 1 && i > aRow - aMissing)
//			{
//				break;
//			}
//			myUvs.push_back(UV({ addingUVX * i, addingUVY * j }, { (addingUVX * i) + addingUVX, (addingUVY * j) + addingUVY }));
//		}
//	}
//	GetSprite().GetTextureRect() = { myUvs[0].myStart.x, myUvs[0].myStart.y, myUvs[0].myEnd.x, myUvs[0].myEnd.y };
//	myTextureRect = GetSprite().GetTextureRect();
//	GetSprite().SetCharacterUVFlipbook(myTextureRect);
//	return true;
//}
//
//bool Flipbook::Init(VFXName /*aName*/, std::string /*aModelPath*/, float /*aTime*/, VFXType /*aType*/, bool /*aAnimated*/ )
//{
//	return false;
//}
//
//void Flipbook::Update(float aDeltaTime)
//{
//	if (!GetIsPlaying()) return;
//
//	SetTimer(GetTimer() + aDeltaTime);
//
//	if (GetTimer() >= 0.05f)
//	{
//		myIndex++;
//		if (myIndex > myCol * myRow -1 - myMissing)
//		{
//			myIndex = 0;
//		}
//		SetTimer(0.0f);
//	}
//
//	GetSprite().GetTextureRect() = {myUvs[myIndex].myStart.x, myUvs[myIndex].myStart.y, myUvs[myIndex].myEnd.x, myUvs[myIndex].myEnd.y};
//	GetSprite().SetCharacterUVFlipbook(myTextureRect);
//
//	if (myIndex == myCol * myRow - 1 - myMissing && !GetLoop())
//	{
//		SetIsPlaying(false);
//	}
//}
//
//void Flipbook::Play()
//{
//	SetIsPlaying(true);
//	myIndex = 0;
//	SetTimer(0.0f);
//}