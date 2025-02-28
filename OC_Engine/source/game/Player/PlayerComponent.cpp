#define NOMINMAX 
#include "PlayerComponent.h"
#include "Log.h"

#include "mainSingelton\MainSingelton.h"
#include "events\inputMapper\InputMapper.h"
#include "events/EventSystem.h"

#include "component\Particle\ParticleComponent.h"

#include "gameObject\GameObject.h"
#include "component\CameraComponent.h"
#include "../CardSystem/CardSystemComponent.h"
#include "math/Transform.h"
#include "PhysXCode/PhysXManager.h"
#include "component/PhysicsComponent.h"
#include "../GameManager/GameManager.h"
#include "../PollingStation.h"
#include "../Ammo/AmmoComponent.h"
#include "../Ammo/AmmoPool.h"
#include "../EnemyComponent/EnemyPool.h"
#include "../EnemyComponent/EnemyComponent.h"
#include "../EnemyComponent/BalloonComponent.h"
#include "../EnemyComponent/TurretComponent.h"

//Has to be included or sceneh will not compile
#include "graphics\Texture.h"
#include "scene\Scene.h"
//TODO REMOVE: TO CAPTURE MOUSE
#include "mainSingelton\MainSingelton.h"
#include "events\inputMapper\InputMapper.h"
#include "scene/SceneManager.h"
#include "tools/lineDrawer/LineDrawer.h"
#include "graphics/model/ModelFactory.h"
#include "PlayerBulletComponent.h"
//To Save Player Settings
#include "nlohmann/json.hpp"
#include <fstream>
#include "Paths.h"
#include "sound/SoundManager.h"
PlayerComponent::PlayerComponent() : theInputMapper(MainSingleton::GetInstance().GetInputMapper())
{
	myResetCameraTransform = new Transform();
	myStandardFOV = 90;
	myTargetFOV = myStandardFOV;
	myLastFOV = myTargetFOV;
	standardBulletGeo = physx::PxSphereGeometry(10.f);
	MyCachedParticleEffect = std::make_unique<ParticleComponent>();
	MyCachedParticleEffect->Init(1);
	MyCachedParticleEffect->GetParticleSystem().LoadParticleSystemSettings("BulletParticlePlayer.json");
}

PlayerComponent::~PlayerComponent()
{
	delete myResetCameraTransform;
	{
		theInputMapper.RemoveObserver(eInputEvent::eMoveUp, this);
		theInputMapper.RemoveObserver(eInputEvent::eMoveDown, this);
		theInputMapper.RemoveObserver(eInputEvent::eMoveLeft, this);
		theInputMapper.RemoveObserver(eInputEvent::eMoveRight, this);
		theInputMapper.RemoveObserver(eInputEvent::eSpace, this);

		theInputMapper.RemoveObserver(eInputEvent::eLeftMouseClick, this);
		theInputMapper.RemoveObserver(eInputEvent::eLeftMouseHold, this);
		theInputMapper.RemoveObserver(eInputEvent::eRightMouseClick, this);
		theInputMapper.RemoveObserver(eInputEvent::eKey_Q_Pressed, this);
		theInputMapper.RemoveObserver(eInputEvent::eKey_F_Pressed, this);


		//TODO REMOVE: TO CAPTURE MOUSE
		theInputMapper.RemoveObserver(eInputEvent::eKey_1_Pressed, this);
		theInputMapper.RemoveObserver(eInputEvent::eKey_2_Pressed, this);

		//TODO REMOVE: ADD CARDS
		theInputMapper.RemoveObserver(eInputEvent::eKey_3_Pressed, this);
		theInputMapper.RemoveObserver(eInputEvent::eKey_4_Pressed, this);
		theInputMapper.RemoveObserver(eInputEvent::eKey_5_Pressed, this);
		theInputMapper.RemoveObserver(eInputEvent::eKey_6_Pressed, this);
		theInputMapper.RemoveObserver(eInputEvent::eKey_7_Pressed, this);
		theInputMapper.RemoveObserver(eInputEvent::eKey_8_Pressed, this);
		theInputMapper.RemoveObserver(eInputEvent::eKey_9_Pressed, this);
		theInputMapper.RemoveObserver(eInputEvent::eKey_0_Pressed, this);
		//Capture and release mouse automatically
		theInputMapper.RemoveObserver(eInputEvent::eFocusBegin, this);
		theInputMapper.RemoveObserver(eInputEvent::eFocusEnd, this);

		//delete MyCachedParticleEffect;
		//MyCachedParticleEffect = nullptr;
	}

	//delete MyCachedParticleEffect;
	//MyCachedParticleEffect = nullptr;
}

void PlayerComponent::Init(Scene& aScene)
{
	GraphicsEngine* graphicsEngine = GraphicsEngine::GetInstance();
	aScene;
	ammoPool = MainSingleton::GetInstance().GetPollingStation().GetAmmoPool();
	activeScene = &aScene;
	//Input Maping
	{
		theInputMapper.AddObserver(eInputEvent::eMoveUp, this);
		theInputMapper.AddObserver(eInputEvent::eMoveDown, this);
		theInputMapper.AddObserver(eInputEvent::eMoveLeft, this);
		theInputMapper.AddObserver(eInputEvent::eMoveRight, this);
		theInputMapper.AddObserver(eInputEvent::eSpace, this);

		theInputMapper.AddObserver(eInputEvent::eLeftMouseClick, this);
		theInputMapper.AddObserver(eInputEvent::eLeftMouseHold, this);
		theInputMapper.AddObserver(eInputEvent::eRightMouseClick, this);
		theInputMapper.AddObserver(eInputEvent::eKey_Q_Pressed, this);
		theInputMapper.AddObserver(eInputEvent::eKey_F_Pressed, this);

#if SKILL_ISSUE

		//TODO REMOVE: TO CAPTURE MOUSE
		theInputMapper.AddObserver(eInputEvent::eKey_1_Pressed, this);
		theInputMapper.AddObserver(eInputEvent::eKey_2_Pressed, this);

		theInputMapper.AddObserver(eInputEvent::eKey_3_Pressed, this);
		theInputMapper.AddObserver(eInputEvent::eKey_4_Pressed, this);
		theInputMapper.AddObserver(eInputEvent::eKey_5_Pressed, this);
		theInputMapper.AddObserver(eInputEvent::eKey_6_Pressed, this);
		theInputMapper.AddObserver(eInputEvent::eKey_7_Pressed, this);
		theInputMapper.AddObserver(eInputEvent::eKey_8_Pressed, this);
		theInputMapper.AddObserver(eInputEvent::eKey_9_Pressed, this);
		theInputMapper.AddObserver(eInputEvent::eKey_0_Pressed, this);
		//Capture and release mouse automatically
#endif
		theInputMapper.AddObserver(eInputEvent::eFocusBegin, this);
		theInputMapper.AddObserver(eInputEvent::eFocusEnd, this);
	}
	//Add Camera
	{
		myCameraGameObject = new GameObject();
		myCamera = new CameraComponent();
		Vector2i windowSize = graphicsEngine->GetWindowSize();
		//myCamera->SetPerspectiveProjection(myStandardFOV, (float)windowSize.x, (float)windowSize.y, 1.f, 100000.f);
		myCameraGameObject->AddComponent(myCamera);
		myCameraGameObject->myName = "Player Camera";
		myCameraGameObject->SetPosition(myParent->GetPosition());
		myCameraGameObject->SetParent(myParent);
		aScene.AddGameObject(myCameraGameObject);
	}

	//Add HurtBox
	{
		myHurtBox = new GameObject();
		myHurtBox->SetPosition(myParent->GetPosition());
		myHurtBoxPhysics = new PhysicsComponent();
		//myDashBox = physx::PxBoxGeometry(myDashAbilityDamageWidth, myDashAbilityDamageHeight, 10);//physx::PxBoxGeometry(50.f, 50.f, 100.f);
		//myStompSphere = physx::PxSphereGeometry(myStompAbilityRadius);

		//myHurtBoxPhysics->AddGeometry(myDashBox, false);

		myHurtBox->AddComponent(myHurtBoxPhysics);
		myHurtBox->SetParent(myParent);
		myHurtBox->myName = "Player HurtBox";
		aScene.AddGameObject(myHurtBox);
		myHurtBoxPhysics->myValue = 1000;
		//myHurtBoxPhysics->SetActive(false);
		myHurtBoxPhysics->SetCollisionFunction(std::bind(&PlayerComponent::OnHurtBoxCollision, this, std::placeholders::_1));
	}

	//Add Controller
	{
		//myFileObserver = new Observer(Message::Type::FileDropped, std::bind(&EditorState::ReceiveMessage, this, std::placeholders::_1));
		myController = new ControllerComponent();
		myController->SetCollisionFunction(std::bind(&PlayerComponent::OnCollision, this, std::placeholders::_1));
		myParent->AddComponent(myController);
		//myController->Teleport(myStartPos, true);

		//myController->Teleport(myParent->GetPosition(), true);
	}

	//Add Card Component
	{
		myCards = new CardSystemComponent();
		myParent->AddComponent(myCards);
	}

	//Load data
	{
		LoadData();
	}
	Reset();

#ifndef _DEBUG

	//inputWrapper.CaptureMouse();
	//inputWrapper.HideMouse();
	//inputWrapper.LockMousePosition();
	SetState(ePlayerState::Normal);
	graphicsEngine->SetCamera(myCameraGameObject->GetComponent<CameraComponent>());
#endif

	return;
}

