#pragma once

#include "component/Component.h"
#include "math/Transform.h"
namespace physx
{
	class PxShape;
}

class MeshComponent;

class DoorPartComponent : public Component
{
public:
	DoorPartComponent(MeshComponent* aMeshComponent);

	virtual ~DoorPartComponent();

	void OnCollision(physx::PxShape* aShape);
private:

	// Inherited via Component
	void Update(const float aDeltaTime, Scene& aScene) override;

	void ImguiUpdate() override;

	void Reset() override;

	MeshComponent* myMeshComponent;

	bool myStartFading = false;

	float myTimeToFade = 4;
	float myActiveTime = 2;

	bool myFade = false;
};