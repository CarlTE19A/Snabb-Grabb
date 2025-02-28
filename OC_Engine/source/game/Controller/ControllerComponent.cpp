#include "ControllerComponent.h"
#include "component/PhysicsComponent.h"
//#include "PhysX\common\PxBase.h"
#include "PhysX\characterkinematic\PxCapsuleController.h"
#include "PhysX\characterkinematic\PxControllerManager.h"
#include "PhysX/geometry/PxGeometry.h"

#include "PhysXCode/PhysXManager.h"
#include "mainSingelton\MainSingelton.h"
#include "PhysXCode\PhysXManager.h"
#include "gameObject\GameObject.h"

#include <../game/EnemyComponent/EnemyPool.h>
#include <../game/EnemyComponent/BalloonComponent.h>

#include "Log.h"

//To Save Controller Settings
#include "nlohmann/json.hpp"
#include <fstream>
#include "Paths.h"
ControllerComponent::ControllerComponent()
{
	PhysXManager& physXManager = MainSingleton::GetInstance().GetPhysXManager();
	physx::PxCapsuleControllerDesc desc;
	desc.setToDefault();
	desc.climbingMode = physx::PxCapsuleClimbingMode::eEASY;
	desc.contactOffset = 0.1f;
	desc.density = 10.0f;
	desc.height = 106.2f;
	desc.invisibleWallHeight = 0.0f;
	desc.material = physXManager.GetPhysics()->createMaterial(0.5f, 0.5f, 0.1f);
	desc.maxJumpHeight = 100.0f;
	desc.nonWalkableMode = physx::PxControllerNonWalkableMode::ePREVENT_CLIMBING_AND_FORCE_SLIDING;
	desc.radius = 50.0f;
	desc.slopeLimit = physx::PxCos(physx::PxDegToRad(5.f));
	desc.stepOffset = 5.0f;
	desc.volumeGrowth = 1.5f;
	desc.reportCallback = this;
	desc.behaviorCallback = this;
	//physx::PxControllerBehaviorCallback
	myController = static_cast<physx::PxCapsuleController*>(physXManager.GetControllerManager()->createController(desc));

	//Hitable Objects
			int filter =
		PhysXManager::FilterGroup::eGROUND |
		PhysXManager::FilterGroup::eGROUNDENEMY |
		PhysXManager::FilterGroup::eDEATHFLOOR |
		PhysXManager::FilterGroup::eDOOR;

	myFilter.mFilterData = new physx::PxFilterData(filter, 0, 0, 0); // = physx::PxQueryFlag::eSTATIC;

	physx::PxShape** shape = new physx::PxShape * ();
	physx::PxU32 nbShapes = myController->getActor()->getShapes(shape, 1);
	myController->getActor()->getShapes(shape, nbShapes);  // [in] shape to test for overlaps
	myShape = shape[0];
	//Send Message Filter
	physXManager.SetFilterOfShape(myShape, PhysXManager::FilterGroup::ePLAYER,
		PhysXManager::FilterGroup::eGROUND |
		PhysXManager::FilterGroup::eBOOST |
		PhysXManager::FilterGroup::eGROUNDENEMY |
		PhysXManager::FilterGroup::eBALLONENEMY |
		PhysXManager::FilterGroup::eENEMYBULLET |
		PhysXManager::FilterGroup::eTRIGGER |
		PhysXManager::FilterGroup::eCARD |
		PhysXManager::FilterGroup::eDOOR |
		PhysXManager::FilterGroup::eGOAL |
		PhysXManager::FilterGroup::eBOMB
	);
	shape[0]->userData = this;

	LoadData();
}

ControllerComponent::~ControllerComponent()
{
}