void PlayerComponent::Update(const float aDeltaTime, Scene& aScene)
{
	myLevelTimer += aDeltaTime;
	myGroundedState = myController->IsGrounded();
	myContactState = myController->IsContact();
	if (myShootTimer > 0)
	{
		myShootTimer -= aDeltaTime;
	}
	if (myContactState & PhysXManager::FilterGroup::eDEATHFLOOR)
	{
		LogInfo("Death Floor Hit: Respawning");
		Die(true);
		//Reset();
		return;
	}
	//Invincibility
	if (myInvincibilityTimer > 0)
	{
		myInvincibilityTimer -= aDeltaTime;
	}

	CardType cardType = myCards->GetCardType();
	if (cardType.myCardType == CardTypeEnum::Grappling)
	{
		physx::PxRaycastHit hitInfo;
		if (MainSingleton::GetInstance().GetPhysXManager().Raycast(
			myParent->GetPosition(),
			myCameraGameObject->GetTransform()->GetForward(),
			myGrapplingAbilityMaxDistance,
			hitInfo,
			PhysXManager::FilterGroup::eGROUND | PhysXManager::FilterGroup::eBALLONENEMY | PhysXManager::FilterGroup::eGROUNDENEMY))
		{
			//float length = (Vector3f(hitInfo.position.x, hitInfo.position.y, hitInfo.position.z) - myParent->GetPosition()).Length();
			//LogInfo("Length to Grapple point: " + std::to_string(length));
		}
		else
		{
			//LogInfo("Length to Grapple point: To Far");
		}
	}
	if (cardType.myCardType == CardTypeEnum::Teleporting)
	{
		physx::PxRaycastHit hitInfo;
		if (MainSingleton::GetInstance().GetPhysXManager().Raycast(
			myParent->GetPosition(),
			myCameraGameObject->GetTransform()->GetForward(),
			myTeleportingAbilityMaxDistance,
			hitInfo,
			PhysXManager::FilterGroup::eBALLONENEMY | PhysXManager::FilterGroup::eGROUNDENEMY))
		{
			//float length = (Vector3f(hitInfo.position.x, hitInfo.position.y, hitInfo.position.z) - myParent->GetPosition()).Length();
			//LogSuccess("Can Teleport");
		}
		else
		{
			//LogError("Can't Teleport");
		}
	}

#ifdef _DEBUG


	//TODO REMOVE: THIS IS FOR DEBUG
	ImGui::Begin("Player");
	ImGui::Text(("Timer:" + std::to_string(myLevelTimer)).c_str());
	CardType* card = &myCards->GetCardType();
	switch (card->myCardType)
	{
	case CardTypeEnum::Sword:
		ImGui::Text("Active Card: Sword");
		break;
	case CardTypeEnum::DoubleJump:
		ImGui::Text("Active Card: Double Jump");
		break;
	case CardTypeEnum::Dash:
		ImGui::Text("Active Card: Dash");
		break;
	case CardTypeEnum::Stomp:
		ImGui::Text("Active Card: Stomp");
		break;
	case CardTypeEnum::DirectionalDash:
		ImGui::Text("Active Card: DirectionalDash");
		break;
	case CardTypeEnum::Grappling:
		ImGui::Text("Active Card: Grappling");
		break;
	case CardTypeEnum::Teleporting:
		ImGui::Text("Active Card: Teleporting");
		break;
	case CardTypeEnum::COUNT:
		ImGui::Text("Active Card: COUNT");
		break;
	default:
		ImGui::Text("Active Card: Error");
		break;
	}
	ImGui::Text("Count: %d", card->myCount);
	ImGui::Text("Life: %d", myHealth);
	ImGui::BeginDisabled();
	ImGui::DragFloat3("Dash Velocity", &myDashForward.x, 0.1f);
	ImGui::EndDisabled();
	ImGui::End();
	//TODO REMOVE: END
#endif // DEBUG

	switch (myState)
	{
	case ePlayerState::Normal:
	case ePlayerState::BallonJumping:
	case ePlayerState::DoubleJumping:
		NormalUpdate(aDeltaTime, aScene);
		break;
	case ePlayerState::Dashing:
		DashingUpdate(aDeltaTime, aScene);
		break;
	case ePlayerState::Stomping:
		StompingUpdate(aDeltaTime, aScene);
		break;
	case ePlayerState::DirectionalDashing:
		DirectionalDashUpdate(aDeltaTime, aScene);
		break;
	case ePlayerState::Grappling:
		GrapplingUpdate(aDeltaTime, aScene);
		break;
	case ePlayerState::Teleporting:
		TeleportingUpdate(aDeltaTime, aScene);
		break;
	case ePlayerState::UnFocused:
		myVelocity = { 0, 0, 0 };
		myAppliedVelocity = myVelocity;
		myController->SetDisplacement(myAppliedVelocity);
		break;
	default:
		break;
	}
	Transform* camTransform = myCameraGameObject->GetTransform();
	camTransform->SetPosition(myParent->GetPosition() + Vector3f(0, myCameraYOffset, 0));

	myTargetFOV = std::clamp(myTargetFOV, 60.f, 175.f);
	if (myTargetFOV != myLastFOV)
	{
		float fov = std::clamp(myTargetFOV, myLastFOV - myMaxFOVChange * aDeltaTime, myLastFOV + myMaxFOVChange * aDeltaTime);

		GraphicsEngine* graphicsEngine = GraphicsEngine::GetInstance();
		myCamera->SetPerspectiveProjection(fov, (float)graphicsEngine->GetWindowSize().x, (float)graphicsEngine->GetWindowSize().y, 1.f, 10000000.f);
		myLastFOV = fov;
	}
}

void PlayerComponent::NormalUpdate(const float aDeltaTime, Scene& /*aScene*/)
{
	//lianne stupid
	//myHurtBoxPhysics->SetActive(false);
	myTargetFOV = myStandardFOV;
	RotateCamera(aDeltaTime);
	myVelocity = { 0, myVelocity.y, 0 };

	Vector3f horizontalInput = { myInputDirection.x, 0, myInputDirection.z };
	horizontalInput.Normalize();
	myInputDirection.x = horizontalInput.x;
	myInputDirection.z = horizontalInput.z;

	myVelocity.y += myGravity * aDeltaTime;

	//Cayote Time 
	if (myGroundedState & (PhysXManager::FilterGroup::eGROUND | PhysXManager::FilterGroup::eGROUNDENEMY | PhysXManager::FilterGroup::eDOOR))
	{
		myCayoteTimer = myCayoteTime;
		myVelocity.y = -0.01f;
	}
	else
	{
		myCayoteTimer -= aDeltaTime;
	}

	if (myCayoteTimer > 0 && myVelocity.y < 0 && myInputDirection.y > 0)
	{
		myCayoteTimer = 0;
		myVelocity.y = myJumpForce;
	}

	if (myState == ePlayerState::DoubleJumping)
	{
		myVelocity.y = myJumpAbilityForce;
	}
	if (myState == ePlayerState::BallonJumping)
	{
		myVelocity.y = myBallonJumpForce;
		SetFalloff(myBallonJumpMultiplier, myBallonJumpSpeedFalloffTime);
		//myBallonJumpSpeedFalloffTimer = myBallonJumpSpeedFalloffTime;
		//myDashTimer = 0;
		//myBoostFalloffTimer = 0;
	}

	//Horizontal movement
	myVelocity.x += myInputDirection.x * mySpeed;
	myVelocity.z += myInputDirection.z * mySpeed;

	myAppliedVelocity = ConvertToWorldDirection(myVelocity);
	float vericalSpeed = std::max(myAppliedVelocity.y, -myGravityMaxSpeed);
	myAppliedVelocity.y = 0;
	float length = std::min(myAppliedVelocity.Length(), myHorizontalMaxSpeed);
	myAppliedVelocity.Normalize();
	myAppliedVelocity *= length;

	//#pragma region BoostFalloff
	if (myGroundedState & PhysXManager::FilterGroup::eBOOST)
	{
		SetFalloff(myBoostMultiplier, myBoostFalloffTime);
	}
	ApplyFalloff(aDeltaTime, myAppliedVelocity);
	//	
	//	//myBoostFalloffTimer -= aDeltaTime;
	//	//if (myBoostFalloffTimer < 0)
	//	//{
	//	//	myBoostFalloffTimer = 0;
	//	//}
	//	//else
	//	//{
	//	//	myAppliedVelocity += (myAppliedVelocity * myBoostMultiplier * (myBoostFalloffTimer / myBoostFalloffTime));
	//	//}
	//#pragma endregion
	//#pragma region BallonJumpSpeed
	//	//myBallonJumpSpeedFalloffTimer -= aDeltaTime;
	//	//if (myBallonJumpSpeedFalloffTimer < 0)
	//	//{
	//	//	myBallonJumpSpeedFalloffTimer = 0;
	//	//}
	//	//else
	//	//{
	//	//	myAppliedVelocity += (myAppliedVelocity * myBallonJumpMultiplier * (myBallonJumpSpeedFalloffTimer / myBallonJumpSpeedFalloffTime));
	//	//}
	//#pragma endregion;
	myAppliedVelocity.y = vericalSpeed;

#pragma region DashFalloff
	//Vector3f dashVelocityVec = { 0, 0, 0 };
	float dashVelocity = 0;
	if (myDashTimer > 0)
	{
		myDashTimer -= aDeltaTime;
		dashVelocity = (myDashSpeed) * (myDashTimer / myDashTimerTotal);
		//Log(std::to_string(myDashTimer) + " F Velocity: " + std::to_string(myDashForward.x * dashVelocity) + ", " + std::to_string(myDashForward.y * dashVelocity) + ", " + std::to_string(myDashForward.z * dashVelocity));
		//Log(std::to_string(dashVelocity));
		//dashVelocityVec = myDashForward * dashVelocity;
		////myVelocity += ConvertToSavedDirection(dashVelocity);
		//if(dashVelocityVec.y > 0)
		//{
		//	//myDashForward.y+=0;
		//	Log("Dash Forward Y: " + std::to_string(myDashForward.y));
		//}
	}
#pragma endregion
	myAppliedVelocity += myDashForward.GetNormalized() * dashVelocity;
	myAppliedVelocity += myBombVelocity;
	myBombVelocity *= (1 - myBombAbilityDrag);
	myAppliedVelocity *= 1000.f;
	myAppliedVelocity *= aDeltaTime;
	//Log("Dash Forward: " + std::to_string(myDashForward.x) + ", " + std::to_string(myDashForward.y) + ", " + std::to_string(myDashForward.z));
	//Log("Applied Velocity: " + std::to_string(myAppliedVelocity.x) + ", " + std::to_string(myAppliedVelocity.y) + ", " + std::to_string(myAppliedVelocity.z));
	myController->SetDisplacement(myAppliedVelocity);
	myInputDirection = { 0,0,0 };

	//For double jump
	myState = ePlayerState::Normal;
}
#pragma region Abilities Update
void PlayerComponent::DashingUpdate(const float aDeltaTime, Scene& aScene)
{
	RotateCamera(aDeltaTime);

	//Ballon Stops Dash
	//if (myContactState & PhysXManager::FilterGroup::eBALLONENEMY)
	//{
	//	myState = ePlayerState::Normal;
	//	myDashTimer = 0;
	//	NormalUpdate(aDeltaTime, aScene);
	//	return;
	//}
	myDashTimer -= aDeltaTime;
	if (myDashTimer <= 0)
	{
		myState = ePlayerState::Normal;
		myDashTimer = myDashAbilityFalloffTime + aDeltaTime; //Extra as normalupdate will remove the deltaTime
		myDashTimerTotal = myDashAbilityFalloffTime;
		myDashSpeed = myDashAbilitySpeed;
		NormalUpdate(aDeltaTime, aScene);
		aScene.GetSoundManager()->StopEvent(4);
		return;
	}
	//myVelocity.z = myDashAbilitySpeed * aDeltaTime;
	//myVelocity.y = 0;
	myTargetFOV = myStandardFOV * 1.05f;
	//dashVelocity.z = (myDashAbilitySpeed * aDeltaTime) * (myDashTimer / myDashAbilityFalloffTime);
	//myAppliedVelocity = ConvertToSavedDirection(myVelocity);
	myAppliedVelocity = myDashForward.GetNormalized() * myDashAbilitySpeed * aDeltaTime;
	myAppliedVelocity *= 1000.f;
	myController->SetDisplacement(myAppliedVelocity);
	myInputDirection = { 0,0,0 };


	Transform targetTransform = *myParent->GetTransform();
	Vector3f targetPos = targetTransform.GetPosition() + (myDashForward * myDashAbilityDamageDistanceAhead);
	targetTransform.RotateTowardsAroundY(targetPos, 1000);
	targetTransform.SetPosition(targetPos);
	physx::PxTransform transform(targetTransform.GetPxMat44());
	myHurtBoxActor->setKinematicTarget(transform);
}

