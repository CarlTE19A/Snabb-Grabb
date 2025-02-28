#include "MainSingelton.h"
#include "stdafx.h"

#include "MainSingelton.h"
#include "events\EventSystem.h"
#include "events\inputMapper\InputMapper.h"
#include "Sound\SoundManager.h"
#include "tools\lineDrawer\LineDrawer.h"
#include "scene\SceneManager.h"
#include "graphics\model\ModelFactory.h"
#include "PhysXCode\PhysXManager.h"
#include "../game/EnemyComponent/EnemyPool.h"
#include "../game/Ammo/AmmoPool.h"
#include "../game/Explosion/ExplosionPool.h"
#include "../game/PollingStation.h"
#include "../game/GameManager/GameManager.h"
#include <assert.h>
#include "ParticleSystem\ParticleCache.h"

MainSingleton* MainSingleton::myInstance = nullptr;

void MainSingleton::Create()
{
	assert(myInstance == nullptr && "MainSingleton already created!");
	myInstance = new MainSingleton();
}

MainSingleton& MainSingleton::GetInstance()
{
	assert(myInstance != nullptr && "Main singleton not created yet!");
	return *myInstance;
}

bool MainSingleton::IsInitiated()
{
	return myInstance;
}

void MainSingleton::Init()
{
	myEventSystem = new EventSystem();
	myInputMapper = new InputMapper();
	mySoundManager = new SoundManager();
	mySceneManager = new SceneManager();
	myLineDrawer = new LineDrawer();
	myModelFactory = new ModelFactory();
	myPhysXManager = new PhysXManager();
	myPollingStation = new PollingStation();
	myGameManager = new GameManager();
	myEnemyPool = nullptr;
	myAmmoPool = nullptr;
	myExplosionPool = nullptr;
	myPhysXManager->Initialize();
	myLineDrawer->Init();
	myGameManager->Init();
	myParticleCache = new ParticleCache();
	myParticleCache->Init();
}

void MainSingleton::CreatePool()
{
	myEnemyPool = new EnemyPool(myPollingStation);
	myAmmoPool = new AmmoPool();
	myExplosionPool = new ExplosionPool();
	myPollingStation->AddAmmoPool(myAmmoPool);
}

void MainSingleton::Destroy()
{
	assert(myInstance != nullptr && "No main singleton exists!");
	delete myInstance;
	myInstance = nullptr;
}





InputMapper& MainSingleton::GetInputMapper()
{
	assert(myInputMapper != nullptr && "MainSingleton not initiated yet!");
	return *myInputMapper;
}

SoundManager& MainSingleton::GetSoundManager()
{
	return *mySoundManager;
}

EventSystem& MainSingleton::GetEventSystem()
{
	return *myEventSystem;
}

LineDrawer& MainSingleton::GetLineDrawer()
{
	return *myLineDrawer;
}
SceneManager& MainSingleton::GetSceneManager()
{
	return *mySceneManager;
}

ModelFactory& MainSingleton::GetModelFactory()
{
	return *myModelFactory;
}

PhysXManager& MainSingleton::GetPhysXManager()
{
	return *myPhysXManager;
}

EnemyPool& MainSingleton::GetEnemyPool()
{
	return *myEnemyPool;
}

PollingStation& MainSingleton::GetPollingStation()
{
	return *myPollingStation;
}

ParticleCache& MainSingleton::GetParticleCache()
{
	return *myParticleCache;
}

ExplosionPool& MainSingleton::GetExplosionPool()
{
	return *myExplosionPool;
}

GameManager& MainSingleton::GetGameManager()
{
	return *myGameManager;
}

MainSingleton::MainSingleton()
	:
	myInputMapper(nullptr)
	, myEventSystem(nullptr)
	, mySoundManager(nullptr)
	, myLineDrawer(nullptr)
	, mySceneManager(nullptr)
	, myModelFactory(nullptr)
	, myPhysXManager(nullptr)
	, myEnemyPool(nullptr)
	, myAmmoPool(nullptr)
	, myExplosionPool(nullptr)
	, myPollingStation(nullptr)
	, myGameManager(nullptr)
{}

MainSingleton::~MainSingleton()
{
	delete mySceneManager;
	mySceneManager = nullptr;

	delete myInputMapper;
	myInputMapper = nullptr;

	delete myEventSystem;
	myEventSystem = nullptr;

	delete mySoundManager;
	mySoundManager = nullptr;

	delete myLineDrawer;
	myLineDrawer = nullptr;

	delete myModelFactory;
	myModelFactory = nullptr;

	delete myPhysXManager;
	myPhysXManager = nullptr;

	delete myEnemyPool;
	myEnemyPool = nullptr;

	delete myAmmoPool;
	myAmmoPool = nullptr;

	delete myExplosionPool;
	myExplosionPool = nullptr;

	delete myPollingStation;
	myPollingStation = nullptr;

	delete myGameManager;
	myGameManager = nullptr;
}