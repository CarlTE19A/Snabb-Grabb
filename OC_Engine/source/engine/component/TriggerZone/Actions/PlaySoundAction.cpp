#include "stdafx.h"
#include "PlaySoundAction.h"

#include "scene/Scene.h"


void PlaySoundAction::Activate(Scene&)
{
	LogInfo("Sound should be played here");
}

bool PlaySoundAction::Update(float deltaTime, Scene& aScene)
{
	deltaTime;
	aScene;
	//TODO Test when sound done
	return false;
}

PlaySoundAction::~PlaySoundAction()
{
}
