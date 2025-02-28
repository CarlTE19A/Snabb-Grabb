#pragma once

class InputMapper;
class SoundManager;
class LineDrawer;
class EventSystem;
class SceneManager;
class ModelFactory;
class PhysXManager;
class EnemyPool;
class AmmoPool;
class ExplosionPool;
class PollingStation;
class GameManager;
class ParticleCache;

class MainSingleton
{
public:
	static void	Create();
	static MainSingleton& GetInstance();
	static bool IsInitiated();
	void Init();
	void CreatePool();
	void Destroy();
	InputMapper& GetInputMapper();
	SoundManager& GetSoundManager();
	EventSystem& GetEventSystem();
	LineDrawer& GetLineDrawer();
	SceneManager& GetSceneManager();
	ModelFactory& GetModelFactory();
	PhysXManager& GetPhysXManager();
	EnemyPool& GetEnemyPool();
	PollingStation& GetPollingStation();
	ParticleCache& GetParticleCache();
	//AmmoPool& GetAmmoPool();
	ExplosionPool& GetExplosionPool();
	GameManager& GetGameManager();

private:
	MainSingleton();
	~MainSingleton();

	static MainSingleton* myInstance;
	InputMapper* myInputMapper;
	SoundManager* mySoundManager;
	EventSystem* myEventSystem;
	LineDrawer* myLineDrawer;
	SceneManager* mySceneManager;
	ModelFactory* myModelFactory;
	PhysXManager* myPhysXManager;
	EnemyPool* myEnemyPool;
	ParticleCache* myParticleCache;
	//Den Används i PollingStation;
	AmmoPool* myAmmoPool;
	ExplosionPool* myExplosionPool;
	PollingStation* myPollingStation;
	GameManager* myGameManager;
};