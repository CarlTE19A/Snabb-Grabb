#pragma once
//#include "../CardSystem/CardComponent.h"
//#include <vector>
#include "events\EventSystem.h"
#include "graphics\ui\UIElement.h"


class UIHandler;
class UISprite;
class UIElement;
class UIText;
class Observer;
class Scene;


class PlayerUI
{
public:
	PlayerUI();
	~PlayerUI();

	void Init();

	void Update(const float aDeltaTime);
	void Render();

private:
	bool UsedCard(const float aDeltaTime);
	bool LerpPlayerCards(const float aDeltaTime, int card1, int card2);
	bool LerpPos(const float aDeltaTime, UIElement* aElement, const Vector2f aTargetPosition, const float aLerpSpeed);
	bool LerpRot(const float aDeltaTime, UIElement* aElement, const float aTargetFloat, const float aLerpSpeed);
	bool LerpScale(const float aDeltaTime, UIElement* aElement, const Vector2f aTargetScale, const float aLerpSpeed);

	void SetIsLerping(bool aIsLerping) { myIsLerping = aIsLerping; }

	void SetCardIsUsed(Message);
	void SameCardSpent(Message);

	void CardAdded(Message);
	void NewCardAdded(Message);
	void CardSwapped(Message);
	void CardSwappedToFront(Message)
	{
		mySawppedToFront = true;
	}
	void ClearCards(Message);

	void SetAmmo(Message aMessage);

	void TookDamage(Message aMessage);

	UIHandler* myUIHandler;
	std::vector<UIElement*> myCardElements;
	UIElement myUsedCardElement;
	UIElement* myPlayerCardElement;
	UIElement* myTurretElement;
	UIElement* myAmmoElement;
	UIText* myTurretText;
	UIText* myAmmoText;

	std::vector<int> myLerpCards;

	std::vector<Vector2f> myCardPositions;
	Vector2f myUsedCardDesiredPosition;
	std::vector<Vector2f> myCardScales;
	std::vector<float> myCardRotations;

	float myCardLerpTimer;
	float myCardLerpTime;
	bool myIsLerping;
	bool myWaitLerp;
	float myLerpSpeed;

	bool myIsCardUsed;
	bool mySawppedToFront;


	std::vector<std::string> myCardTextures;
	std::vector<std::string> myAmmoTextures;
	std::vector<std::string> myPlayerCardTextures;

	bool myShowSecondCard;

	Vector2f myStartPos;
	Vector2f mySecondaryStartPos;
	Vector2f myStartSize;
	Vector2f mySecondaryStartSize;
	float myStartRot;
	float mySecondaryStartRot;

	//You stupid
	//Please name observers better	
	Observer* myObserver;
	Observer* myObserver1;
	Observer* myObserver2;
	Observer* myObserver3;
	Observer* myObserver4;
	Observer* myObserver5;
	Observer* myObserver6;
	Observer* myObserver7;
	Observer* myObserver8;

	Texture myNoiseTexture;

	std::vector<UIElement> myCopyCards1;
	std::vector<UIElement> myCopyCards2;

	UIElement* myCardToFollow1;
	UIElement* myCardToFollow2;

	int myCardToFollowIndex1;
	int myCardToFollowIndex2;

	Scene* myScene;

	bool myPlayerProfileCardShake;
	float myPlayerProfileCardShakeTimer;
	Vector2f myPlayerProfileCardShakePos;
};