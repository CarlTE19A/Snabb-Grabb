#include "stdafx.h"
#include "PhysXManager.h"
#include "PhysX\PxPhysicsAPI.h"
#include "PhysX\PxRigidDynamic.h"
#include "PhysX\cudamanager\PxCudaContextManager.h"
#include "PhysX\extensions\PxDefaultCpuDispatcher.h"
#include "PhysX\characterkinematic\PxControllerManager.h"
#include "PhysX\cudamanager\PxCudaContext.h"

#include "component/PhysicsComponent.h"
#include "../game/Controller/ControllerComponent.h"
#include "../Paths.h"
#include "mainSingelton\MainSingelton.h"
#include "graphics\model\ModelFactory.h"
#include "graphics\GraphicsEngine.h"
//#ifdef _DEBUG
//#include "PhysX\pvd\PxPvd.h"
//#endif // _DEBUG
#include "PhysX\extensions\PxParticleExt.h"


using namespace physx;
PhysXManager::PhysXManager() : myFoundation(nullptr), myPhysics(nullptr), myPxScene(nullptr),
dispatcher(nullptr), defaultMaterial(nullptr), mySceneDesc(nullptr)
{
	myTotaltActorsInScene = 0;
}

PhysXManager::~PhysXManager()
{
	Shutdown();
	
	delete myCookingParams;
	myCookingParams = nullptr;

	//delete myControllerManager;
	//myControllerManager = nullptr;

	delete mySceneDesc;
	mySceneDesc = nullptr;

	delete dispatcher;
	dispatcher = nullptr;

	//delete defaultMaterial;
	//defaultMaterial = nullptr;
	//
	//delete myFoundation;
	//myFoundation = nullptr;
	//
	//delete cudaContextManager;
	//cudaContextManager = nullptr;

	delete myPhysics;
	myPhysics = nullptr;

	//delete myPxScene;
	//myPxScene = nullptr;

	delete particleSystem;
	particleSystem = nullptr;

	//delete particleBuffer;
	//particleBuffer = nullptr;
}

// Initialize PhysX
bool PhysXManager::Initialize()
{
	// Initialize the PhysX Foundation
	myFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, allocator, *this);

	if (!myFoundation)
	{
		// Handle initialization error
		return false;
	}
#ifdef _DEBUG

	PxPvd* pvd = PxCreatePvd(*myFoundation);
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	pvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

	myPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *myFoundation, physx::PxTolerancesScale(), true, pvd);
	if (!myPhysics)
	{
		return false;
	}
#else
	myPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *myFoundation, physx::PxTolerancesScale(), true);
	if (!myPhysics)
	{
		return false;
	}
#endif // _DEBUG
	PxCudaContextManagerDesc cudaContextManagerDesc;

	cudaContextManager = PxCreateCudaContextManager(*myFoundation, cudaContextManagerDesc, PxGetProfilerCallback());

	mySceneDesc = new PxSceneDesc(myPhysics->getTolerancesScale());
	mySceneDesc->gravity = PxVec3(0.0f, -981.f, 0.0f);
	dispatcher = PxDefaultCpuDispatcherCreate(2);
	mySceneDesc->cpuDispatcher = dispatcher;
	mySceneDesc->filterShader = FilterShader;
	mySceneDesc->kineKineFilteringMode = PxPairFilteringMode::eKEEP;
	mySceneDesc->staticKineFilteringMode = PxPairFilteringMode::eKEEP;

	mySceneDesc->simulationEventCallback = this;
	//mySceneDesc->filterCallback = this;
	mySceneDesc->broadPhaseType = PxBroadPhaseType::eABP;
	mySceneDesc->cudaContextManager = cudaContextManager;
	mySceneDesc->staticStructure = PxPruningStructureType::eDYNAMIC_AABB_TREE;
	//mySceneDesc->flags |= PxSceneFlag::eENABLE_PCM;
	mySceneDesc->flags |= PxSceneFlag::eENABLE_GPU_DYNAMICS;
	mySceneDesc->flags |= PxSceneFlag::eENABLE_CCD;
	//mySceneDesc->flags |= PxSceneFlag::
	mySceneDesc->broadPhaseType = PxBroadPhaseType::eGPU;
	mySceneDesc->solverType = PxSolverType::eTGS;

	myPxScene = myPhysics->createScene(*mySceneDesc);
	if (!myPxScene)
	{
		return false;
	}
	defaultMaterial = myPhysics->createMaterial(1.f, 1.f, 0.f);
	CreateFloor();

	myCookingParams = new physx::PxCookingParams(physx::PxTolerancesScale());

	myControllerManager = PxCreateControllerManager(*myPxScene);

	cudaContextManager = myPxScene->getCudaContextManager();

	return true;
}

