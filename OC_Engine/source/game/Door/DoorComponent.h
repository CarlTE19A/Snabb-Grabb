#pragma once

#include "component/Component.h"

namespace physx
{
	class PxShape;
}

class DoorComponent : public Component
{
public:
	DoorComponent();

	virtual ~DoorComponent();

	void OnCollision(physx::PxShape* aShape);
private:

	bool myDoorBreakSound = false;

	// Inherited via Component
	void Update(const float aDeltaTime, Scene& aScene) override;

	void ImguiUpdate() override;

	void Reset() override;

	void DestroyDoor();

};