void PlayerComponent::StompingUpdate(const float aDeltaTime, Scene& aScene)
{
	myDashTimer = 0;
	myDashTimerTotal = 0;
	myDashSpeed = 0;
	RotateCamera(aDeltaTime);
	//if (myGroundedState & PhysXManager::FilterGroup::eBALLONENEMY)
	//{
	//	//Ballon Stops Dash
	//	myState = ePlayerState::Normal;
	//	myVelocity = { 0, 0, 0 };
	//	myDashTimer = 0;
	//	DoStomp();
	//	NormalUpdate(aDeltaTime, aScene);
	//	return;
	//}
	myVelocity = { 0, -myStompAbilitySpeed * aDeltaTime, 0 };
	if (myGroundedState & (PhysXManager::FilterGroup::eGROUND | PhysXManager::FilterGroup::eGROUNDENEMY | PhysXManager::FilterGroup::eDOOR))
	{
		myState = ePlayerState::Normal;
		myVelocity.y = -0.01f;
		DoStomp();
		NormalUpdate(aDeltaTime, aScene);
		//Todo Stomp Sound here test
		return;
	}
	myTargetFOV = myStandardFOV * 1.1f;
	myAppliedVelocity = ConvertToWorldDirection(myVelocity);
	myAppliedVelocity *= 1000.f;
	myController->SetDisplacement(myAppliedVelocity);
	myInputDirection = { 0,0,0 };
}

void PlayerComponent::DirectionalDashUpdate(const float aDeltaTime, Scene& aScene)
{
	RotateCamera(aDeltaTime);
	myDashTimer -= aDeltaTime;
	if (myDashTimer <= 0)
	{
		myState = ePlayerState::Normal;
		myDashSpeed = myDirectionalDashAbilitySpeed;
		myDashTimerTotal = myDirectionalDashAbilityFalloffTime;
		myDashTimer = myDirectionalDashAbilityFalloffTime + aDeltaTime; //Extra as normalupdate will remove the deltaTime
		NormalUpdate(aDeltaTime, aScene);
		//myDashForward.y = 0;
		return;
	}
	// myVelocity.z = myDashAbilitySpeed * aDeltaTime;
	//dashVelocity.z = (myDashAbilitySpeed * aDeltaTime) * (myDashTimer / myDashAbilityFalloffTime);
	//myVelocity.y = 0;

	myTargetFOV = myStandardFOV * 1.1f;
	myAppliedVelocity = myDashForward.GetNormalized() * myDirectionalDashAbilitySpeed * aDeltaTime;
	myAppliedVelocity *= 1000.f;
	myController->SetDisplacement(myAppliedVelocity);
	//Log("DirDash: Dash Forward: " + std::to_string(myDashForward.x) + ", " + std::to_string(myDashForward.y) + ", " + std::to_string(myDashForward.z));
	//Log("DirDash: Applied Velocity: " + std::to_string(myAppliedVelocity.x) + ", " + std::to_string(myAppliedVelocity.y) + ", " + std::to_string(myAppliedVelocity.z));
	myInputDirection = { 0,0,0 };


	Transform targetTransform = *myParent->GetTransform();
	Vector3f targetPos = targetTransform.GetPosition() + (myDashForward * myDirectionalDashAbilityDamageDistanceAhead);
	targetTransform.RotateTowards(targetPos, 1000);
	targetTransform.SetPosition(targetPos);
	physx::PxTransform transform(targetTransform.GetPxMat44());
	myHurtBoxActor->setKinematicTarget(transform);
}

void PlayerComponent::GrapplingUpdate(const float aDeltaTime, Scene& aScene)
{
	RotateCamera(aDeltaTime);
	DebugLine line;
	line.start = myParent->GetPosition();
	line.end = myGrapplingPoint;
	line.color = { 1, 0, 0, 1 };
	MainSingleton::GetInstance().GetLineDrawer().Draw(line);
	aDeltaTime;
	aScene;

	myVelocity = { 0, 0, 0 };
	Vector3f forward = (myGrapplingPoint - myParent->GetPosition()).GetNormalized();
	myVelocity = forward * myGrapplingAbilitySpeed * aDeltaTime;

	myAppliedVelocity = myVelocity;
	myAppliedVelocity *= 1000.f;
	myController->SetDisplacement(myAppliedVelocity);

	if ((myGrapplingPoint - myParent->GetPosition()).Length() < myGrapplingAbilityReleaseDistance)
	{
		myDashForward = forward;
		myDashForward.y += 1;
		myDashForward.Normalize();
		myVelocity = { 0,0,0 };
		myDashTimer = myGrapplingAbilityFalloffTime;
		myDashTimerTotal = myGrapplingAbilityFalloffTime;
		myDashSpeed = myGrapplingAbilitySpeed;
		myState = ePlayerState::Normal;
	}
}

void PlayerComponent::TeleportingUpdate(const float aDeltaTime, Scene& aScene)
{
	aDeltaTime;
	aScene;


	if (myTeleportTimer >= myTeleportingAbilityTime - (myTeleportingAbilityTime * myTeleportingAbilityPercentforShow))
	{
		myTargetFOV = myStandardFOV * 1.3f;
		//Vector3f moveDistance = (myTeleportShowPosition - myTeleportStartPosition) * myTeleportTimer / 2.f * aDeltaTime * 10.f;
		//Log("Move: " + std::to_string(moveDistance.x) +", " + std::to_string(moveDistance.y) + ", " + std::to_string(moveDistance.z));
		//myController->SetDisplacement(moveDistance);
		//Vector3f moveDistance = ((myTeleportShowPosition - myTeleportStartPosition) / (myTeleportTimer / 2.f)) * aDeltaTime;// *1000.f;
		//Log("Move: " + std::to_string(moveDistance.x) + ", " + std::to_string(moveDistance.y) + ", " + std::to_string(moveDistance.z));
		myController->SetDisplacement(myTeleportMovePerSecond * aDeltaTime);
	}
	else
	{
		myTargetFOV = myStandardFOV * 1.1f;
		myController->SetDisplacement({ 0,0,0 });
	}

	myTeleportTimer -= aDeltaTime;
	if (myTeleportTimer < 0)
	{
		//TurretComponent* enemy = myTeleportObject->GetComponent<TurretComponent>();
		//int enemyIndex = enemy->GetPoolIndex();
		SetHurtBox(CardTypeEnum::Teleporting);
		physx::PxMat44 mat = myParent->GetTransform()->GetPxMat44();
		mat.setPosition({ myTeleportToPosition.x, myTeleportToPosition.y, myTeleportToPosition.z });
		physx::PxTransform transform(mat);
		myHurtBoxActor->setKinematicTarget(transform);
		//MainSingleton::GetInstance().GetEnemyPool().ReturnEnemy(enemy->GetEnemyInfo(), enemyIndex);
		//myController->SetDisplacement(myTeleportToPosition - myParent->GetPosition());
		myController->Teleport(myTeleportToPosition, false);
		myDashSpeed = myTeleportingAbilitySpeed;
		myDashTimerTotal = myTeleportingAbilityFalloffTime;
		myDashTimer = myTeleportingAbilityFalloffTime;
		//SetFalloff(myTeleportingAbilitySpeed, myTeleportingAbilityFalloffTime);
		SetState(ePlayerState::Normal);
		return;
	}

}
#pragma endregion