void PhysXManager::Update(float deltaTime)
{
	if (!myPxScene)
		return;

	myAccumulator += deltaTime;

	if (myAccumulator < myTimeStep)
		return;

	myAccumulator -= myTimeStep;

	myPxScene->simulate(myTimeStep);
	myPxScene->fetchResults(true);
	myPxScene->fetchResultsParticleSystem();

	//Can skip giving contact data (used for respawn)
	if (skipFrame)
	{
		std::queue<PairData> empty;
		std::swap(myPairData, empty);
		skipFrame = false;
		return;
	}
	
	afterContact();
}

// Clean up and release resources
void PhysXManager::Shutdown()
{
	if (myControllerManager)
	{
		myControllerManager->release();
		myControllerManager = nullptr;
	}
	if (myPxScene)
	{
		myPxScene->release();
		myPxScene = nullptr;
	}
	if (dispatcher)
	{
		dispatcher->release();
		dispatcher = nullptr;
	}
	if (defaultMaterial)
	{
		defaultMaterial->release();
		defaultMaterial = nullptr;
	}
	if (cudaContextManager)
	{
		cudaContextManager->release();
		cudaContextManager = nullptr;
	}
	if (myPhysics)
	{
		myPhysics->release();
		myPhysics = nullptr;
	}
	if (myFoundation)
	{
		myFoundation->release();
		myFoundation = nullptr;
	}
}

void PhysXManager::SetSkipFrame()
{
	skipFrame = true;
}

physx::PxPhysics* PhysXManager::GetPhysics()
{
	return myPhysics;
}

physx::PxScene* PhysXManager::GetPxScene()
{
	return myPxScene;
}

physx::PxControllerManager* PhysXManager::GetControllerManager()
{
	return myControllerManager;
}

void PhysXManager::ResetPhysXScene()
{
	// Iterate through all actors in the scene and release them
	myTotaltActorsInScene = myPxScene->getNbActors(physx::PxActorTypeFlag::eRIGID_STATIC | physx::PxActorTypeFlag::eRIGID_DYNAMIC);

	physx::PxActor** tempActors = new physx::PxActor * [myTotaltActorsInScene];

	myPxScene->getActors(physx::PxActorTypeFlag::eRIGID_STATIC | physx::PxActorTypeFlag::eRIGID_DYNAMIC, tempActors, myTotaltActorsInScene);

	// Iterate through the array of actors and release them
	myPxScene->removeActors(tempActors, myTotaltActorsInScene);

	// Free the memory allocated for tempActors
	delete[] tempActors;
	tempActors = nullptr;

	CreateFloor();
}

physx::PxMaterial* PhysXManager::GetDefaultMaterial()
{
	return defaultMaterial;
}

void PhysXManager::AddActor(physx::PxActor& aActor)
{
	myPxScene->addActor(aActor);
}

void PhysXManager::RemoveActor(physx::PxActor& aActor)
{
	myPxScene->removeActor(aActor);
}

void PhysXManager::SetFilterOfShape(physx::PxShape* aShape, physx::PxU32 aFilterGroup, physx::PxU32 aFilterMask)
{
	physx::PxFilterData filterData;
	filterData.word0 = aFilterGroup; // word0 = own ID
	filterData.word1 = aFilterMask; // word1 = ID mask to filter pairs that trigger a contact callback;
	aShape->setSimulationFilterData(filterData);
	aShape->setQueryFilterData(filterData);
}