void ControllerComponent::Update(const float aDeltaTime, Scene& aScene)
{
	aScene;
	aDeltaTime;

	Vector3f priorPosition = myParent->GetPosition();
	Vector3f newPosition;

	//myGrounded = false;
	myGroundedState = 0;
	myContactState = 0;
	myController->move(myDisplacement, 0, aDeltaTime, myFilter);
	physx::PxExtendedVec3 pos = myController->getPosition();
	newPosition = Vector3f((float)pos.x, (float)pos.y, (float)pos.z);



	myParent->SetPosition(newPosition);
}

void ControllerComponent::Reset()
{
	myGroundedState = 0;
	myContactState = 0;
}

void ControllerComponent::Teleport(const Vector3f& aPosition, bool teleportIsFootPos)
{
	if (teleportIsFootPos)
	{
		physx::PxExtendedVec3 pos = physx::PxExtendedVec3(aPosition.x, aPosition.y, aPosition.z);
		myController->setFootPosition(pos);
		pos = myController->getPosition();
		myParent->SetPosition(Vector3f((float)pos.x, (float)pos.y, (float)pos.z));
	}
	else
	{
		myController->setPosition(physx::PxExtendedVec3(aPosition.x, aPosition.y, aPosition.z));
		myParent->SetPosition(aPosition);
	}
}

physx::PxU32 ControllerComponent::IsGrounded() const
{
	return myGroundedState;
}

physx::PxU32 ControllerComponent::IsContact() const
{
	return myContactState;
}

physx::PxControllerBehaviorFlags ControllerComponent::getBehaviorFlags(const physx::PxShape&, const physx::PxActor&)
{
	physx::PxControllerBehaviorFlags flags = physx::PxControllerBehaviorFlag::eCCT_SLIDE;
	return flags;
}

physx::PxControllerBehaviorFlags ControllerComponent::getBehaviorFlags(const physx::PxController&)
{
	physx::PxControllerBehaviorFlags flags = physx::PxControllerBehaviorFlag::eCCT_SLIDE;
	return flags;
}

physx::PxControllerBehaviorFlags ControllerComponent::getBehaviorFlags(const physx::PxObstacle&)
{
	physx::PxControllerBehaviorFlags flags = physx::PxControllerBehaviorFlag::eCCT_SLIDE;
	return flags;
}

void ControllerComponent::onShapeHit(const physx::PxControllerShapeHit& hit)
{
	physx::PxFilterData filterData = hit.shape->getSimulationFilterData();
	myLastCollisionFlags = filterData.word0;

	void* actorData = hit.actor->userData;
	if (actorData == nullptr)
	{
		LogWarning("onControllerHit ControllerComponent : hit should have actor userData");
	}
	myContactState |= filterData.word0;
	if (hit.worldNormal.y > 0.55f)
	{
		myGroundedState |= filterData.word0;
	}
	if (filterData.word0 & PhysXManager::FilterGroup::eBALLONENEMY)
	{
		PhysicsComponent* physicsComponent = static_cast<PhysicsComponent*>(actorData);
		GameObject* gameObject = physicsComponent->GetOwner();
		float health = gameObject->GetComponent<EnemyComponent>()->GetHealth();
		gameObject->GetComponent<EnemyComponent>()->TakeDamage(health);
	}
	//if (filterData.word0 & PhysXManager::FilterGroup::eBOOST)
	//{
	//	//Log("Boost");
	//
	//}
}

void ControllerComponent::onControllerHit(const physx::PxControllersHit& hit)
{
	hit;
	LogWarning("onControllerHit ControllerComponent : Should be no other Controllers");
}

void ControllerComponent::onObstacleHit(const physx::PxControllerObstacleHit& hit)
{
	hit;
	Log("onControllerHit ControllerComponent : Hit Obstacle");
}