void PlayerComponent::Reset()
{
	myTeleportTimer = 0;
	myBombVelocity = { 0,0,0 };
	myHealth = myMaxHealth;
	myVelocity = { 0, 0, 0 };
	myAppliedVelocity = myVelocity;
	myDashForward = { 0, 0, 0 };
	myDashTimer = 0;
	myDashTimerTotal = 0;
	myDashSpeed = 0;
	myCayoteTimer = 0;
	myInvincibilityTimer = 0;

	myFalloffTimer = 0;
	myFalloffMultiplier = 0;
	myShootTimer = 0;


	GraphicsEngine* graphicsEngine = GraphicsEngine::GetInstance();
	myCamera->SetPerspectiveProjection(myStandardFOV, (float)graphicsEngine->GetWindowSize().x, (float)graphicsEngine->GetWindowSize().y, 1.f, 1000000.f);
	myLastFOV = myStandardFOV;
	myState = ePlayerState::Normal;
	ResetFalloff();

	myController->Teleport(myResetCameraTransform->GetPosition(), true);
	myController->SetDisplacement(myAppliedVelocity);
	myCameraGameObject->SetTransform(myResetCameraTransform);

	SetFocus(true);
	//myCards->ClearDeck();
	myHitCards.clear();
	myLevelTimer = 0;
}
void PlayerComponent::RotateCamera(float aDeltaTime)
{
	Transform* cameratransform = myCameraGameObject->GetTransform();
	Vector2f mouseDelta = theInputMapper.GetMouseDelta();
	// Modify the rotation angles based on mouse delta
	float yaw = mouseDelta.x * myRotationSpeed * aDeltaTime;
	float pitch = -mouseDelta.y * myRotationSpeed * aDeltaTime;

	// Apply rotation to the camera's transform
	//myRotation += yaw;

	cameratransform->RotateAroundAxis({ 0, 1, 0 }, yaw);
	//LogInfo(std::to_string(transform->GetRotation().x));
	cameratransform->RotateAroundX(pitch);
	if (cameratransform->GetUp().y < 0)
		cameratransform->RotateAroundX(-pitch);
	//Log("Camera UP: " + std::to_string(cameratransform->GetUp().x) + ", " + std::to_string(cameratransform->GetUp().y) + ", " + std::to_string(cameratransform->GetUp().z));
}

void PlayerComponent::DoStomp()
{
	SetHurtBox(CardTypeEnum::Stomp);
	physx::PxTransform transform(myParent->GetTransform()->GetPxMat44());
	myHurtBoxActor->setKinematicTarget(transform);
	MainSingleton::GetInstance().GetSoundManager().EditParameter(4, "Sound", 2);
	MainSingleton::GetInstance().GetSoundManager().PlayEvent(4);
	
}

#pragma region StateChange
void PlayerComponent::Win()
{
	LogSuccess("You won, time: " + std::to_string(myLevelTimer));
	MainSingleton& mainSingleton = MainSingleton::GetInstance();
	mainSingleton.GetGameManager().SetTime(myLevelTimer);
	mainSingleton.GetEventSystem().SendMsg(Message(Message::Type::WonGame));
}

void PlayerComponent::Die(bool isFastRespawn)
{
	MainSingleton& mainSingleton = MainSingleton::GetInstance();
	myHealth = 0;
	//mainSingleton.GetEventSystem().SendMsg(Message(Message::Type::LostGame));
	mainSingleton.GetEventSystem().SendMsg(Message(Message::Type::LostGame, isFastRespawn));
}
#pragma endregion


void PlayerComponent::ApplyFalloff(float aDeltaTime, Vector3f& velocity)
{
	if (myFalloffTimer > 0)
	{
		myFalloffTimer -= aDeltaTime;
		velocity += (velocity * myFalloffMultiplier * (myFalloffTimer / myBoostFalloffTime));
	}

}
void PlayerComponent::SetFalloff(float aMultiplier, float aTime)
{
	myFalloffMultiplier = aMultiplier;
	myFalloffTimer = aTime;
}

void PlayerComponent::ResetFalloff()
{
	myFalloffMultiplier = 0;
	myFalloffTimer = 0;
}

void PlayerComponent::SetHurtBox(CardTypeEnum aType)
{
	physx::PxGeometry* geo = nullptr;
	switch (aType)
	{
	case CardTypeEnum::Sword:
		geo = new physx::PxBoxGeometry(mySwordAbilityDamageWidth, mySwordAbilityDamageHeight, 100);
		myHurtBoxPhysics->myValue = mySwordAbilityDamage;
		break;
	case CardTypeEnum::DoubleJump:
		return;
	case CardTypeEnum::Dash:
		geo = new physx::PxBoxGeometry(myDashAbilityDamageWidth, myDashAbilityDamageHeight, 100);
		myHurtBoxPhysics->myValue = 1000;
		break;
	case CardTypeEnum::Stomp:
		geo = new physx::PxSphereGeometry(myStompAbilityRadius);
		myHurtBoxPhysics->myValue = 1000;
		break;
	case CardTypeEnum::DirectionalDash:
		geo = new physx::PxBoxGeometry(myDirectionalDashAbilityDamageWidth, myDirectionalDashAbilityDamageHeight, 100);
		myHurtBoxPhysics->myValue = 1000;
		break;
	case CardTypeEnum::Grappling:
		return;
	case CardTypeEnum::Teleporting:
		geo = new physx::PxSphereGeometry(myTeleportingAbilityKillRadius);
		myHurtBoxPhysics->myValue = 1000;
		break;
	case CardTypeEnum::COUNT:
		break;
	default:
		return;
	}
	if (geo == nullptr)
	{
		return;
	}
	myHurtBoxPhysics->AddGeometry(*geo, false);

	myHurtBoxPhysics->SetFilter(PhysXManager::FilterGroup::ePLAYERATTACK,
		PhysXManager::FilterGroup::eGROUNDENEMY |
		PhysXManager::FilterGroup::eBALLONENEMY |
		PhysXManager::FilterGroup::eDOOR
	);

	myHurtBoxActor = myHurtBoxPhysics->GetActor()->is<physx::PxRigidDynamic>();
	if (myHurtBoxActor == nullptr)
	{
		LogError("HurtBoxActor is null");
		return;
	}
	myHurtBoxActor->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, true);
	myHurtBoxActor->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, true);
	//myHurtBoxPhysics->SetActive(true);
}
#pragma region Math Functions
Vector3f PlayerComponent::ConvertToWorldDirection(const Vector3f& aLocalDirection) const
{
	Vector3f forward = TwoDNormalize(myCameraGameObject->GetTransform()->GetForward());
	Vector3f worldDirection;

	worldDirection.x = forward.x * aLocalDirection.z + forward.z * aLocalDirection.x;
	worldDirection.y = aLocalDirection.y;
	worldDirection.z = forward.z * aLocalDirection.z - forward.x * aLocalDirection.x;

	return worldDirection;
}
Vector3f PlayerComponent::ConvertToSavedDirection(const Vector3f& aLocalDirection) const
{
	Vector3f worldDirection;

	worldDirection.x = myDashForward.x * aLocalDirection.z - myDashForward.z * aLocalDirection.x;
	worldDirection.y = myDashForward.y;
	worldDirection.z = myDashForward.z * aLocalDirection.z + myDashForward.x * aLocalDirection.x;
	return worldDirection;
}

//Vector3f PlayerComponent::ApplyLength(const Vector3f& aVector, float aLength) const
//{
//	Vector3f vectorToReturn = aVector;
//	vectorToReturn.Normalize();
//	vectorToReturn *= aLength;
//	return vectorToReturn;
//
//}

Vector3f PlayerComponent::TwoDNormalize(const Vector3f& aVector) const
{
	Vector3f vectorToReturn = aVector;
	vectorToReturn.y = 0;
	vectorToReturn.Normalize();
	return vectorToReturn;
}
#pragma endregion

#pragma region Input Functions
void PlayerComponent::OnCollision(physx::PxShape* aShape)
{
	physx::PxFilterData filterData = aShape->getQueryFilterData();
	if (filterData.word0 & PhysXManager::FilterGroup::eENEMYBULLET)
	{
		if (!myTakeDamage)
		{
			return;
		}
		if (myInvincibilityTimer > 0)
		{
			return;
		}
		myHealth--;
		myCamera->Shake(100.f, 0.2f, 5.f);
		MainSingleton::GetInstance().GetEventSystem().SendMsg(Message(Message::Type::PlayerHit, myHealth));
		myInvincibilityTimer = myInvincibilityTime;
		if (myHealth <= 0)
		{
			Die(false);
			//Reset();
		}
	}
	if (filterData.word0 & PhysXManager::FilterGroup::eGOAL)
	{
		Win();
	}
	if (filterData.word0 & PhysXManager::FilterGroup::eBALLONENEMY)
	{
		myState = ePlayerState::BallonJumping;
		//Log("Player hit Ballon: " + std::to_string(myLevelTimer));
		myDashTimer = 0;
		myDashTimerTotal = 0;
		myDashSpeed = 0;
	}
	if (filterData.word0 & PhysXManager::FilterGroup::eCARD)
	{
		PhysicsComponent* physics = static_cast<PhysicsComponent*>(aShape->userData);
		GameObject* targetObject = physics->GetOwner();
		int card = targetObject->GetID();
		physics->SetActive(false);
		if (!(std::find(myHitCards.begin(), myHitCards.end(), card) != myHitCards.end()))
		{
			//Log("Player hit Card");
			myHitCards.push_back(card);

			if (!myCards->AddCard(targetObject->GetComponent<CardComponent>()->GetCardType()))
			{
				LogWarning("Whoops, card should not have been picked up");
			}

		}
		else
		{
			LogError("Player hit Card, but already picked up");
		}
	}
	if (filterData.word0 & PhysXManager::FilterGroup::eBOMB)
	{
		Log("Player hit Bomb");
		PhysicsComponent* physics = static_cast<PhysicsComponent*>(aShape->userData);
		GameObject* targetObject = physics->GetOwner();
		//int explosion 
		Vector3f relativePos = myParent->GetPosition() - targetObject->GetPosition();
		if (relativePos.Length() > 1000)
		{
			return;
		}
		relativePos.y = 0;
		myBombVelocity += relativePos * myBombAbilityHorizontalMultiplier;
		myBombVelocity.y += myBombAbilityVerticalForce;

	}
}
void PlayerComponent::OnHurtBoxCollision(physx::PxShape* aShape)
{
	physx::PxFilterData filterData = aShape->getQueryFilterData();
	if (filterData.word0 & PhysXManager::FilterGroup::eBALLONENEMY)
	{
		//LogWarning("Player hit Ballon");
		myState = ePlayerState::BallonJumping;
		myDashTimer = 0;
		myDashTimerTotal = 0;
		myDashSpeed = 0;
	}
	aShape;
}
void PlayerComponent::RecieveEvent(const eInputEvent aEvent, std::any /*aValue*/)
{
#ifdef _DEBUG
	if (aEvent == eInputEvent::eKey_1_Pressed)
	{
		//theInputMapper.GetInputWrapper().SetFocus(false, true);
		SetFocus(true);
		return;
	}

	//#else
	//	if (aEvent == eInputEvent::eFocusBegin)
	//	{
	//		SetFocus(false);
	//		return;
	//	}
#endif
	if (myState == ePlayerState::UnFocused)
		return;
	switch (aEvent)
	{
	case eInputEvent::eMoveUp:
		myInputDirection.z += 1;
		break;
	case eInputEvent::eMoveDown:
		myInputDirection.z -= 1;
		break;
	case eInputEvent::eMoveLeft:
		myInputDirection.x -= 1;
		break;
	case eInputEvent::eMoveRight:
		myInputDirection.x += 1;
		break;
	case eInputEvent::eSpace:
		myInputDirection.y = 1;
		break;
	case eInputEvent::eLeftMouseClick:
	case eInputEvent::eLeftMouseHold:
		MainAttack();
		break;
	case eInputEvent::eRightMouseClick:
		SecondaryAttack();
		break;
	case eInputEvent::eKey_Q_Pressed:
		myCards->SwapCard();
		break;
	case eInputEvent::eKey_F_Pressed:
		//Reset();
		Die(true);
		break;

	case eInputEvent::eFocusBegin:
#ifndef _DEBUG
		SetFocus(true);
#endif
		break;
	case eInputEvent::eFocusEnd:
		//Log("Lost focus");
		SetFocus(false);
		break;
#ifdef _DEBUG


		//TODO REMOVE: TO CAPTURE MOUSE
	case eInputEvent::eKey_1_Pressed:
		break;
	case eInputEvent::eKey_2_Pressed:	//Unfocus
		SetFocus(false);
		break;
#endif
		//TODO REMOVE: ADD CARDS
	case eInputEvent::eKey_3_Pressed:
		myCards->AddCard(CardTypeEnum::DoubleJump);
		break;
	case eInputEvent::eKey_4_Pressed:
		myCards->AddCard(CardTypeEnum::Dash);
		break;
	case eInputEvent::eKey_5_Pressed:
		myCards->AddCard(CardTypeEnum::Stomp);
		break;
	case eInputEvent::eKey_6_Pressed:
		myCards->AddCard(CardTypeEnum::DirectionalDash);
		break;
	case eInputEvent::eKey_7_Pressed:
		myCards->AddCard(CardTypeEnum::Grappling);
		break;
	case eInputEvent::eKey_8_Pressed:
		myCards->AddCard(CardTypeEnum::Teleporting);
		break;
	case eInputEvent::eKey_9_Pressed:
		myCards->AddCard(CardTypeEnum::Bomb);
		break;
	case eInputEvent::eKey_0_Pressed:
		myCards->ClearDeck();
		break;
		//#endif // DEBUG
	default:
		break;
	}
}

