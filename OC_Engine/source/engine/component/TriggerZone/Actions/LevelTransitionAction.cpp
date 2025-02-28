#include "stdafx.h"
#include "LevelTransitionAction.h"
#include "gameObject/GameObject.h"
#include "scene\SceneManager.h"
#include "mainSingelton\MainSingelton.h"

void LevelTransitionAction::Activate(Scene& /*aScene*/)
{
	ActivateKeys();
	MainSingleton::GetInstance().GetSceneManager().SetActiveScene(nextSceneName);
}

void LevelTransitionAction::Reset(){}

bool LevelTransitionAction::Update(float /*deltaTime*/, Scene& /*aScene*/)
{
	return false;
}

LevelTransitionAction::~LevelTransitionAction(){}
