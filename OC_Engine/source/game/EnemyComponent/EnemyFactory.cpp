#include "EnemyFactory.h"
#include "EnemyComponent.h"
#include "BalloonComponent.h"
#include "TurretComponent.h"
#include "../PollingStation.h"

//AI
#include "../BehaviorTree/BehaviorNode.h"
#include "../BehaviorTree/ConditionNode.h"
#include "../BehaviorTree/ActionNode.h"
#include "../BehaviorTree/SelectorNode.h"
#include "../BehaviorTree/SequenceNode.h"
#include "../BehaviorTree/RotateTowardsThePlayer.h"
#include "../BehaviorTree/CanIShoot.h"

#include "../BehaviorTree/PlayerInReachCondition.h"
#include "../BehaviorTree/ShootPlayer.h"

//Load
#include "Paths.h"
#include "nlohmann/json.hpp"
#include <fstream>


EnemyFactory::EnemyFactory(PollingStation* aPollingStation)
	:myPollingStation(aPollingStation)
{
	std::ifstream readJson;

	readJson.open(STATIC_ASSET_PATH "json/TurretsSettings.json");

	if (readJson.is_open())
	{
		nlohmann::json json;
		readJson >> json;

		myAttackCooldown = json["AttackCooldown"];
		myAttackDamage = json["AttackDamage"];
		myMaxHealth = json["MaxHealth"];
		myRange = json["Range"];
	}

	readJson.close();

}

EnemyComponent* EnemyFactory::CreateEnemy(EnemyType aEnemyType)
{
	EnemyComponent* enemy;
	BehaviorNode* aIBehavior{};
	switch (aEnemyType)
	{
	case EnemyType::eBalloon:

	{
		enemy = new BalloonComponent(myPollingStation);
		enemy->SetAttackDamage(1);
		enemy->SetAttackCooldown(3);
		enemy->SetMaxHealthPoint(100);
		enemy->SetHealthPoint(100);
		enemy->SetRange(500);
		enemy->SetPosition(Vector3f(-100, -100, -100));
		enemy->SetDirection(Vector3f(0, 0, 0));
		enemy->SetEnemyTyp(aEnemyType);

	
		/*ActionNode* nothinsAction = new ActionNode(enemy);
		ConditionNode* nothing = new ConditionNode(enemy,nothinsAction);
		aIBehavior =
			new SelectorNode(enemy, { nothinsAction });

		enemy->SetBehaviorNode(aIBehavior);*/

		break;
	}

	case EnemyType::eTurret:
	{

		enemy = new TurretComponent(myPollingStation);
		enemy->SetAttackDamage(myAttackDamage);
		enemy->SetAttackCooldown(myAttackCooldown);
		enemy->SetMaxHealthPoint(myMaxHealth);
		enemy->SetHealthPoint(myMaxHealth);
		enemy->SetRange(myRange);
		enemy->SetPosition(Vector3f(-100, -100, -100));
		enemy->SetDirection(Vector3f(0, 0, 0));
		enemy->SetEnemyTyp(aEnemyType);

		

		ActionNode* RotateTowardThePlayer = new RotateTowardsThePlayer(enemy);
		ConditionNode* playerInReach = new PlayerInReachCondition(enemy, RotateTowardThePlayer);

		ActionNode* shotPlayer = new ShootPlayer(enemy);
		ConditionNode* canIShoot = new CanIShoot(enemy, shotPlayer);

		aIBehavior = new SequenceNode(enemy, { playerInReach,canIShoot });


		enemy->SetBehaviorNode(aIBehavior);

		break;
	}
	default:
		enemy = nullptr;
		break;
	}

	return enemy;
}
