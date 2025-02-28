#include "EnemyPool.h"
#include "EnemyFactory.h"
#include "EnemyType.h"
#include "EnemyComponent.h"
#include "gameObject/GameObject.h"
#include "graphics/model/ModelFactory.h"
#include "mainSingelton/MainSingelton.h"
#include "EnemyInfo.h"
#include "Paths.h"
#include "component/mesh/AnimatedMeshComponent.h"
#include "component/mesh/MeshComponent.h"
#include "math/Transform.h"
#include "PhysXCode/PhysXManager.h"
#include <component/PhysicsComponent.h>
Enemy::~Enemy()
{
	for (size_t i = 0; i < myEnemy->GetChildren().size(); i++)
	{
		delete  myEnemy->GetChildren()[i];
		myEnemy->GetChildren()[i] = nullptr;
	}
	myEnemy->GetChildren().clear();
	delete myEnemy;
	myEnemy = nullptr;
}

EnemyPool::EnemyPool(PollingStation* aPollingStation)
{
	EnemyFactory factory(aPollingStation);
	ModelFactory& modelFactory = MainSingleton::GetInstance().GetModelFactory();

	std::string defaultBalloonPath = RELATIVE_ASSET_ASSET_PATH "Balloon_Enemy/SK_balloon.fbx";
	std::string pinkBalloonPath = RELATIVE_ASSET_ASSET_PATH "Balloon_Enemy/SK_balloonpink.fbx";
	std::string orangeBalloonPath = RELATIVE_ASSET_ASSET_PATH "Balloon_Enemy/SK_balloonorange.fbx";
	std::string blueBalloonPath = RELATIVE_ASSET_ASSET_PATH "Balloon_Enemy/SK_balloonblue.fbx";

	std::string defaultTurretTopPath = RELATIVE_ASSET_ASSET_PATH "Turret_Enemy/SK_Turret.fbx";
	std::string pinkTurretTopPath = RELATIVE_ASSET_ASSET_PATH "Turret_Enemy/SK_turretpink.fbx";
	std::string orangeTurretTopPath = RELATIVE_ASSET_ASSET_PATH "Turret_Enemy/SK_turretorange.fbx";
	std::string blueTurretTopPath = RELATIVE_ASSET_ASSET_PATH "Turret_Enemy/SK_turretblue.fbx";

	std::string TurretBottomPath = RELATIVE_ASSET_ASSET_PATH "Turret_Enemy/sm_turretBase.fbx";

	myEnemies[EnemyType::eBalloon][CardTypeEnum::Sword] = std::vector<Enemy>();
	myEnemies[EnemyType::eBalloon][CardTypeEnum::DoubleJump] = std::vector<Enemy>();
	myEnemies[EnemyType::eBalloon][CardTypeEnum::Dash] = std::vector<Enemy>();
	myEnemies[EnemyType::eBalloon][CardTypeEnum::Stomp] = std::vector<Enemy>();

	myEnemies[EnemyType::eTurret][CardTypeEnum::Sword] = std::vector<Enemy>();
	myEnemies[EnemyType::eTurret][CardTypeEnum::DoubleJump] = std::vector<Enemy>();
	myEnemies[EnemyType::eTurret][CardTypeEnum::Dash] = std::vector<Enemy>();
	myEnemies[EnemyType::eTurret][CardTypeEnum::Stomp] = std::vector<Enemy>();

	myEnemies.at(EnemyType::eBalloon).at(CardTypeEnum::Sword).resize(BALLOONE_ENEMY_POOL_SIZE);
	myEnemies.at(EnemyType::eBalloon).at(CardTypeEnum::DoubleJump).resize(BALLOONE_ENEMY_POOL_SIZE);
	myEnemies.at(EnemyType::eBalloon).at(CardTypeEnum::Dash).resize(BALLOONE_ENEMY_POOL_SIZE);
	myEnemies.at(EnemyType::eBalloon).at(CardTypeEnum::Stomp).resize(BALLOONE_ENEMY_POOL_SIZE);

	myEnemies.at(EnemyType::eTurret).at(CardTypeEnum::Sword).resize(TURRET_ENEMY_POOL_SIZE);
	myEnemies.at(EnemyType::eTurret).at(CardTypeEnum::DoubleJump).resize(TURRET_ENEMY_POOL_SIZE);
	myEnemies.at(EnemyType::eTurret).at(CardTypeEnum::Dash).resize(TURRET_ENEMY_POOL_SIZE);
	myEnemies.at(EnemyType::eTurret).at(CardTypeEnum::Stomp).resize(TURRET_ENEMY_POOL_SIZE);

	//myEnemies.at(EnemyType::eBalloon).resize(BALLOONE_ENEMY_POOL_SIZE);
	//myEnemies.at(EnemyType::eTurret).resize(TURRET_ENEMY_POOL_SIZE);

	std::string idleAnimationballoonPath = RELATIVE_ASSET_ASSET_PATH "Balloon_Enemy/anim_balloonIdle.fbx";
	std::string deathAnimationballoonPath = RELATIVE_ASSET_ASSET_PATH "Balloon_Enemy/anim_balloonDeath.fbx";

	std::string idleAnimationTurretPath = RELATIVE_ASSET_ASSET_PATH "Turret_Enemy/anim_turretIdle.fbx";
	std::string windupAnimationTurretPath = RELATIVE_ASSET_ASSET_PATH "Turret_Enemy/anim_turretWindup.fbx";
	std::string shootAnimationTurretPath = RELATIVE_ASSET_ASSET_PATH "Turret_Enemy/anim_turretShoot.fbx";
	std::string deathAnimationTurretPath = RELATIVE_ASSET_ASSET_PATH "Turret_Enemy/anim_turretDeath.fbx";


	for (int j = 0; j < 4; j++)
	{

		std::vector<Enemy>* balloneColorType = {};
		std::string balloonPath;
		if (j == 0)
		{
			balloneColorType = &myEnemies.at(EnemyType::eBalloon).at(CardTypeEnum::Sword);
			balloonPath = defaultBalloonPath;
		}
		else if (j == 1)
		{
			balloneColorType = &myEnemies.at(EnemyType::eBalloon).at(CardTypeEnum::DoubleJump);
			balloonPath = orangeBalloonPath;
		}
		else if (j == 2)
		{
			balloneColorType = &myEnemies.at(EnemyType::eBalloon).at(CardTypeEnum::Dash);
			balloonPath = pinkBalloonPath;
		}
		else if (j == 3)
		{
			balloneColorType = &myEnemies.at(EnemyType::eBalloon).at(CardTypeEnum::Stomp);
			balloonPath = blueBalloonPath;
		}

		for (int i = 0; i < BALLOONE_ENEMY_POOL_SIZE; ++i)
		{
			std::vector<AnimatedMesh*> balloonMesh = modelFactory.Load(balloonPath, true);
			AnimatedMeshComponent* balloonMeshComponent = new AnimatedMeshComponent(balloonMesh);
			balloonMeshComponent->AddAnimation(idleAnimationballoonPath, false, true);
			balloonMeshComponent->AddAnimation(deathAnimationballoonPath, false, false);
			balloonMeshComponent->SetPath(balloonPath);
			balloonMeshComponent->SetIsHighLighted(true);

			physx::PxSphereGeometry sphereGeo(155);
			PhysicsComponent* physicsComponent = new PhysicsComponent();
			physicsComponent->AddGeometry(sphereGeo, true);
			physicsComponent->SetActive(false);
			//physicsComponent->SetFilter(PhysXManager::FilterGroup::eBALLONENEMY, PhysXManager::FilterGroup::ePLAYERATTACK| PhysXManager::FilterGroup::ePLAYER);
			//physicsComponent->SetSimulationDoPair(false);

			GameObject* gameObject = new GameObject(false);
			(*balloneColorType)[i].myEnemy = gameObject;
			gameObject->AddComponent<AnimatedMeshComponent>(balloonMeshComponent);

			EnemyComponent* enemyComponent = factory.CreateEnemy(EnemyType::eBalloon);
			gameObject->AddComponent<EnemyComponent>(enemyComponent);
			enemyComponent->SetPoolIndex(i);
			enemyComponent->SetPhysicsComponent(physicsComponent);

			gameObject->AddComponent<PhysicsComponent>(physicsComponent);
			gameObject->myName = "BalloonEnemy";
			gameObject->myTag = "Enemy";
		}
	}



	std::vector<GameObject*>children;
	children.resize(2);

	for (int j = 0; j < 4; j++)
	{
		std::vector<Enemy>* turretColorType = {};
		std::string turretTopPath;
		if (j == 0)
		{
			turretColorType = &myEnemies.at(EnemyType::eTurret).at(CardTypeEnum::Sword);
			turretTopPath = defaultTurretTopPath;
		}
		else if (j == 1)
		{
			turretColorType = &myEnemies.at(EnemyType::eTurret).at(CardTypeEnum::DoubleJump);
			turretTopPath = orangeTurretTopPath;
		}
		else if (j == 2)
		{
			turretColorType = &myEnemies.at(EnemyType::eTurret).at(CardTypeEnum::Dash);
			turretTopPath = pinkTurretTopPath;
		}
		else if (j == 3)
		{
			turretColorType = &myEnemies.at(EnemyType::eTurret).at(CardTypeEnum::Stomp);
			turretTopPath = blueTurretTopPath;
		}
		for (int i = 0; i < TURRET_ENEMY_POOL_SIZE; ++i)
		{
			GameObject* gameObject = new GameObject(false);
			std::vector<Mesh*> TurretBaseMesh = modelFactory.Load(TurretBottomPath);
			children = CreateTurret(TurretBaseMesh, TurretBottomPath, gameObject);

			std::vector<AnimatedMesh*> TurretHeadMesh = modelFactory.Load(turretTopPath, true);
			AnimatedMeshComponent* TurretHeadMeshComponent = new AnimatedMeshComponent(TurretHeadMesh);
			TurretHeadMeshComponent->AddAnimation(idleAnimationTurretPath, true, true);
			TurretHeadMeshComponent->AddAnimation(windupAnimationTurretPath, true, false);
			TurretHeadMeshComponent->AddAnimation(shootAnimationTurretPath, true, false);
			TurretHeadMeshComponent->AddAnimation(deathAnimationTurretPath, true, false);
			TurretHeadMeshComponent->SetPath(turretTopPath);
			TurretHeadMeshComponent->SetIsHighLighted(true);
			(*turretColorType)[i].myEnemy = gameObject;
			EnemyComponent* enemyComponent = factory.CreateEnemy(EnemyType::eTurret);
			gameObject->AddComponent<EnemyComponent>(enemyComponent);
			enemyComponent->SetPoolIndex(i);
			enemyComponent->SetPhysicsComponent(children[0]->GetComponent<PhysicsComponent>());
			gameObject->AddComponent<AnimatedMeshComponent>(TurretHeadMeshComponent);
			gameObject->myName = "TurretEnemy";
			gameObject->myTag = "Enemy";
			gameObject->myChildren = children;

			children.clear();
		}
	}



}

