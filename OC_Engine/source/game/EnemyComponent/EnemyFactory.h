#pragma once
#include "EnemyType.h"
class EnemyComponent;

class PollingStation;

class EnemyFactory
{
public:
	EnemyFactory(PollingStation* aPollingStation);
	~EnemyFactory() = default;

	EnemyComponent* CreateEnemy(EnemyType aEnemyType);

private:

	float myAttackCooldown = 5;
	float myAttackDamage = 1;
	float myMaxHealth = 100;
	float myRange = 3500;

	PollingStation* myPollingStation;
};
