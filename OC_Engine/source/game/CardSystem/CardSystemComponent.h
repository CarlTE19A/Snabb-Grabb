#pragma once

#include "component/Component.h"
#include "math/Vector.h"
#include <array>

enum class CardTypeEnum 
{
	Sword = 0,
	DoubleJump = 1,
	Dash = 2,
	Stomp = 3,
	DirectionalDash = 4,
	Grappling = 5,
	Teleporting = 6,
	Bomb = 7,
	COUNT = 8
};

struct CardType
{
	float myAccuracy;
	float myTimeBetweenShots;
	float myRange;
	float mySpeed;
	int myBulletsPerShot;
	int myAmmo;
	int myDamage;
	CardTypeEnum myCardType;
	int myCount;
};



class CardSystemComponent : public Component
{
public:
	CardSystemComponent();
	~CardSystemComponent() override;
	void Update(const float aDeltaTime, Scene& /*aScene*/) override;
	void ImguiUpdate() override;
	void Reset() override;
	
	inline CardType& GetCardType() { return myCardsInDeck[0]; }
	
	void UseCard();
	
	void CardSpent();

	bool AddCard(CardTypeEnum aCardType);

	void SwapCard();

	void ClearDeck();

	void ReadCardsFromJson();

private:
	std::array<CardType, (int)CardTypeEnum::COUNT> myCardTypes;
	std::array<CardType, 3> myCardsInDeck;

	void SwapCardsToFront();
	void SwapCardsToBack();
	void SwapCardFromBack();

	
};