#pragma once
#include "component/Component.h"
#include "CardSystemComponent.h"
#include "math/Vector.h"
#include <array>
#include "PhysX/PxShape.h"

class PhysicsComponent;

class CardComponent : public Component
{
public:
	CardComponent(CardTypeEnum aCardType);
	~CardComponent() override {};
	void Update(const float /*aDeltaTime*/, Scene& /*aScene*/) override;
	void ImguiUpdate() override;
	void Reset() override;

	void OnCollision(physx::PxShape* aShape);

	inline CardTypeEnum& GetCardType() { return myCardType; }
	inline void SetCardType(CardTypeEnum aCardType) { myCardType = aCardType; }

	void SetNewCardInTheScene();
	inline bool IAmNewCard() const { return myIAmFromEnemy; }
private:
	CardTypeEnum myCardType;
	bool myIAmFromEnemy = false;
	bool isColliderActive = true;
	PhysicsComponent* physicsComponent;
};