EnemyPool::~EnemyPool()
{
}

void EnemyPool::Init()
{
	for (auto& type : myEnemies)
	{
		for (auto& cardDrop : type.second)
		{
			for (auto& enemy : cardDrop.second)
			{
				enemy.myEnemy->GetComponent<EnemyComponent>()->Init();
			}
		}

	}
}

GameObject* EnemyPool::GetEnemyType(const EnemyInfo aEnemyInfo)
{

	auto& enemyTypeMap = myEnemies.at(aEnemyInfo.myEnemyType);

	if (enemyTypeMap.find(aEnemyInfo.myDropCardType) != enemyTypeMap.end()) {
		auto& enemyVector = enemyTypeMap.at(aEnemyInfo.myDropCardType);
		for (auto& enemy : enemyVector)
		{
			if (!enemy.myEnemyIsAvailaible)
				continue;

			enemy.myEnemy->SetActive(true);
			enemy.myEnemyIsAvailaible = false;
			enemy.myEnemy->GetComponent<EnemyComponent>()->SetEnemyDropCard(aEnemyInfo.myDropCardType);
			for (int i = 0; i < enemy.myEnemy->myChildren.size(); i++)
			{
				enemy.myEnemy->myChildren[i]->SetActive(true);
			}
			return enemy.myEnemy;
		}
	}
	return nullptr;

	/*for (Enemy& enemies : myEnemies.at(aEnemyInfo.myEnemyType))
	{
		if (!enemies.myEnemyIsAvailaible)
			continue;
		enemies.myEnemy->SetActive(true);
		enemies.myEnemyIsAvailaible = false;
		enemies.myEnemy->GetComponent<EnemyComponent>()->SetEnemyDropCard(aEnemyInfo.myDropCardType);
		for (int i = 0; i < enemies.myEnemy->myChildren.size(); i++)
		{
			enemies.myEnemy->myChildren[i]->SetActive(true);
		}
		return enemies.myEnemy;
	}
	return nullptr;*/
}

