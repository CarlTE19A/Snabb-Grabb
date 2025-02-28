#pragma once

#include "component/Component.h"
#include "math/Vector.h"
#include "EnemyInfo.h"

#include "../game/EnemyAnimation/AnimationType.h"


enum class BalloonAnimation;
enum class TurretAnimation;

class PhysicsComponent;
class PollingStation;
class BehaviorNode;
class Scene;
class EnemyComponent : public Component
{
public:
	EnemyComponent(PollingStation* aPollingStation);

	virtual ~EnemyComponent() = 0;

	virtual void Init() = 0;

	virtual void Update(const float aDeltaTime, Scene& aScene) override;


	inline void KillAfterTime(float aTime) { myDoKillAfterTime = true; myKillAfterTime = aTime; };

	void ImguiUpdate() override;

	void Reset() override;

	void SetHealthPoint(const float aHealthPoint);
	void SetMaxHealthPoint(const float aMaxHealthPoint);
	void SetAttackDamage(const float aAttackDamage);
	void SetAttackCooldown(const float aAttackCooldown);
	void SetRange(const float aRange);
	void SetPosition(const Vector3f aPosition);
	void SetDirection(const Vector3f aDirection);
	void SetRotation(const float aRotation);
	void SetPositionAndRotationToScene(const Vector3f aPosition, const float angle);
	//Set To it's Cooldownd
	void SetAttackTimer();

	void ReactToHit(const float aDamageTaken);

	void TakeDamage(const float aDamageTaken);


	void SetPoolIndex(const int aIndex);
	int GetPoolIndex()const;

	void SetEnemyTyp(const EnemyType aEnemyType);
	void SetEnemyDropCard(const CardTypeEnum aDropCard);
	Vector3f GetPosition();
	Vector3f GetRotation();
	EnemyType GetEnemyTyp()const;
	EnemyInfo GetEnemyInfo()const;

	float GetAttackRange()const;
	float GetAttackTimer()const;
	float GetHealth()const;

	void SetBehaviorNode(BehaviorNode* aBehaviorNode);
	PollingStation* GetPollingStation();

	void RotateTowardThePlayer(const Vector3f& aPosition, const float aRotationSpeed);

	virtual void CheckIfDied(Scene& aScene);

	virtual void ChangeAnimation(BalloonAnimation aAnimationType) { aAnimationType; };

	virtual void ChangeAnimation(TurretAnimation aAnimationType) { aAnimationType; };

	virtual void AddDontChangeToIdleAnimation(TurretAnimation aAnimationType) { aAnimationType; };

	virtual void RemoveFromDontChangeToIdleAnimation(TurretAnimation aAnimationType) { aAnimationType; };

	virtual bool IsTheAnimationFinish(TurretAnimation aAnimationType) { aAnimationType; return false; };




	virtual TurretAnimation whatAnimationIam() { return TurretAnimation::eIdle; };

	bool IsAlive()const;

	inline void SetPhysicsComponent(PhysicsComponent* aPhysicsComponent) {myPhysicsComponent = aPhysicsComponent;};

	void AddCardToScene(Scene& aScene);

	GameObject* CreateGameObject();

protected:

	PollingStation* myPollingStation;
	Vector3f myPosition;
	Vector3f myDirection;

	EnemyInfo myEnemyInformation{};

	float myHealthPoint = 0;
	float myMaxHealth = 0;
	float myAttackDamage = 0;
	float myAttackCooldown = 0;
	float myAttackTimer = 0;
	float myRange = 0;
	int myPoolIndex{};
	bool myIsAlive = true;

	BehaviorNode* myBehaviorNode = nullptr;
	PhysicsComponent* myPhysicsComponent=nullptr;

private:
	bool myDoKillAfterTime = false;
	float myKillAfterTime = 0.f;




};