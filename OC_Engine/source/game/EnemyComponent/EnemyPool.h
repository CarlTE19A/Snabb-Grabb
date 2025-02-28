#pragma once
#include <vector>
#include <map>
#include <string>
#include "EnemyInfo.h"
constexpr unsigned BALLOONE_ENEMY_POOL_SIZE = 25;
constexpr unsigned TURRET_ENEMY_POOL_SIZE = 25;

class EnemyComponent;
class PollingStation;
class GameObject;
class AnimatedMesh;
class Mesh;

struct Enemy
{
	bool myEnemyIsAvailaible = true;
	GameObject* myEnemy;
	~Enemy();
};

class EnemyPool
{
public:
	EnemyPool(PollingStation* aPollingstation);
	~EnemyPool();

	void Init();
	GameObject* GetEnemyType(const EnemyInfo aEnemyInfo);

	void ReturnEnemy(const EnemyInfo aEnemyType, const int aPoolIndex);

	void NewStatesForTheTurret(const float aMaxHealth, const float aHealthPoint, const float aAttackCooldown, const float aAttackDamae, const float aRange);

private:

	std::vector<GameObject*> CreateTurret(std::vector<Mesh*> aMesh, std::string aPath, GameObject* aParent);
	//std::map<EnemyType, std::vector<Enemy>>myEnemies;
	std::map<EnemyType, std::map<CardTypeEnum, std::vector<Enemy>>> myEnemies;
};