void PhysXManager::reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line)
{
	std::string physxError = std::to_string(code) + " " + std::string(message) + " " + std::string(file) + " " + std::to_string(line);
	//LogError(physxError);
	switch (code)
	{
	case physx::PxErrorCode::eNO_ERROR:
		Log("PhysX Error: " + physxError);
		break;
	case physx::PxErrorCode::eDEBUG_INFO:
		LogInfo("PhysX Info: " + physxError);
		break;
	case physx::PxErrorCode::eDEBUG_WARNING:
		LogWarning("PhysX Warning: " + physxError);
		break;
	case physx::PxErrorCode::eINVALID_PARAMETER:
		LogError("PhysX Invalid Parameter: " + physxError);
		break;
	case physx::PxErrorCode::eINVALID_OPERATION:
		LogError("PhysX Invalid Operation: " + physxError);
		break;
	case physx::PxErrorCode::eOUT_OF_MEMORY:
		LogError("PhysX Out of Memory: " + physxError);
		break;
	case physx::PxErrorCode::eINTERNAL_ERROR:
		LogError("PhysX Internal Error: " + physxError);
		break;
	case physx::PxErrorCode::eABORT:
		LogError("PhysX Abort: " + physxError);
		break;
	case physx::PxErrorCode::ePERF_WARNING:
		LogWarning("PhysX Performace Warning: " + physxError);
		break;
	case physx::PxErrorCode::eMASK_ALL:
		LogError("PhysX go cray cray, you stupid: " + physxError);
		break;
	default:
		break;
	}
}

void PhysXManager::CreateFloor()
{
	// Create a ground plane geometry (using a plane shape)
	physx::PxPlaneGeometry groundGeometry;

	// Create a transform for the ground's position and orientation
	physx::PxTransform groundTransform(physx::PxVec3(0.0f, 25.0f, 0.0f)); // Adjust the position as needed
	groundTransform.q = physx::PxQuat(physx::PxHalfPi, physx::PxVec3(0.0f, 0.0f, 1.0f));
	// Create a static rigid body for the ground
	physx::PxRigidStatic* groundPlane = myPhysics->createRigidStatic(groundTransform);

	// Create a shape for the ground using the ground geometry
	physx::PxShape* groundShape = myPhysics->createShape(groundGeometry, *defaultMaterial);
	SetFilterOfShape(groundShape, eDEATHFLOOR, 0);
	groundPlane->attachShape(*groundShape);

	// Add the ground plane to the scene (assuming you have a 'scene' variable)
	myPxScene->addActor(*groundPlane);
	return;
}

// Filter shader for collision filtering
physx::PxFilterFlags FilterShader(physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0, physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1, physx::PxPairFlags& pairFlags, const void* constantBlock, physx::PxU32 constantBlockSize)
{
	attributes0;
	attributes1;
	filterData0;
	filterData1;
	constantBlock;
	constantBlockSize;
	//pairFlags = PxPairFlag::eCONTACT_DEFAULT | PxPairFlag::eTRIGGER_DEFAULT;
//return PxFilterFlag::eDEFAULT;
	// let triggers through
	if (PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1))
	{
		pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
		pairFlags |= PxPairFlag::eDETECT_CCD_CONTACT;
		return PxFilterFlag::eDEFAULT;
	}

	if (PxFilterObjectIsKinematic(attributes0) && PxFilterObjectIsKinematic(attributes1))
	{	//
		//if (filterData0.word0 & PhysXManager::FilterGroup::ePLAYER || filterData1.word0 & PhysXManager::FilterGroup::ePLAYER)
		//{
		//	pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
		//	return PxFilterFlag::eDEFAULT;
		//}
		pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
		pairFlags |= PxPairFlag::eDETECT_CCD_CONTACT;
		return PxFilterFlag::eDEFAULT;
	}

	// generate contacts for all that were not filtered above
	pairFlags = PxPairFlag::eCONTACT_DEFAULT;
	pairFlags |= PxPairFlag::eDETECT_CCD_CONTACT;

	////If Eney bullet and player bullet
	//if (filterData0.word0 & PhysXManager::FilterGroup::eENEMYBULLET || filterData1.word0 & PhysXManager::FilterGroup::ePLAYERBULLET || filterData1.word0 & PhysXManager::FilterGroup::eCARD)
	//{
	//	pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
	//	return PxFilterFlag::eDEFAULT;
	//	//Log("bullet collison");
	//}
	// trigger the contact callback for pairs (A,B) where
	// the filtermask of A contains the ID of B  and vice versa.
	if ((filterData0.word0 & filterData1.word1) || (filterData1.word0 & filterData0.word1))
	{
		pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;

	}

	return PxFilterFlag::eDEFAULT;
}

