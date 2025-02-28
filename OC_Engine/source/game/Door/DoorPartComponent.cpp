#include "DoorPartComponent.h"
#include "gameObject/GameObject.h"
#include "component/mesh/MeshComponent.h"
#include "component/PhysicsComponent.h"
DoorPartComponent::DoorPartComponent(MeshComponent* aMeshComponent)
	:myMeshComponent(aMeshComponent)
{

}

DoorPartComponent::~DoorPartComponent()
{
}

void DoorPartComponent::OnCollision(physx::PxShape* aShape)
{
	aShape;
}

void DoorPartComponent::Update(const float aDeltaTime, Scene& )
{
	if (myTimeToFade <= 0 && !myStartFading)
	{
		myStartFading = true;
	}
	else if (!myStartFading)
	{
		myTimeToFade -= aDeltaTime;

	}
	else
	{
		myFade = !myFade;
		myMeshComponent->SetActive(myFade);
		myActiveTime -= aDeltaTime;

		if (myActiveTime<=0)
		{
			myParent->SetActive(false);
			myParent->GetComponent<PhysicsComponent>()->SetActive(false);
		}
	}
}

void DoorPartComponent::ImguiUpdate()
{
}

void DoorPartComponent::Reset()
{
	myStartFading = false;
	myTimeToFade = 4;
	myActiveTime = 2;
	myMeshComponent->SetActive(true);
	myFade = false;

}