void EnemyPool::ReturnEnemy(const EnemyInfo aEnemyType, const int aPoolIndex)
{
	std::map<CardTypeEnum, std::vector<Enemy>>& enemyType = myEnemies.at(aEnemyType.myEnemyType);
	std::vector<Enemy>& enemyColor = enemyType.at(aEnemyType.myDropCardType);
	Enemy& enemyData = enemyColor[aPoolIndex];
	enemyData.myEnemyIsAvailaible = true;
	enemyData.myEnemy->SetActive(false);
	for (int i = 0; i < enemyData.myEnemy->myChildren.size(); i++)
	{
		enemyData.myEnemy->myChildren[i]->SetActive(false);
	}


	/*auto& enemyData = myEnemies.at(aEnemyType)[aPoolIndex];
	enemyData.myEnemyIsAvailaible = true;
	enemyData.myEnemy->SetActive(false);

	for (int i = 0; i < enemyData.myEnemy->myChildren.size(); i++)
	{
		enemyData.myEnemy->myChildren[i]->SetActive(false);
	}*/
}

void EnemyPool::NewStatesForTheTurret(const float aMaxHealth, const float aHealthPoint, const float aAttackCooldown, const float aAttackDamae, const float aRange)
{
	for (auto& turretsEnemy : myEnemies.at(EnemyType::eTurret))
	{
		for (int i = 0; i < turretsEnemy.second.size(); ++i)
		{
			auto& turretComponent = *turretsEnemy.second[i].myEnemy->GetComponent<EnemyComponent>();
			turretComponent.SetMaxHealthPoint(aMaxHealth);
			turretComponent.SetHealthPoint(aHealthPoint);
			turretComponent.SetAttackCooldown(aAttackCooldown);
			turretComponent.SetAttackDamage(aAttackDamae);
			turretComponent.SetRange(aRange);
		}
	}

	/*for (int i = 0; i < turretsEnemy.size(); ++i)
	{
		auto& turretComponent = *turretsEnemy[i].myEnemy->GetComponent<EnemyComponent>();
		turretComponent.SetMaxHealthPoint(aMaxHealth);
		turretComponent.SetHealthPoint(aHealthPoint);
		turretComponent.SetAttackCooldown(aAttackCooldown);
		turretComponent.SetAttackDamage(aAttackDamae);
		turretComponent.SetRange(aRange);
	}*/
}