#pragma region PxSimulationFilterCallback
/*
PxFilterFlags PhysXManager::pairFound(PxU64 pairID, PxFilterObjectAttributes attributes0, PxFilterData filterData0, const PxActor* a0, const PxShape* s0, PxFilterObjectAttributes attributes1, PxFilterData filterData1, const PxActor* a1, const PxShape* s1, PxPairFlags& pairFlags)
{
	pairID;
	attributes0;
	filterData0;
	a0;
	s0;
	attributes1;
	filterData1;
	a1;
	s1;
	pairFlags;

	//PairData data;
	//data.myActor0 = a0;
	//data.myActor1 = a1;
	//data.myAttributes0 = attributes0;
	//data.myAttributes1 = attributes1;
	//data.myFilterData0 = filterData0;
	//data.myFilterData1 = filterData1;
	//data.myPairFlags = pairFlags;
	//data.myPairID = pairID;
	//data.myShape0 = s0;
	//data.myShape1 = s1;
	//myActivePairs.push_back(data);
	return PxFilterFlags();
}

void PhysXManager::pairLost(PxU64 pairID, PxFilterObjectAttributes attributes0, PxFilterData filterData0, PxFilterObjectAttributes attributes1, PxFilterData filterData1, bool objectRemoved)
{
	//myActivePairs.erase(std::remove_if(myActivePairs.begin(), myActivePairs.end(), [&](PairData& aData) { return aData.myPairID == pairID; }), myActivePairs.end());
	pairID;
	attributes0;
	attributes1;
	filterData0;
	filterData1;
	objectRemoved;
	return;
}

bool PhysXManager::statusChange(PxU64& pairID, PxPairFlags& pairFlags, PxFilterFlags& filterFlags)
{
	pairID;
	pairFlags;
	filterFlags;
	return false;
}
*/
#pragma endregion
#pragma region PxSimulationEventCallback


void PhysXManager::onConstraintBreak(physx::PxConstraintInfo* constraints, physx::PxU32 count)
{
	constraints;
	count;
	return;
}

void PhysXManager::onWake(physx::PxActor** actors, physx::PxU32 count)
{
	actors;
	count;
	return;
}

void PhysXManager::onSleep(physx::PxActor** actors, physx::PxU32 count)
{
	actors;
	count;
	return;

}

void PhysXManager::onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs)
{
	//myContactPairHeaders.push(pairHeader);
	//myContactPairs.push(pairs);
	//myNumPairs.push(nbPairs);
	myPairData.push(PairData(pairHeader, pairs, nbPairs));

	//LogInfo("PhysXManager::onContact");
}

void PhysXManager::onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count)
{
	pairs;
	count;
	LogInfo("PhysXManager::onTrigger");
	return;

}