void PlayerComponent::MainAttack()
{
	CardType card = myCards->GetCardType();
	if (myShootTimer > 0)
	{
		return;
	}
	myCards->UseCard();
	myShootTimer = card.myTimeBetweenShots;

	if (card.myCardType == CardTypeEnum::Sword)
	{
		LogInfo("Sword Attack");
		SetHurtBox(CardTypeEnum::Sword);


		Transform targetTransform = *myParent->GetTransform();
		Vector3f targetPos = targetTransform.GetPosition() + (myCameraGameObject->GetTransform()->GetForward() * mySwordAbilityDistanceAhead);
		targetTransform.RotateTowards(targetPos, 1000);
		targetTransform.SetPosition(targetPos);
		physx::PxTransform transform(targetTransform.GetPxMat44());


		//physx::PxTransform transform(myCameraGameObject->GetTransform()->GetPxMat44());
		myHurtBoxActor->setKinematicTarget(transform);
		return;
	}

	for (int i = 0; i < card.myBulletsPerShot; i++)
	{
		//GameObject* ammo;
		//ammo = ammoPool->GetAmmo(AmmoPool::AmmoType::PlayerBullet);
		//if (!ammo)
		//{
		//	LogError("No Ammo");
		//	return;
		//}
		//ammo->SetActive(true);
		//ammo->GetComponent<PhysicsComponent>()->myValue = (float)card.myDamage;
		//Vector3f direction = myCameraGameObject->GetTransform()->GetForward();
		//float lifetime = card.myRange / card.mySpeed;
		//ammoComponent->EditBullet(lifetime, card.mySpeed);
		//ammoComponent->Shoot(myCameraGameObject->GetPosition(), direction.GetNormalized(), false);

		Transform transform;
		Vector3f position = myParent->GetPosition();
		Vector3f direction = myCameraGameObject->GetTransform()->GetForward().GetNormalized();
		transform.SetPosition(position);
		//transform.RotateTowards(position + direction, 1000);
		physx::PxTransform pxTransform = physx::PxTransform(transform.GetPxMat44());

		physx::PxRaycastHit hit;
		PhysXManager& physxManager = MainSingleton::GetInstance().GetPhysXManager();
		Vector3f endPos = position + direction * card.myRange;
		if (physxManager.Raycast(
			//standardBulletGeo,
			//pxTransform,
			position,
			direction,
			card.myRange,
			hit,
			PhysXManager::FilterGroup::eGROUND | PhysXManager::FilterGroup::eDOOR | PhysXManager::FilterGroup::eBALLONENEMY | PhysXManager::FilterGroup::eGROUNDENEMY))
		{
			endPos = Vector3f(hit.position.x, hit.position.y, hit.position.z);
			//LogInfo("Hit: " + std::to_string(hit.distance) + ", Pos:" + std::to_string(endPos.x) + ", " + std::to_string(endPos.y) + ", " + std::to_string(endPos.z));
		}

		GameObject* bullet = new GameObject();
		//bullet->GetTransform()->RotateTowards(endPos, 10000.f);
		bullet->myName = "PlayerBullet";

		ParticleComponent* particleTrailEffect = new ParticleComponent(*MyCachedParticleEffect);

		//particleTrailEffect = MyCachedParticleEffect;

		//particleTrailEffect->Init(1);6
		//particleTrailEffect->GetParticleSystem().LoadParticleSystemSettings("BulletParticlePlayer.json");
	
		bullet->AddComponent<ParticleComponent>(particleTrailEffect);

		ModelFactory& modelFactory = MainSingleton::GetInstance().GetModelFactory();
		std::string bulletPath = "Assets/Bullet/sm_bullet.fbx";
		std::vector<Mesh*> bulletMesh = modelFactory.Load(bulletPath);

		MeshComponent* meshComponent = new MeshComponent(bulletMesh);
		bullet->AddComponent<MeshComponent>(meshComponent);

		PlayerBulletComponent* playerBullet = new PlayerBulletComponent();
		bullet->AddComponent<PlayerBulletComponent>(playerBullet);

		activeScene->AddGameObject(bullet);

		float timeToImpact = playerBullet->Start(position, endPos, card.mySpeed);
		if (hit.actor != nullptr)
		{
			PhysicsComponent* physicsComp = static_cast<PhysicsComponent*>(hit.actor->userData);
			GameObject* gameObject = physicsComp->GetOwner();
			EnemyComponent* eneComp = gameObject->GetComponent<EnemyComponent>();
			if (eneComp == nullptr)
			{
				gameObject = gameObject->GetParent();
				if (gameObject != nullptr)
				{
					eneComp = gameObject->GetComponent<EnemyComponent>();
				}
			}
			if (eneComp != nullptr)
			{
				eneComp->KillAfterTime(timeToImpact);
			}
		}
	}
}
void PlayerComponent::SecondaryAttack()
{
	CardType card = myCards->GetCardType();
	switch (card.myCardType)
	{
	case CardTypeEnum::Sword:
		break;
	case CardTypeEnum::DoubleJump:
	{
		LogInfo("Double Jump");
		SetState(ePlayerState::DoubleJumping);
		myCards->CardSpent();
		//Jump Sound
		MainSingleton::GetInstance().GetSoundManager().EditParameter(4, "Sound", 1);
		MainSingleton::GetInstance().GetSoundManager().PlayEvent(4);
		//MainSingleton::GetInstance().GetSoundManager().UpdatePosition(4, myParent->GetPosition());
		break;
	}
	case CardTypeEnum::Dash:
		myVelocity = { 0,0,0 };
		myBombVelocity = { 0,0,0 };
		LogInfo("Dash");
		SetState(ePlayerState::Dashing);
		myDashTimer = myDashAbilityTime;
		myDashForward = TwoDNormalize(myCameraGameObject->GetTransform()->GetForward());
		SetHurtBox(CardTypeEnum::Dash);
		myCards->CardSpent();
		//Dash Sound 
		MainSingleton::GetInstance().GetSoundManager().EditParameter(4, "Sound", 0);
		MainSingleton::GetInstance().GetSoundManager().PlayEvent(4);
		//MainSingleton::GetInstance().GetSoundManager().UpdatePosition(4, myParent->GetPosition());
		break;
	case CardTypeEnum::Stomp:
		myVelocity = { 0,0,0 };
		myBombVelocity = { 0,0,0 };
		LogInfo("Stomp");
		SetState(ePlayerState::Stomping);
		myCards->CardSpent();
		//Stomp sound
		/*MainSingleton::GetInstance().GetSoundManager().EditParameter(4, "Sound", 2);
		MainSingleton::GetInstance().GetSoundManager().PlayEvent(4);*/
		//MainSingleton::GetInstance().GetSoundManager().UpdatePosition(4, myParent->GetPosition());

		

		break;
	case CardTypeEnum::DirectionalDash:
		LogInfo("DirectionalDash");
		SetState(ePlayerState::DirectionalDashing);
		myVelocity = { 0,0,0 };
		myBombVelocity = { 0,0,0 };
		myDashTimer = myDirectionalDashAbilityTime;
		myDashForward = myCameraGameObject->GetTransform()->GetForward().GetNormalized();
		SetHurtBox(CardTypeEnum::DirectionalDash);
		myCards->CardSpent();
		break;
	case CardTypeEnum::Grappling:
	{
		physx::PxRaycastHit hitInfo;
		if (MainSingleton::GetInstance().GetPhysXManager().Raycast(
			myParent->GetPosition(),
			myCameraGameObject->GetTransform()->GetForward(),
			myGrapplingAbilityMaxDistance,
			hitInfo,
			PhysXManager::FilterGroup::eGROUND | PhysXManager::FilterGroup::eBALLONENEMY | PhysXManager::FilterGroup::eGROUNDENEMY))
		{
			myVelocity = { 0,0,0 };
			myBombVelocity = { 0,0,0 };
			LogInfo("Grapple");
			SetState(ePlayerState::Grappling);
			myGrapplingPoint = Vector3f(hitInfo.position.x, hitInfo.position.y, hitInfo.position.z);
			myCards->CardSpent();
		}
		else
		{
			LogInfo("Grapple Miss");
		}
		break;
	}
	case CardTypeEnum::Teleporting:
	{
		//myVelocity = { 0,0,0 };
		//myBombVelocity = { 0,0,0 };
		//LogInfo("Teleport");
		//SetState(ePlayerState::Teleporting);
		//myCards->CardSpent();
		Vector3f forward = myCameraGameObject->GetTransform()->GetForward();
		physx::PxRaycastHit hitInfo;
		if (MainSingleton::GetInstance().GetPhysXManager().Raycast(
			myParent->GetPosition(),
			forward,
			myTeleportingAbilityMaxDistance,
			hitInfo,
			PhysXManager::FilterGroup::eBALLONENEMY | PhysXManager::FilterGroup::eGROUNDENEMY))
		{
			//float length = (Vector3f(hitInfo.position.x, hitInfo.position.y, hitInfo.position.z) - myParent->GetPosition()).Length();
			LogSuccess("Teleporting");
			PhysicsComponent* physicsComp = static_cast<PhysicsComponent*>(hitInfo.actor->userData);
			//physicsComp->SetActive(false);

			GameObject* targetObject = physicsComp->GetOwner();
			if (targetObject == nullptr)
			{
				LogError("No Target Object");
				return;
			}
			if (targetObject->GetParent())
			{
				targetObject = targetObject->GetParent();
			}
			myTeleportObject = targetObject;
			myTeleportToPosition = targetObject->GetPosition();
			myTeleportOutDirection = forward.GetNormalized();
			myDashForward = myTeleportOutDirection;
			myTeleportShowPosition = myTeleportToPosition - (myTeleportOutDirection * myTeleportingAbilityShowLength);
			myTeleportTimer = myTeleportingAbilityTime;
			//myTeleportStartPosition = myParent->GetPosition();
			myTeleportMovePerSecond = (myTeleportShowPosition - myParent->GetPosition()) / (myTeleportingAbilityTime * myTeleportingAbilityPercentforShow);
			myVelocity = { 0,0,0 };
			myBombVelocity = { 0,0,0 };

			SetState(ePlayerState::Teleporting);
			myCards->CardSpent();
		}
		else
		{
			LogError("Can't Teleport");
		}
		break;
	}
	case CardTypeEnum::Bomb:
	{
		LogInfo("Bomb");
		GameObject* ammo;
		ammo = ammoPool->GetAmmo(AmmoPool::AmmoType::PlayerBullet);
		if (!ammo)
		{
			LogError("No Ammo");
			return;
		}
		ammo->SetActive(true);
		ammo->GetComponent<PhysicsComponent>()->myValue = (float)card.myDamage;
		AmmoComponent* ammoComponent = ammo->GetComponent<AmmoComponent>();
		Vector3f direction = myCameraGameObject->GetTransform()->GetForward();
		float lifetime = card.myRange / card.mySpeed;
		ammoComponent->EditBullet(lifetime, card.mySpeed);
		ammoComponent->Shoot(myCameraGameObject->GetPosition(), direction.GetNormalized(), true, myBombAbilityTime);
		activeScene->AddAmmoObject(ammo);
		myCards->CardSpent();
	}
	case CardTypeEnum::COUNT:
		LogWarning("CardType::COUNT is not a valid card type");
		break;
	default:
		LogError("CardType Error");
		break;
	}
}

