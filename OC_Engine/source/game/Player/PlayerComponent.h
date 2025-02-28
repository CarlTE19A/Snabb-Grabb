#pragma once
#include "component\Component.h"
#include "events\inputMapper\InputObserver.h"
#include "math/Vector.h"
#include "../CardSystem/CardComponent.h"
#include "PhysX/PxShape.h"
#include <vector>

#include "../Controller/ControllerComponent.h"

#define SKILL_ISSUE false

class PhysicsComponent;
class CameraComponent;
class CardSystemComponent;
class Transform;
class AmmoPool;
class ExplosionPool;

class ParticleComponent;

//Cached Ptr
class InputMapper;

enum class ePlayerState
{
	Normal,
	BallonJumping,
	DoubleJumping,
	Dashing,
	Stomping,
	DirectionalDashing,
	Grappling,
	Teleporting,
	UnFocused
};

class PlayerComponent : public Component, public InputObserver
{
public:
	PlayerComponent();
	~PlayerComponent() override;

	//Creates a controller component and a camera gameobject
	void Init(Scene& aScene);

	void Update(const float aDeltaTime, Scene& aScene) override;

	void ImguiUpdate();

	void Reset() override;

	inline GameObject* GetCameraGameObject() { return myCameraGameObject; }

	void RecieveEvent(const eInputEvent aEvent, std::any aValue) override;

	void SetStartPos(const Vector3f& aPos);
	void SetStartRotation(const float aRotation);

	void SetState(const ePlayerState aState);

	Vector3f GetAppliedVelocity() const { return myAppliedVelocity; }

	void OnCollision(physx::PxShape* aShape);

	void OnHurtBoxCollision(physx::PxShape* aShape);

	//inline bool GetIsFocused() const { return myState != ePlayerState::UnFocused; };
private:

	void NormalUpdate(const float aDeltaTime, Scene& aScene);
	void DashingUpdate(const float aDeltaTime, Scene& aScene);
	void StompingUpdate(const float aDeltaTime, Scene& aScene);
	void DirectionalDashUpdate(const float aDeltaTime, Scene& aScene);
	void GrapplingUpdate(const float aDeltaTime, Scene& aScene);
	void TeleportingUpdate(const float aDeltaTime, Scene& aScene);

	void MainAttack();
	void SecondaryAttack();

	void DoStomp();

	//Loads data from the json file
	void LoadData();
	//Saves data to the json file
	void SaveData();

	void RotateCamera(float aDeltaTime);

	void Win();
	void Die(bool isInstantRespawn);

	Transform* myResetCameraTransform;

#pragma region Loadable Normal Data
	float mySpeed;
	float myJumpForce;
	float myGravity;
	float myRotationSpeed = 10;

	float myCayoteTime;
	float myCayoteTimer;

	bool myTakeDamage = true;
	int myMaxHealth = 3;
	int myHealth;
	float myInvincibilityTime = 0.5f;

	float myCameraYOffset;
#pragma endregion

	float myStandardFOV;
	//Max change in FOV per second
	float myMaxFOVChange = 50;

#pragma region Loadable Special Data
	//Loadable boost data
	float myBoostMultiplier = 2.0f;
	float myBoostFalloffTime = 0.5f;

	//Only in normal state
	float myHorizontalMaxSpeed;
	float myGravityMaxSpeed;

	//Ballon data
	float myBallonJumpForce;
	float myBallonJumpSpeedFalloffTime;
	float myBallonJumpMultiplier;
#pragma endregion
#pragma region Loadable Ability data
	//Sword ability
	float mySwordAbilityDamage;
	float mySwordAbilityCooldown;
	float mySwordAbilityDamageWidth;
	float mySwordAbilityDamageHeight;
	float mySwordAbilityDistanceAhead;

