#include "ExplosionComponent.h"
#include "gameObject/GameObject.h"
#include "ExplosionPool.h"
#include "Log.h"
#include "component/PhysicsComponent.h"

ExplosionComponent::ExplosionComponent()
{
	myGeometry = new physx::PxSphereGeometry();
	myGeometry->radius = 300.f;
	myElapsedTime = 0;
	myActiveTime = 0;
	myMaxSize = 0;
	myReturnExplosion = false;
	myIndex = 0;
	myPoolOwner = nullptr;
	myActor = nullptr;
	myParent = nullptr;
}
ExplosionComponent::~ExplosionComponent()
{
}
void ExplosionComponent::Update(const float aDeltaTime, Scene&)
{
	aDeltaTime;
	//PhysicsComponent* physicsComponent = myParent->GetComponent<PhysicsComponent>();
		
	//const physx::PxSphereGeometry* constGeo = reinterpret_cast<const physx::PxSphereGeometry*>(&physicsComponent->GetShape()->getGeometry());
	//Log("ExplosionComponent::Update " + std::to_string(myGeometry->radius));

	//physx::PxSphereGeometry sphereGeometry(myMaxSize * (myElapsedTime / myActiveTime));
	myElapsedTime += aDeltaTime;
	if (myElapsedTime > myActiveTime)
	{
		//myGeometry->radius = 0.1f;
		myPoolOwner->ReturnExplosion(myIndex);
	}
	//else
	//{
	//	myGeometry->radius = myMaxSize * (myElapsedTime / myActiveTime);
	//}
	
	
	myActor->setGlobalPose(physx::PxTransform(myParent->GetTransform()->GetPxMat44()));
	//myActor->set
}

void ExplosionComponent::SetPoolIndex(const int aIndex)
{
	myIndex = aIndex;
}

bool ExplosionComponent::IsItExpired()
{
	if (myReturnExplosion || myElapsedTime >= myActiveTime)
	{
		myElapsedTime = 0;
		return true;
	}
	return false;
}

int ExplosionComponent::GetPoolIndex() const
{
	return myIndex;
}

void ExplosionComponent::SetPoolOwner(ExplosionPool* aOwnerPool)
{
	myPoolOwner = aOwnerPool;
}

void ExplosionComponent::SetActor(physx::PxRigidDynamic* aActor)
{
	myActor = aActor;
}

ExplosionPool* ExplosionComponent::GetOwnerPool()
{
	return myPoolOwner;
}

void ExplosionComponent::ExplosionReset()
{
	myReturnExplosion = false;
	myElapsedTime = 0;
	myMaxSize = 0;
	myGeometry->radius = 0.2f;
}

//void ExplosionComponent::Shoot(const Vector3f aPosition, const Vector3f aDirection)
//{
//	myElapsedTime = 0;
//	Transform transform;
//	myDirection = aDirection.GetNormalized();
//	transform.SetPosition(aPosition);
//	transform.RotateTowards(aPosition + aDirection, 1000);
//	myParent->SetPosition(aPosition);
//	myActor->setGlobalPose(physx::PxTransform(transform.GetPxMat44()));
//	//myActor->addForce({ aDirection.x * aSpeed,aDirection.y * aSpeed,aDirection.z * aSpeed }, physx::PxForceMode::eIMPULSE);
//}

void ExplosionComponent::Spawn(const Vector3f aPosition, const float aTime, const float aMaxRadius)
{
	myElapsedTime = 0;
	myActiveTime = aTime;
	myMaxSize = aMaxRadius;
	myParent->SetPosition(aPosition);
	myActor->setGlobalPose(physx::PxTransform(myParent->GetTransform()->GetPxMat44()));
	myGeometry->radius = 0.3f;

	//PhysicsComponent* physicsComponent = myParent->GetComponent<PhysicsComponent>();
	//physicsComponent->AddGeometry(*myGeometry, false, false);

	//const physx::PxSphereGeometry* constGeo = reinterpret_cast<const physx::PxSphereGeometry*>(&physicsComponent->GetShape()->getGeometry());
	//physx::PxSphereGeometry& test = static_cast<const physx::PxSphereGeometry*>(&physicsComponent->GetShape()->getGeometry());
	//physicsComponent->AddGeometry(*myGeometry,false, false);
	//myGeometry = &constGeo;
	
	//memcpy(myGeometry, constGeo, sizeof(physx::PxSphereGeometry));
	//physx::PxGeometry* geo = &myGeometry;
	//physicsComponent->AddGeometry(myGeometry);
	//if (&physicsComponent->GetShape()->getGeometry() == geo)
	//{
	//	Log("ExplosionComponent::Update " + std::to_string(myGeometry.radius));
	//}
	//if(physicsComponent->GetShape()->getGeometry().getType() == physx::PxGeometryType::eSPHERE)
	//{
	//	physx::PxSphereGeometry* sphere = static_cast<physx::PxSphereGeometry*>(geo);
	//	sphere->radius = myGeometry.radius;
	//}

}

void ExplosionComponent::OnCollision(physx::PxShape* aShape)
{
	physx::PxFilterData filterData = aShape->getQueryFilterData();
	//myPoolOwner->ReturnExplosion(myIndex);
	myReturnExplosion = true;
	aShape;
}

void ExplosionComponent::ImguiUpdate()
{
	if (ImGui::BeginTabItem("Explosion"))
	{
		ImGui::Text("Elapsed Time %f", myElapsedTime);
		ImGui::Text("Active Time %f", myActiveTime);
		//ImGui::Text("Current Size %f", myGeometry.radius);
		ImGui::Text("Max Size %f", myMaxSize);
		ImGui::EndTabItem();
	}
}

void ExplosionComponent::Reset()
{
	myPoolOwner->ReturnExplosion(myIndex);
}