#pragma endregion
#pragma region Setters
void PlayerComponent::SetStartPos(const Vector3f& aPos)
{
	myResetCameraTransform->SetPosition(aPos);
}
void PlayerComponent::SetStartRotation(const float aRotation)
{
	aRotation;
	Vector3f pos = myResetCameraTransform->GetPosition();
	myResetCameraTransform->ResetMatrix();
	myResetCameraTransform->SetPosition(pos);
	myResetCameraTransform->RotateAroundY(aRotation);
}
void PlayerComponent::SetFocus(bool aFocus)
{
	if (aFocus)
	{
#ifdef _DEBUG
		InputWrapper& inputWrapper = theInputMapper.GetInputWrapper();
		inputWrapper.SetFocus(false, true);
		SetState(ePlayerState::Normal);
		GraphicsEngine::GetInstance()->SetCamera(myCameraGameObject->GetComponent<CameraComponent>());
#else
#endif
	}
	else
	{
#ifdef _DEBUG
		InputWrapper& inputWrapper = theInputMapper.GetInputWrapper();
		inputWrapper.SetFocus(true, false);
		SetState(ePlayerState::UnFocused);
#else
		//inputWrapper.SetFocus(true, false);
		MainSingleton::GetInstance().GetEventSystem().SendMsg(Message(Message::Type::LostFocus));
#endif
	}
	aFocus;
	//InputWrapper& inputWrapper = MainSingleton::GetInstance().GetInputMapper().GetInputWrapper();
	//if (aFocus)
	//{
	//	inputWrapper.CaptureMouse();
	//	inputWrapper.HideMouse();
	//	inputWrapper.LockMousePosition();
	//	SetState(ePlayerState::Normal);
	//	GraphicsEngine::GetInstance()->SetCamera(myCameraGameObject->GetComponent<CameraComponent>());
	//}
	//else
	//{
	//	inputWrapper.ReleaseMouse();
	//	inputWrapper.ShowMouse();
	//	inputWrapper.UnlockMousePosition();
	//	SetState(ePlayerState::UnFocused);
	//}
}