#pragma region SaveLoad
void ControllerComponent::ImguiUpdate()
{
	//myController->setPosition(physx::PxExtendedVec3(myParent->GetTransform()->GetPosition().x, myParent->GetTransform()->GetPosition().y, myParent->GetTransform()->GetPosition().z));
	Teleport(myParent->GetPosition());
	if (ImGui::BeginTabItem("Controller"))
	{
		physx::PxF32 number = myController->getContactOffset();
		ImGui::DragFloat("Contact Offset", &number, 0.1f);
		myController->setContactOffset(number);

		number = physx::PxRadToDeg(physx::PxAcos(myController->getSlopeLimit()));
		ImGui::DragFloat("Slope Limit", &number, 0.1f);
		myController->setSlopeLimit(physx::PxCos(physx::PxDegToRad(number)));

		number = myController->getStepOffset();
		ImGui::DragFloat("Step Offset", &number, 0.1f);
		myController->setStepOffset(number);

		ImGui::BeginDisabled();
		int grounded = myGroundedState;
		int contact = myContactState;
		ImGui::InputInt("Grounded", &grounded);
		ImGui::InputInt("Contact", &contact);
		//ImGui::Checkbox("Grounded", &myGrounded);

		physx::PxExtendedVec3 numbersVec = myController->getFootPosition();
		float numbers[3] = { (float)numbersVec.x, (float)numbersVec.y, (float)numbersVec.z };
		ImGui::DragFloat3("Foot Position", numbers, 0.1f);

		numbers[0] = myDisplacement.x;
		numbers[1] = myDisplacement.y;
		numbers[2] = myDisplacement.z;
		ImGui::DragFloat3("Displacement", numbers, 0.1f);

		ImGui::EndDisabled();
		ImGui::SeparatorText("Capsule");
		number = myController->getHeight();
		ImGui::DragFloat("Height", &number, 0.1f, 0.01f);
		myController->resize(number);

		number = myController->getRadius();
		ImGui::DragFloat("Radius", &number, 0.1f, 0.01f);
		myController->setRadius(number);

		ImGui::Separator();
		if (ImGui::Button("Save Settings"))
		{
			SaveData();
		}
		if (ImGui::Button("Load Settings"))
		{
			LoadData();
		}

		ImGui::EndTabItem();
	}
}

void ControllerComponent::LoadData()
{
	std::ifstream file;
	file.open(STATIC_ASSET_PATH "json/controllerSettings.json");
	if (file.is_open())
	{
		nlohmann::json json;
		file >> json;
#ifdef _DEBUG

		try { myController->setContactOffset(json["ContactOffset"]); }
		catch (const std::exception&) { myController->setContactOffset(0.1f); }
		try { myController->setSlopeLimit(physx::PxCos(physx::PxDegToRad(json["SlopeLimit"]))); }
		catch (const std::exception&) { myController->setSlopeLimit(physx::PxCos(physx::PxDegToRad(30.f))); }
		try { myController->setStepOffset(json["StepOffset"]); }
		catch (const std::exception&) { myController->setStepOffset(5.0f); }
		try { myController->resize(json["Height"]); }
		catch (const std::exception&) { myController->resize(106.2f); }
		try { myController->setRadius(json["Radius"]); }
		catch (const std::exception&) { myController->setRadius(50.0f); }

#else
		myController->setContactOffset(json["ContactOffset"]);
		myController->setSlopeLimit(physx::PxCos(physx::PxDegToRad(json["SlopeLimit"])));
		myController->setStepOffset(json["StepOffset"]);
		myController->resize(json["Height"]);
		myController->setRadius(json["Radius"]);
#endif
	}
}
void ControllerComponent::SaveData()
{
	nlohmann::json json;

	json["ContactOffset"] = myController->getContactOffset();
	json["SlopeLimit"] = physx::PxRadToDeg(physx::PxAcos(myController->getSlopeLimit()));
	json["StepOffset"] = myController->getStepOffset();
	json["Height"] = myController->getHeight();
	json["Radius"] = myController->getRadius();

	std::ofstream file;
	file.open(STATIC_ASSET_PATH "json/controllerSettings.json");
	if (file.is_open())
	{
		file << json.dump(4);
	}
	file.close();
}
#pragma region SaveLoad