	//Double Jump ability
	float myJumpAbilityForce;
	//Dash ability
	float myDashAbilitySpeed;
	float myDashAbilityTime;
	float myDashAbilityFalloffTime;
	float myDashAbilityDamageWidth;
	float myDashAbilityDamageHeight;
	float myDashAbilityDamageDistanceAhead;
	//Stomp ability
	float myStompAbilitySpeed;
	float myStompAbilityRadius;
	//Directional Dash ability
	float myDirectionalDashAbilitySpeed;
	float myDirectionalDashAbilityTime;
	float myDirectionalDashAbilityFalloffTime;
	float myDirectionalDashAbilityDamageWidth;
	float myDirectionalDashAbilityDamageHeight;
	float myDirectionalDashAbilityDamageDistanceAhead;
	//Grappling ability
	float myGrapplingAbilitySpeed;
	float myGrapplingAbilityFalloffTime;
	float myGrapplingAbilityMaxDistance;
	float myGrapplingAbilityReleaseDistance;
	//Teleporting ability
	float myTeleportingAbilitySpeed;
	float myTeleportingAbilityTime;
	float myTeleportingAbilityFalloffTime;
	float myTeleportingAbilityMaxDistance;
	float myTeleportingAbilityShowLength;
	float myTeleportingAbilityKillRadius;
	float myTeleportingAbilityPercentforShow;
	//Bomb ability
	float myBombAbilityTime;
	float myBombAbilityMinForce;
	float myBombAbilityMaxForce;
	float myBombAbilityVerticalForce;
	float myBombAbilityHorizontalMultiplier;
	float myBombAbilityDrag;


#pragma endregion

#pragma region Local Temporary Data
	float myTargetFOV;
	float myLastFOV;

	std::unique_ptr<ParticleComponent> MyCachedParticleEffect;

	ePlayerState myState = ePlayerState::UnFocused;
	Vector3f myInputDirection = { 0,0,0 };
	Vector3f myVelocity = { 0,0,0 };
	physx::PxU32 myGroundedState = 0;
	physx::PxU32 myContactState = 0;
	float myInvincibilityTimer = 0;
#pragma region Falloff
	float myFalloffTimer = 0;
	float myFalloffMultiplier = 0;

	void ApplyFalloff(float aDeltaTime, Vector3f& velocity);
	void SetFalloff(float aMultiplier, float aTime);
	void ResetFalloff();

#pragma endregion
	float myLevelTimer = 0;
	Vector3f myBombVelocity = { 0,0,0 };

	float myTeleportTimer = 0;
	Vector3f myTeleportToPosition;
	Vector3f myTeleportShowPosition;
	//Vector3f myTeleportStartPosition;
	Vector3f myTeleportOutDirection;
	Vector3f myTeleportMovePerSecond;
	GameObject* myTeleportObject = nullptr;

	//Hit cards should not be needed if cards disepeared in time
	std::vector<int> myHitCards;
	//Temporary dash data
	float myDashTimer = 0;
	float myDashTimerTotal = 0;
	float myDashSpeed = 0;
	//Temporary shoot data
	float myShootTimer = 0;

	Vector3f myDashForward = { 0,0,0 };

	Vector3f myGrapplingPoint = { 0,0,0 };

	Vector3f myAppliedVelocity = { 0,0,0 };
#pragma endregion

	//Saved data
	ControllerComponent* myController = nullptr;
	CardSystemComponent* myCards = nullptr;
	GameObject* myCameraGameObject = nullptr;
	CameraComponent* myCamera = nullptr;
	AmmoPool* ammoPool = nullptr;
	ExplosionPool* explosionPool = nullptr;
	Scene* activeScene = nullptr;

#pragma region HurtBox
	GameObject* myHurtBox = nullptr;
	PhysicsComponent* myHurtBoxPhysics = nullptr;
	physx::PxRigidDynamic* myHurtBoxActor = nullptr;
	void SetHurtBox(CardTypeEnum aType);
#pragma endregion


#pragma region Math Functions
	Vector3f ConvertToWorldDirection(const Vector3f& aLocalDirection) const;
	Vector3f ConvertToSavedDirection(const Vector3f& aLocalDirection) const;
	Vector3f TwoDNormalize(const Vector3f& aVector) const;
#pragma endregion

	void SetFocus(bool aFocus);


#pragma region CachedPtr
	InputMapper& theInputMapper;
#pragma endregion


	physx::PxSphereGeometry standardBulletGeo;
};