std::vector<GameObject*> EnemyPool::CreateTurret(std::vector<Mesh*> aMesh, std::string aPath, GameObject* aParent)
{

	std::vector<Mesh*> baseMeshBase;

	baseMeshBase.push_back(aMesh[1]);

	MeshComponent* TurretBaseMeshComponent = new MeshComponent(baseMeshBase);

	TurretBaseMeshComponent->SetPath(aPath);
	TurretBaseMeshComponent->SetIsHighLighted(true);
	//Transform* turretBaseTransform = new Transform();

	GameObject* baseTurret;



	baseTurret = new GameObject(false);
	//baseTurret->myTransform = turretBaseTransform;
	baseTurret->AddComponent<MeshComponent>(TurretBaseMeshComponent);

	physx::PxBoxGeometry boxGeo(125, 170, 125);
	PhysicsComponent* physicsComponent = new PhysicsComponent();
	physicsComponent->AddGeometry(boxGeo, true);
	//physicsComponent->SetFilter(PhysXManager::FilterGroup::eGROUNDENEMY & PhysXManager::FilterGroup::eGROUND, PhysXManager::FilterGroup::ePLAYERATTACK);
	baseTurret->AddComponent<PhysicsComponent>(physicsComponent);
	//physicsComponent->SetCollisionFunction(std::bind(&EnemyComponent::ReactToHit, baseTurret->GetParent().GetComponent<EnemyComponent>(), );

	baseTurret->myParent = aParent;
	baseTurret->myName = "TurretBaseEnemy";
	baseTurret->myTag = "EnemyPart";


	std::vector<Mesh*> middleMeshBase;

	middleMeshBase.push_back(aMesh[0]);

	MeshComponent* TurretMiddleMeshComponent = new MeshComponent(middleMeshBase);

	TurretMiddleMeshComponent->SetPath(aPath);
	TurretMiddleMeshComponent->SetIsHighLighted(true);

	GameObject* middleTurret;

	middleTurret = new GameObject(false);
	middleTurret->AddComponent<MeshComponent>(TurretMiddleMeshComponent);

	middleTurret->myParent = aParent;
	middleTurret->myName = "TurretMiddleEnemy";
	middleTurret->myTag = "EnemyPart";

	std::vector<GameObject*> children(2);
	children[0] = baseTurret;
	children[1] = middleTurret;
	return children;
}