void PlayerComponent::SetState(const ePlayerState aState)
{
	myState = aState;
}
#pragma endregion
#pragma region SaveLoad
//Mostly for debug below here
void PlayerComponent::ImguiUpdate()
{
	if (ImGui::BeginTabItem("Player"))
	{
		ImGui::SeparatorText("Info");
		ImGui::Text("Camera ID: %d", myCameraGameObject->GetID());
		ImGui::Text("Timer: %f", myLevelTimer);
		//ImGui::Text("Health: %d", myHealth);
		ImGui::Separator();
		CardType* card = &myCards->GetCardType();
		ImGui::Text("Active Card: %d", card->myCardType);
		ImGui::Text("Accuracy: %f", card->myAccuracy);
		ImGui::Text("Ammo: %d", card->myAmmo);
		ImGui::Text("Bullets per Shot: %d", card->myBulletsPerShot);
		ImGui::Text("Count %d", card->myCount);
		ImGui::Text("Damage: %f", card->myDamage);
		ImGui::Text("Range: %f", card->myRange);
		ImGui::Text("Speed: %f", card->mySpeed);
		ImGui::Text("Time Between Shots: %f", card->myTimeBetweenShots);


		ImGui::Separator();
		if (ImGui::Button("Save Settings"))
		{
			SaveData();
		}
		if (ImGui::Button("Load Settings"))
		{
			LoadData();
		}
		//ImGui::Checkbox("Focused", &myFocused);
		ImGui::Text("State:", myState);

		ImGui::Checkbox("Take Damage", &myTakeDamage);
		ImGui::SliderInt("Health:", &myHealth, 0, myMaxHealth);
		ImGui::DragInt("Max Health:", &myMaxHealth, 1, 1);
		ImGui::DragFloat("Invicibility (s)", &myInvincibilityTime, 0.05f);

		ImGui::DragFloat("Speed##Standard", &mySpeed, 10.f);
		ImGui::DragFloat("Jump Force##Standard", &myJumpForce, 1.f);
		ImGui::DragFloat("Gravity##Standard", &myGravity, 1.f);
		ImGui::DragFloat("Rotation Speed##Standard", &myRotationSpeed, 0.1f);
		ImGui::DragFloat("Cayote Time##Standard", &myCayoteTime, 0.05f);

		ImGui::Text("Clamped");
		ImGui::DragFloat("Max Horizontal Velocity##Clamped", &myHorizontalMaxSpeed, 0.1f);
		ImGui::DragFloat("Max Gravity Velocity##Clamped", &myGravityMaxSpeed, 0.1f);
		ImGui::DragFloat("Camera Y Offset##Clamped", &myCameraYOffset, 0.1f);

		if (ImGui::CollapsingHeader("Boosts"))
		{
			ImGui::Text("Ballon");
			ImGui::DragFloat("Force##Ballon", &myBallonJumpForce, 1.f);
			ImGui::DragFloat("Timer##Ballon", &myBallonJumpSpeedFalloffTime, 0.1f);
			ImGui::DragFloat("Multiplier##Ballon", &myBallonJumpMultiplier, 1.f);

			ImGui::Text("SpeedBoost");
			ImGui::DragFloat("Timer##Speed", &myBoostFalloffTime, 0.05f);
			ImGui::DragFloat("Multiplier##Speed", &myBoostMultiplier, 0.05f);
		}

		if (ImGui::BeginTabBar("Abilities"))
		{
			if (ImGui::BeginTabItem("Sword"))
			{
				ImGui::DragFloat("Damage##Sword", &mySwordAbilityDamage, 1.f);
				ImGui::DragFloat("Cooldown##Sword", &mySwordAbilityCooldown, 0.1f);
				ImGui::DragFloat("Width##Sword", &mySwordAbilityDamageWidth, 0.1f);
				ImGui::DragFloat("Height##Sword", &mySwordAbilityDamageHeight, 0.1f);
				ImGui::DragFloat("Distance Ahead##Sword", &mySwordAbilityDistanceAhead, 0.1f);
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Double Jump"))
			{
				ImGui::DragFloat("Speed##DoubleJump", &myJumpAbilityForce, 1.f);
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Dash"))
			{
				ImGui::DragFloat("Speed##Dash", &myDashAbilitySpeed, 1.f);
				ImGui::DragFloat("Time##Dash", &myDashAbilityTime, 0.1f);
				ImGui::DragFloat("Falloff Time##Dash", &myDashAbilityFalloffTime, 0.1f);
				ImGui::DragFloat("Width##Dash", &myDashAbilityDamageWidth, 0.1f, 0.1f);
				ImGui::DragFloat("Height##Dash", &myDashAbilityDamageHeight, 0.1f, 0.1f);
				ImGui::DragFloat("Distance Ahead##Dash", &myDashAbilityDamageDistanceAhead, 0.1f, 0.1f);
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Stomp"))
			{
				ImGui::DragFloat("Force##Stomp", &myStompAbilitySpeed, 1.f);
				ImGui::DragFloat("Radius##Stomp", &myStompAbilityRadius, 0.1f);
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Directional Dash"))
			{
				ImGui::DragFloat("Speed##DirDash", &myDirectionalDashAbilitySpeed, 1.f);
				ImGui::DragFloat("Time##DirDash", &myDirectionalDashAbilityTime, 0.1f);
				ImGui::DragFloat("Falloff Time##DirDash", &myDirectionalDashAbilityFalloffTime, 0.1f);
				ImGui::DragFloat("Width##DirDash", &myDirectionalDashAbilityDamageWidth, 0.1f, 0.1f);
				ImGui::DragFloat("Height##DirDash", &myDirectionalDashAbilityDamageHeight, 0.1f, 0.1f);
				ImGui::DragFloat("Distance Ahead##DirDash", &myDirectionalDashAbilityDamageDistanceAhead, 0.1f, 0.1f);
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Grappling"))
			{
				ImGui::DragFloat("Speed##Grappling", &myGrapplingAbilitySpeed, 1.f);
				ImGui::DragFloat("Time##Grappling", &myGrapplingAbilityFalloffTime, 0.1f);
				ImGui::DragFloat("Max Distance##Grappling", &myGrapplingAbilityMaxDistance, 1.f);
				ImGui::DragFloat("Release Distance##Grappling", &myGrapplingAbilityReleaseDistance, 1.f);
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Teleporting"))
			{
				ImGui::DragFloat("Speed##Teleporting", &myTeleportingAbilitySpeed, 1.f);
				ImGui::DragFloat("Time##Teleporting", &myTeleportingAbilityTime, 0.1f);
				ImGui::DragFloat("Faloff Time##Teleporting", &myTeleportingAbilityFalloffTime, 0.1f);
				ImGui::DragFloat("Max Distance##Teleporting", &myTeleportingAbilityMaxDistance, 1.f);
				ImGui::DragFloat("Show Length##Teleporting", &myTeleportingAbilityShowLength, 1.f);
				ImGui::DragFloat("Kill Radius##Teleporting", &myTeleportingAbilityKillRadius, 1.f);
				ImGui::DragFloat("PercentageForShow##Teleporting", &myTeleportingAbilityPercentforShow, 0.01f, 0.f, 1.f);
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Bomb"))
			{
				ImGui::DragFloat("Time##Bomb", &myBombAbilityTime, 1.f);
				ImGui::DragFloat("Min Force##Bomb", &myBombAbilityMinForce, 1.f, 0.f, myBombAbilityMaxForce);
				ImGui::DragFloat("Max Force##Bomb", &myBombAbilityMaxForce, 1.f, myBombAbilityMinForce);
				ImGui::DragFloat("Vertical##Bomb", &myBombAbilityVerticalForce, 0.1f);
				ImGui::DragFloat("Horizontal Multiplier##Bomb", &myBombAbilityHorizontalMultiplier, 0.1f);
				ImGui::DragFloat("Drag##Bomb", &myBombAbilityDrag, 0.1f);
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}
		ImGui::EndTabItem();


	}
}

void PlayerComponent::LoadData()
{
	std::ifstream file;
	file.open(STATIC_ASSET_PATH "json/playerSettings.json");
	if (file.is_open())
	{
		nlohmann::json json;
		file >> json;
#ifdef _DEBUG

		try { myMaxHealth = json["MaxHealth"]; }
		catch (const std::exception&) { myMaxHealth = 3; }
		try { myInvincibilityTime = json["Invicibility"]; }
		catch (const std::exception&) { myInvincibilityTime = 0.5f; }
		try { mySpeed = json["Speed"]; }
		catch (const std::exception&) { mySpeed = 4000.f; };
		try { myJumpForce = json["JumpForce"]; }
		catch (const std::exception&) { myJumpForce = 6000.f; };
		try { myGravity = json["Gravity"]; }
		catch (const std::exception&) { myGravity = 200.f; };
		try { myRotationSpeed = json["RotationSpeed"]; }
		catch (const std::exception&) { myRotationSpeed = 1.f; };
		try { myCayoteTime = json["CayoteTime"]; }
		catch (const std::exception&) { myCayoteTime = 0.1f; };
		try { myHorizontalMaxSpeed = json["MaxHorizontalVelocity"]; }
		catch (const std::exception&) { myHorizontalMaxSpeed = 1000.f; };
		try { myGravityMaxSpeed = json["MaxVerticalVelocity"]; }
		catch (const std::exception&) { myGravityMaxSpeed = 1000.f; };
		try { myCameraYOffset = json["CameraYOffset"]; }
		catch (const std::exception&) { myCameraYOffset = 0.f; };

		try { mySwordAbilityDamage = json["Sword"]["Damage"]; }
		catch (const std::exception&) { mySwordAbilityDamage = 1.f; };
		try { mySwordAbilityCooldown = json["Sword"]["Cooldown"]; }
		catch (const std::exception&) { mySwordAbilityCooldown = 0.1f; };
		try { mySwordAbilityDamageWidth = json["Sword"]["Width"]; }
		catch (const std::exception&) { mySwordAbilityDamageWidth = 0.1f; };
		try { mySwordAbilityDamageHeight = json["Sword"]["Height"]; }
		catch (const std::exception&) { mySwordAbilityDamageHeight = 0.1f; };
		try { mySwordAbilityDistanceAhead = json["Sword"]["DistanceAhead"]; }
		catch (const std::exception&) { mySwordAbilityDistanceAhead = 1.f; };

		try { myJumpAbilityForce = json["DoubleJump"]["Force"]; }
		catch (const std::exception&) { myJumpAbilityForce = 0.6f; };

		try { myDashAbilitySpeed = json["Dash"]["Speed"]; }
		catch (const std::exception&) { myDashAbilitySpeed = 0.6f; };
		try { myDashAbilityTime = json["Dash"]["Time"]; }
		catch (const std::exception&) { myDashAbilityTime = 0.1f; };
		try { myDashAbilityFalloffTime = json["Dash"]["FalloffTime"]; }
		catch (const std::exception&) { myDashAbilityFalloffTime = 0.1f; }
		try { myDashAbilityDamageWidth = json["Dash"]["Width"]; }
		catch (const std::exception&) { myDashAbilityDamageWidth = 0.1f; };
		try { myDashAbilityDamageHeight = json["Dash"]["Height"]; }
		catch (const std::exception&) { myDashAbilityDamageHeight = 0.1f; };
		try { myDashAbilityDamageDistanceAhead = json["Dash"]["DistanceAhead"]; }
		catch (const std::exception&) { myDashAbilityDamageDistanceAhead = 1.f; };

		try { myStompAbilitySpeed = json["Stomp"]["Force"]; }
		catch (const std::exception&) { myStompAbilitySpeed = 0.6f; };
		try { myStompAbilityRadius = json["Stomp"]["Radius"]; }
		catch (const std::exception&) { myStompAbilityRadius = 0.1f; };
		//try { myStompAbilityDistanceAhead = json["Stomp"]["DistanceAhead"]; }catch (const std::exception&) { myStompAbilityDistanceAhead = 1.f;}

		try { myDirectionalDashAbilitySpeed = json["DirectionalDash"]["Speed"]; }
		catch (const std::exception&) { myDirectionalDashAbilitySpeed = 0.6f; };
		try { myDirectionalDashAbilityTime = json["DirectionalDash"]["Time"]; }
		catch (const std::exception&) { myDirectionalDashAbilityTime = 0.1f; };
		try { myDirectionalDashAbilityFalloffTime = json["DirectionalDash"]["FalloffTime"]; }
		catch (const std::exception&) { myDirectionalDashAbilityFalloffTime = 0.1f; };
		try { myDirectionalDashAbilityDamageWidth = json["DirectionalDash"]["Width"]; }
		catch (const std::exception&) { myDirectionalDashAbilityDamageWidth = 0.1f; };
		try { myDirectionalDashAbilityDamageHeight = json["DirectionalDash"]["Height"]; }
		catch (const std::exception&) { myDirectionalDashAbilityDamageHeight = 0.1f; };
		try { myDirectionalDashAbilityDamageDistanceAhead = json["DirectionalDash"]["DistanceAhead"]; }
		catch (const std::exception&) { myDirectionalDashAbilityDamageDistanceAhead = 1.f; };

		try { myGrapplingAbilitySpeed = json["Grappling"]["Speed"]; }
		catch (const std::exception&) { myGrapplingAbilitySpeed = 0.6f; };
		try { myGrapplingAbilityFalloffTime = json["Grappling"]["Time"]; }
		catch (const std::exception&) { myGrapplingAbilityFalloffTime = 0.1f; };
		try { myGrapplingAbilityMaxDistance = json["Grappling"]["MaxDistance"]; }
		catch (const std::exception&) { myGrapplingAbilityMaxDistance = 100.f; };
		try { myGrapplingAbilityReleaseDistance = json["Grappling"]["ReleaseDistance"]; }
		catch (const std::exception&) { myGrapplingAbilityReleaseDistance = 100.f; };

		try { myTeleportingAbilitySpeed = json["Teleporting"]["Speed"]; }
		catch (const std::exception&) { myTeleportingAbilitySpeed = 0.6f; };
		try { myTeleportingAbilityTime = json["Teleporting"]["Time"]; }
		catch (const std::exception&) { myTeleportingAbilityTime = 0.1f; };
		try { myTeleportingAbilityFalloffTime = json["Teleporting"]["FalloffTime"]; }
		catch (const std::exception&) { myTeleportingAbilityFalloffTime = 0.1f; };
		try { myTeleportingAbilityMaxDistance = json["Teleporting"]["MaxDistance"]; }
		catch (const std::exception&) { myTeleportingAbilityMaxDistance = 100.f; };
		try { myTeleportingAbilityShowLength = json["Teleporting"]["ShowLength"]; }
		catch (const std::exception&) { myTeleportingAbilityShowLength = 100.f; };
		try { myTeleportingAbilityKillRadius = json["Teleporting"]["Radius"]; }
		catch (const std::exception&) { myTeleportingAbilityKillRadius = 100.f; };
		try { myTeleportingAbilityPercentforShow = json["Teleporting"]["Percent"]; }
		catch (const std::exception&) { myTeleportingAbilityPercentforShow = 0.5f; };

		try { myBombAbilityTime = json["Bomb"]["Time"]; }
		catch (const std::exception&) { myBombAbilityTime = 1.f; };
		try { myBombAbilityMinForce = json["Bomb"]["MinForce"]; }
		catch (const std::exception&) { myBombAbilityMinForce = 1.f; };
		try { myBombAbilityMaxForce = json["Bomb"]["MaxForce"]; }
		catch (const std::exception&) { myBombAbilityMaxForce = 1.f; };
		try { myBombAbilityVerticalForce = json["Bomb"]["VerticalForce"]; }
		catch (const std::exception&) { myBombAbilityVerticalForce = 0.1f; };
		try { myBombAbilityHorizontalMultiplier = json["Bomb"]["HorizontalMultiplier"]; }
		catch (const std::exception&) { myBombAbilityHorizontalMultiplier = 0.1f; };
		try { myBombAbilityDrag = json["Bomb"]["Drag"]; }
		catch (const std::exception&) { myBombAbilityDrag = 0.1f; };

		try { myBallonJumpForce = json["Ballon"]["Force"]; }
		catch (const std::exception&) { myBallonJumpForce = 0.6f; };
		try { myBallonJumpSpeedFalloffTime = json["Ballon"]["FalloffTime"]; }
		catch (const std::exception&) { myBallonJumpSpeedFalloffTime = 0.1f; };
		try { myBallonJumpMultiplier = json["Ballon"]["Speed"]; }
		catch (const std::exception&) { myBallonJumpMultiplier = 0.6f; };

		try { myBoostMultiplier = json["Boost"]["Multiplier"]; }
		catch (const std::exception&) { myBoostMultiplier = 2.0f; };
		try { myBoostFalloffTime = json["Boost"]["FalloffTime"]; }
		catch (const std::exception&) { myBoostFalloffTime = 0.4f; };


#else
		myMaxHealth = json["MaxHealth"];
		myInvincibilityTime = json["Invicibility"];
		mySpeed = json["Speed"];
		myJumpForce = json["JumpForce"];
		myGravity = json["Gravity"];
		myRotationSpeed = json["RotationSpeed"];
		myCayoteTime = json["CayoteTime"];
		myHorizontalMaxSpeed = json["MaxHorizontalVelocity"];
		myGravityMaxSpeed = json["MaxVerticalVelocity"];
		myCameraYOffset = json["CameraYOffset"];

		mySwordAbilityDamage = json["Sword"]["Damage"];
		mySwordAbilityCooldown = json["Sword"]["Cooldown"];
		mySwordAbilityDamageWidth = json["Sword"]["Width"];
		mySwordAbilityDamageHeight = json["Sword"]["Height"];
		mySwordAbilityDistanceAhead = json["Sword"]["DistanceAhead"];

		myJumpAbilityForce = json["DoubleJump"]["Force"];

		myDashAbilitySpeed = json["Dash"]["Speed"];
		myDashAbilityTime = json["Dash"]["Time"];
		myDashAbilityFalloffTime = json["Dash"]["FalloffTime"];
		myDashAbilityDamageWidth = json["Dash"]["Width"];
		myDashAbilityDamageHeight = json["Dash"]["Height"];
		myDashAbilityDamageDistanceAhead = json["Dash"]["DistanceAhead"];

		myStompAbilitySpeed = json["Stomp"]["Force"];
		myStompAbilityRadius = json["Stomp"]["Radius"];

		myDirectionalDashAbilitySpeed = json["DirectionalDash"]["Speed"];
		myDirectionalDashAbilityTime = json["DirectionalDash"]["Time"];
		myDirectionalDashAbilityFalloffTime = json["DirectionalDash"]["FalloffTime"];
		myDirectionalDashAbilityDamageWidth = json["DirectionalDash"]["Width"];
		myDirectionalDashAbilityDamageHeight = json["DirectionalDash"]["Height"];
		myDirectionalDashAbilityDamageDistanceAhead = json["DirectionalDash"]["DistanceAhead"];

		myGrapplingAbilitySpeed = json["Grappling"]["Speed"];
		myGrapplingAbilityFalloffTime = json["Grappling"]["Time"];
		myGrapplingAbilityMaxDistance = json["Grappling"]["MaxDistance"];
		myGrapplingAbilityReleaseDistance = json["Grappling"]["ReleaseDistance"];

		myTeleportingAbilitySpeed = json["Teleporting"]["Speed"];
		myTeleportingAbilityTime = json["Teleporting"]["Time"];
		myTeleportingAbilityFalloffTime = json["Teleporting"]["FalloffTime"];
		myTeleportingAbilityMaxDistance = json["Teleporting"]["MaxDistance"];
		myTeleportingAbilityShowLength = json["Teleporting"]["ShowLength"];
		myTeleportingAbilityKillRadius = json["Teleporting"]["Radius"];
		myTeleportingAbilityPercentforShow = json["Teleporting"]["Percent"];

		myBombAbilityTime = json["Bomb"]["Time"];
		myBombAbilityMinForce = json["Bomb"]["MinForce"];
		myBombAbilityMaxForce = json["Bomb"]["MaxForce"];
		myBombAbilityVerticalForce = json["Bomb"]["VerticalForce"];
		myBombAbilityHorizontalMultiplier = json["Bomb"]["HorizontalMultiplier"];
		myBombAbilityDrag = json["Bomb"]["Drag"];

		myBallonJumpForce = json["Ballon"]["Force"];
		myBallonJumpSpeedFalloffTime = json["Ballon"]["FalloffTime"];
		myBallonJumpMultiplier = json["Ballon"]["Multiplier"];

		myBoostMultiplier = json["Boost"]["Multiplier"];
		myBoostFalloffTime = json["Boost"]["FalloffTime"];
#endif
	}
}
void PlayerComponent::SaveData()
{
	nlohmann::json json;
	json["MaxHealth"] = myMaxHealth;
	json["Invicibility"] = myInvincibilityTime;
	json["Speed"] = mySpeed;
	json["JumpForce"] = myJumpForce;
	json["Gravity"] = myGravity;
	json["RotationSpeed"] = myRotationSpeed;
	json["CayoteTime"] = myCayoteTime;
	json["MaxHorizontalVelocity"] = myHorizontalMaxSpeed;
	json["MaxVerticalVelocity"] = myGravityMaxSpeed;
	json["CameraYOffset"] = myCameraYOffset;

	json["Sword"]["Damage"] = mySwordAbilityDamage;
	json["Sword"]["Cooldown"] = mySwordAbilityCooldown;
	json["Sword"]["Width"] = mySwordAbilityDamageWidth;
	json["Sword"]["Height"] = mySwordAbilityDamageHeight;
	json["Sword"]["DistanceAhead"] = mySwordAbilityDistanceAhead;

	json["DoubleJump"]["Force"] = myJumpAbilityForce;

	json["Dash"]["Speed"] = myDashAbilitySpeed;
	json["Dash"]["Time"] = myDashAbilityTime;
	json["Dash"]["FalloffTime"] = myDashAbilityFalloffTime;
	json["Dash"]["Width"] = myDashAbilityDamageWidth;
	json["Dash"]["Height"] = myDashAbilityDamageHeight;
	json["Dash"]["DistanceAhead"] = myDashAbilityDamageDistanceAhead;

	json["Stomp"]["Force"] = myStompAbilitySpeed;
	json["Stomp"]["Radius"] = myStompAbilityRadius;

	json["DirectionalDash"]["Speed"] = myDirectionalDashAbilitySpeed;
	json["DirectionalDash"]["Time"] = myDirectionalDashAbilityTime;
	json["DirectionalDash"]["FalloffTime"] = myDirectionalDashAbilityFalloffTime;
	json["DirectionalDash"]["Width"] = myDirectionalDashAbilityDamageWidth;
	json["DirectionalDash"]["Height"] = myDirectionalDashAbilityDamageHeight;
	json["DirectionalDash"]["DistanceAhead"] = myDirectionalDashAbilityDamageDistanceAhead;

	json["Grappling"]["Speed"] = myGrapplingAbilitySpeed;
	json["Grappling"]["Time"] = myGrapplingAbilityFalloffTime;
	json["Grappling"]["MaxDistance"] = myGrapplingAbilityMaxDistance;
	json["Grappling"]["ReleaseDistance"] = myGrapplingAbilityReleaseDistance;

	json["Teleporting"]["Speed"] = myTeleportingAbilitySpeed;
	json["Teleporting"]["Time"] = myTeleportingAbilityTime;
	json["Teleporting"]["FalloffTime"] = myTeleportingAbilityFalloffTime;
	json["Teleporting"]["MaxDistance"] = myTeleportingAbilityMaxDistance;
	json["Teleporting"]["ShowLength"] = myTeleportingAbilityShowLength;
	json["Teleporting"]["Radius"] = myTeleportingAbilityKillRadius;
	json["Teleporting"]["Percent"] = myTeleportingAbilityPercentforShow;

	json["Bomb"]["Time"] = myBombAbilityTime;
	json["Bomb"]["MinForce"] = myBombAbilityMinForce;
	json["Bomb"]["MaxForce"] = myBombAbilityMaxForce;
	json["Bomb"]["VerticalForce"] = myBombAbilityVerticalForce;
	json["Bomb"]["HorizontalMultiplier"] = myBombAbilityHorizontalMultiplier;
	json["Bomb"]["Drag"] = myBombAbilityDrag;

	json["Ballon"]["Force"] = myBallonJumpForce;
	json["Ballon"]["FalloffTime"] = myBallonJumpSpeedFalloffTime;
	json["Ballon"]["Multiplier"] = myBallonJumpMultiplier;

	json["Boost"]["Multiplier"] = myBoostMultiplier;
	json["Boost"]["FalloffTime"] = myBoostFalloffTime;

	std::ofstream file;
	file.open(STATIC_ASSET_PATH "json/playerSettings.json");
	if (file.is_open())
	{
		file << json.dump(4);
	}
	file.close();
}
#pragma endregion