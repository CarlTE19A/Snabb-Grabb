#include "stdafx.h"
//
//#include "ScrollMesh.h"
//#include "../GraphicsEngine.h"
//
//#include "mainSingelton/MainSingelton.h"
//#include "../model/ModelFactory.h"
//#include "../model/Mesh.h"
//#include "graphics\model\ModelDrawer.h"
//
//ScrollMesh::ScrollMesh()
//{
//}
//
//ScrollMesh::~ScrollMesh()
//{
//}
//
//bool ScrollMesh::Init(VFXName aName, std::string aModelPath, float aTime, VFXType aType, bool aAnimated)
//{
//	if (aAnimated)
//	{
//		SetAnimatedMeshes(MainSingleton::GetInstance().GetModelFactory().Load(aModelPath, true));
//	}
//	else
//	{
//		SetMeshes(MainSingleton::GetInstance().GetModelFactory().Load(aModelPath));
//	}
//	SetType(aType);
//	SetName(aName);
//	SetPlayTime(aTime);
//	SetTimer(aTime);
//	SetIsPlaying(true);
//	SetIntensity(1.0f);
//
//	return true;
//}
//
//bool ScrollMesh::Init(VFXName /*aName*/, std::string /*aTexturePath*/, int /*aRow*/, int /*aCol*/, int /*aMissing*/, VFXType /*aType*/)
//{
//	return false;
//}
//
//void ScrollMesh::Update(float aDeltaTime)
//{
//	if (!GetIsPlaying() && !GetLoop()) return;
//
//	SetTimer(GetTimer() - aDeltaTime);
//	if (GetTimer() <= 0)
//	{
//		if (!GetLoop()) SetIsPlaying(false);
//		else SetTimer(GetPlayTime());
//	}
//}
//
//void ScrollMesh::Play()
//{
//	SetIsPlaying(true);
//	SetTimer(GetPlayTime());
//}