void PhysXManager::onAdvance(const physx::PxRigidBody* const* bodyBuffer, const physx::PxTransform* poseBuffer, const physx::PxU32 count)
{
	bodyBuffer;
	poseBuffer;
	count;
	return;
}
bool PhysXManager::Raycast(const Vector3f& aOrigin, const Vector3f& aDirection, const float aDistance, physx::PxRaycastHit& hitInfo, unsigned int filter, physx::PxQueryFlags aFlags)
{
	physx::PxRaycastBuffer hit;
	physx::PxQueryFilterData filterData;
	filterData.flags = aFlags;
	filterData.data.word0 = filter;

	if(myPxScene->raycast(physx::PxVec3(aOrigin.x, aOrigin.y, aOrigin.z), physx::PxVec3(aDirection.x, aDirection.y, aDirection.z).getNormalized(), aDistance, hit, physx::PxHitFlag::eDEFAULT, filterData))
	{
		hitInfo = hit.block;
		if(hitInfo.actor != nullptr)
		{
			//Log(std::to_string(hitInfo.distance));
			return true;
		}
		//aCallback.processTouches(hit.getTouches(), hit.getNbTouches());
		//return aCallback.hasBlock;
	}
	return false;
}
bool PhysXManager::Sweep(const physx::PxGeometry& aGeometry, const physx::PxTransform& aPose, const Vector3f& aDirection, const float aDistance, physx::PxSweepHit hitInfo, unsigned int filter, physx::PxQueryFlags aFlags)
{
	physx::PxSweepBuffer hit;
	physx::PxQueryFilterData filterData;
	
	filterData.flags = aFlags;
	filterData.data.word1 = filter;
	if (myPxScene->sweep(aGeometry, aPose, physx::PxVec3(aDirection.x, aDirection.y, aDirection.z).getNormalized(), aDistance, hit, physx::PxHitFlag::ePOSITION, filterData))
	{
		hitInfo = hit.block;
		if (hitInfo.actor != nullptr)
		{
			return true;
		}
		//aCallback.processTouches(hit.getTouches(), hit.getNbTouches());
		//return aCallback.hasBlock;
	}
	return false;
}
bool PhysXManager::Overlap(const physx::PxGeometry& aGeometry, const physx::PxTransform& aPose, physx::PxOverlapCallback& aCallback, unsigned int filter, physx::PxQueryFlags aFlags)
{
	physx::PxOverlapBuffer hit;
	physx::PxQueryFilterData filterData;
	filterData.flags = aFlags;
	filterData.data.word1 = filter;
	if (myPxScene->overlap(aGeometry, aPose, hit, filterData))
	{
		aCallback.processTouches(hit.getTouches(), hit.getNbTouches());
		return aCallback.hasBlock;
	}
	return false;
}
void PhysXManager::afterContact()
{
	while (!myPairData.empty())
	{
		//const physx::PxContactPairHeader& pairHeader = myPairData.front().myPairHeader;
		const physx::PxContactPair* pairs = myPairData.front().myPairs;
		physx::PxU32 nbPairs = myPairData.front().nbPairs;

		for (int i = 0; i < (int)nbPairs; i++)
		{
			physx::PxFilterData filterData0 = pairs[i].shapes[0]->getSimulationFilterData();
			physx::PxFilterData filterData1 = pairs[i].shapes[1]->getSimulationFilterData();
			if(filterData0.word0 & filterData1.word1)
			{
				
				try { static_cast<PhysicsComponent*>(pairs[i].shapes[1]->userData)->CallCollisionFunction(pairs[i].shapes[0]); }
				catch (const std::exception&) { LogWarning("failed cast to physics 1"); }
				try { static_cast<ControllerComponent*>(pairs[i].shapes[1]->userData)->CallCollisionFunction(pairs[i].shapes[0]); }
				catch (const std::exception&) { LogWarning("failed cast to controller 1"); }
			}
			if (filterData1.word0 & filterData0.word1)
			{
				try { static_cast<PhysicsComponent*>(pairs[i].shapes[0]->userData)->CallCollisionFunction(pairs[i].shapes[1]); }
				catch (const std::exception&) { LogWarning("failed cast to physics 0"); }
				try { static_cast<ControllerComponent*>(pairs[i].shapes[0]->userData)->CallCollisionFunction(pairs[i].shapes[1]); }
				catch (const std::exception&) { LogWarning("failed cast to controller 0"); }
			}

			//if (filterData0.word0 & PhysXManager::FilterGroup::eENEMYBULLET && filterData1.word0 & PhysXManager::FilterGroup::ePLAYER)
			//{
			//	PhysicsComponent* physicsComponent = static_cast<PhysicsComponent*>(pairs[i].shapes[0]->userData);
			//	ControllerComponent* controllerComponent = static_cast<ControllerComponent*>(pairs[i].shapes[1]->userData);
			//	Log("bullet collison bullet to player");
			//	physicsComponent->CallCollisionFunction(pairs[i].shapes[1]);
			//	controllerComponent->CallCollisionFunction(pairs[i].shapes[0]);
			//}
			//if (filterData1.word0 & PhysXManager::FilterGroup::eENEMYBULLET && filterData0.word0 & PhysXManager::FilterGroup::ePLAYER)
			//{
			//	PhysicsComponent* physicsComponent = static_cast<PhysicsComponent*>(pairs[i].shapes[1]->userData);
			//	ControllerComponent* controllerComponent = static_cast<ControllerComponent*>(pairs[i].shapes[0]->userData);
			//	physicsComponent->CallCollisionFunction(pairs[i].shapes[0]);
			//	controllerComponent->CallCollisionFunction(pairs[i].shapes[1]);
			//	Log("bullet collison 1->0");
			//}  
		}
		myPairData.pop();
	}
}
#pragma endregion

//SweepCallback::SweepCallback(physx::PxSweepHit* aBuffer, int maxHits) : 
//	physx::PxSweepCallback(aBuffer, physx::PxU32(maxHits))
//{
//
//}
//
//physx::PxAgain SweepCallback::processTouches(const physx::PxSweepHit* hit , physx::PxU32 nbHits)
//{
//	for (int i = 0; i < (int)nbHits; i++)
//	{
//		float distance = hit->distance;
//		physx::PxHitFlags flags = hit->flags;
//		LogInfo("Hit " + std::to_string(i) + " (distance: " + std::to_string(distance) + ", flag: " + std::to_string(flags) + ")");
//	}
//	return false;
//}
