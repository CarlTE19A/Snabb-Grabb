#pragma once
#include "component\Component.h"
#include "PhysX/foundation/px.h"
#include "PhysX/foundation/PxVec3.h"
#include "PhysX\characterkinematic\PxCapsuleController.h"
#include "PhysX\characterkinematic\PxControllerBehavior.h"
#include "PhysXCode/PhysicsFunctionCalls.h"
#include "math/Vector.h"
#include <functional>

//enum class eGroundedState
//{
//	eGrounded,
//	eWaterGrounded,
//	eBallonGrounded,
//	eEnemyGrounded,
//	eDeathFloorGrounded,
//	eNotGrounded
//};

class ControllerComponent : public Component, public physx::PxUserControllerHitReport, public PhysicsFunctionCalls, public physx::PxControllerBehaviorCallback
{
public:

	ControllerComponent();
	~ControllerComponent();

	void Update(const float aDeltaTime, Scene& aScene) override;
	void ImguiUpdate() override;
	void Reset() override;

	inline void SetDisplacement(const Vector3f& aDisplacement) { myDisplacement.x = aDisplacement.x; myDisplacement.y = aDisplacement.y; myDisplacement.z = aDisplacement.z; }
	inline void ClearDisplacement() { myDisplacement.x = 0; myDisplacement.y = 0; myDisplacement.z = 0; }

	void Teleport(const Vector3f& aPosition, bool teleportIsFootPos = false);

	//eGroundedState IsGrounded() const;
	physx::PxU32 IsGrounded() const;
	physx::PxU32 IsContact() const;

	physx::PxControllerBehaviorFlags getBehaviorFlags(const physx::PxShape& shape, const physx::PxActor& actor) override;
	physx::PxControllerBehaviorFlags getBehaviorFlags(const physx::PxController& controller) override;
	physx::PxControllerBehaviorFlags getBehaviorFlags(const physx::PxObstacle& obstacle) override;



private:

	physx::PxCapsuleController* myController;
	physx::PxVec3 myDisplacement;
	physx::PxControllerFilters myFilter;

	int myLastCollisionFlags = 0;

	//Grounded
	//bool myGrounded = false;
	physx::PxU32 myGroundedState = 0;
	physx::PxU32 myContactState = 0;

	//For PxUserControllerHitReport
	void onShapeHit(const physx::PxControllerShapeHit& hit) override;
	void onControllerHit(const physx::PxControllersHit& hit) override;
	void onObstacleHit(const physx::PxControllerObstacleHit& hit) override;

	physx::PxShape* myShape = nullptr;

	void SaveData();
	void LoadData();
};

//A character controller only moves a gameobject, it has no logic of its own