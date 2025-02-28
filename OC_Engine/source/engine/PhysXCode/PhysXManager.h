#pragma once
#include "PhysX\PxPhysicsAPI.h"
#include "PhysX\characterkinematic\PxControllerManager.h"
#include "PhysX\cooking\PxCooking.h"
#include <d3d11.h>
#include <vector>
#include <queue>

physx::PxFilterFlags FilterShader(
	physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0,
	physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1,
	physx::PxPairFlags& pairFlags, const void* constantBlock, physx::PxU32 constantBlockSize);

struct PairData
{
	const physx::PxContactPairHeader& myPairHeader;
	const physx::PxContactPair* myPairs;
	physx::PxU32 nbPairs;

	PairData(const physx::PxContactPairHeader& aPairHeader, const physx::PxContactPair* aPairs, physx::PxU32 aNbPairs)
		: myPairHeader(aPairHeader), myPairs(aPairs), nbPairs(aNbPairs)
	{}
};

class PhysXManager : public physx::PxSimulationEventCallback, /*public physx::PxSimulationFilterCallback,*/ public physx::PxErrorCallback
{
public:
	// Constructor
	PhysXManager();
	// Destructor
	~PhysXManager();
	// Initialize PhysX
	bool Initialize();
	// Update myPhysics simulation
	void Update(float deltaTime);
	// Clean up and release resources
	void Shutdown();

	void SetSkipFrame();

	physx::PxPhysics* GetPhysics();
	physx::PxScene* GetPxScene();
	physx::PxControllerManager* GetControllerManager();
	void ResetPhysXScene();

	physx::PxMaterial* GetDefaultMaterial();

	physx::PxCudaContextManager* GetCudaContext() { return cudaContextManager; }

	physx::PxCookingParams& GetPxCookingParams() { return *myCookingParams; }

	void AddActor(physx::PxActor& aActor);
	void RemoveActor(physx::PxActor& aActor);

	//For PxSimulationEventCallback
	void onConstraintBreak(physx::PxConstraintInfo* constraints, physx::PxU32 count) override;
	void onWake(physx::PxActor** actors, physx::PxU32 count) override;
	void onSleep(physx::PxActor** actors, physx::PxU32 count) override;
	void onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs) override;
	void onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count) override;
	void onAdvance(const physx::PxRigidBody* const* bodyBuffer, const physx::PxTransform* poseBuffer, const physx::PxU32 count) override;

	bool Raycast(const Vector3f& aOrigin, const Vector3f& aDirection, const float aDistance, physx::PxRaycastHit& hitInfo, unsigned int filter, physx::PxQueryFlags aFlags = physx::PxQueryFlag::eSTATIC | physx::PxQueryFlag::eDYNAMIC );
	bool Sweep(const physx::PxGeometry& aGeometry, const physx::PxTransform& aPose, const Vector3f& aDirection, const float aDistance, physx::PxSweepHit hitInfo, unsigned int filter, physx::PxQueryFlags aFlags = physx::PxQueryFlag::eSTATIC | physx::PxQueryFlag::eDYNAMIC);
	bool Overlap(const physx::PxGeometry& aGeometry, const physx::PxTransform& aPose, physx::PxOverlapCallback& aCallback, unsigned int filter, physx::PxQueryFlags aFlags = physx::PxQueryFlag::eSTATIC | physx::PxQueryFlag::eDYNAMIC);

	//Goes thorugh all the pairs and calls the correct function
	void afterContact();
	//std::queue<const physx::PxContactPairHeader &> myContactPairHeaders;
	//std::queue<const physx::PxContactPair*> myContactPairs;
	//std::queue<physx::PxU32> myNumPairs;
	std::queue<PairData> myPairData;


	//For PxSimulationFilterCallback
	/*
	physx::PxFilterFlags pairFound(physx::PxU64 pairID,
		physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0, const physx::PxActor* a0, const physx::PxShape* s0,
		physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1, const physx::PxActor* a1, const physx::PxShape* s1,
		physx::PxPairFlags& pairFlags) override;

	void pairLost(physx::PxU64 pairID,
		physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0,
		physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1,
		bool objectRemoved) override;

	bool statusChange(physx::PxU64& pairID, physx::PxPairFlags& pairFlags, physx::PxFilterFlags& filterFlags) override;
	*/
	//For PxErrorCallback
	void reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line) override;

	enum FilterGroup
	{
		ePLAYER = (1 << 0),
		eGROUNDENEMY = (1 << 1),
		eGROUND = (1 << 2),
		//eDYNAMIC = (1 << 3),
		eDEATHFLOOR = (1 << 3),
		//ePICKUP = (1 << 5),
		eBALLONENEMY = (1 << 4),
		ePLAYERATTACK = (1 << 5),
		eENEMYBULLET = (1 << 6),
		eCARD = (1 << 7),
		eBOOST = (1 << 8),
		eGOAL = (1 << 9),
		eTRIGGER = (1 << 10),
		eDOOR = (1 << 11),
		eDOORPART = (1 << 12),
		ePLAYERDESTROY = (1 << 13),
		eBOMB = (1 << 14),
	};
	//Universal PhysX Functions
	static void SetFilterOfShape(physx::PxShape* aShape, physx::PxU32 aFilterGroup, physx::PxU32 aFilterMask);
private:
	float myTimeStep = 1.0f / 60.0f;
	float myAccumulator = 0.0f;

	physx::PxDefaultAllocator allocator;

	physx::PxCudaContextManager* cudaContextManager;
	physx::PxFoundation* myFoundation;
	physx::PxSceneDesc* mySceneDesc;
	physx::PxPhysics* myPhysics;
	physx::PxScene* myPxScene;

	physx::PxPBDParticleSystem* particleSystem;
	physx::PxParticleAndDiffuseBuffer* particleBuffer;

	//physx::PxSimulationEventCallback *myCollisionCallback;

	physx::PxDefaultCpuDispatcher* dispatcher;
	physx::PxMaterial* defaultMaterial;
	physx::PxControllerManager* myControllerManager;
	physx::PxU32 myTotaltActorsInScene;
	//Ground Floor
	void CreateFloor();

	bool skipFrame = false;

	physx::PxCookingParams* myCookingParams;
	//dispatcher(nullptr), defaultMaterial(nullptr), myCookingParams(nullptr), myControllerManager(nullptr)

};

//class SweepCallback : public physx::PxSweepCallback
//{
//public:
//	SweepCallback(physx::PxSweepHit* aBuffer, int maxHits);
//	~SweepCallback() {};
//
//	physx::PxAgain processTouches(const physx::PxSweepHit*, physx::PxU32) override;
//};