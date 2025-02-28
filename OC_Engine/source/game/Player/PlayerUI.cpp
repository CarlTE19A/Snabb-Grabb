#include "stdafx.h"
#include "PlayerUI.h"
#include "graphics\ui\UIHandler.h"
#include "graphics\ui\UIText.h"
#include "graphics\ui\UISprite.h"
#include "mainSingelton\MainSingelton.h"

#include "scene\SceneManager.h"
#include "scene\Scene.h"

PlayerUI::PlayerUI()
{
	myUsedCardElement = UIElement(true);
	myCopyCards1.push_back(UIElement(true));
	myCopyCards1.push_back(UIElement(true));
	myCopyCards2.push_back(UIElement(true));
	myCopyCards2.push_back(UIElement(true));
	myCopyCards1[0].SetActive(false);
	myCopyCards1[1].SetActive(false);
	myCopyCards2[0].SetActive(false);
	myCopyCards2[1].SetActive(false);

	myUIHandler = new UIHandler();
	myUIHandler->Init("PlayerUI");
	myCardLerpTime = 0.4f;
	myCardLerpTimer = myCardLerpTime;
	myLerpCards.resize(2);
	myLerpCards[0] = 0;
	myLerpCards[1] = 1;
	myLerpSpeed = 65.0f;
	myIsLerping = false;
	//You smart
	myNoiseTexture.Initialize(TextureInitializationType::DDS, STATIC_ASSET_PATH "noise_vfx_c.dds", false);
	myNoiseTexture.Bind(1, GraphicsEngine::GetInstance()->GetContext());

	myObserver = new Observer(Message::Type::CardSpent, std::bind(&PlayerUI::SetCardIsUsed, this, std::placeholders::_1));
	myObserver1 = new Observer(Message::Type::CardAdded, std::bind(&PlayerUI::CardAdded, this, std::placeholders::_1));
	myObserver2 = new Observer(Message::Type::CardSwapped, std::bind(&PlayerUI::CardSwapped, this, std::placeholders::_1));
	myObserver3 = new Observer(Message::Type::NewCardAdded, std::bind(&PlayerUI::NewCardAdded, this, std::placeholders::_1));
	myObserver4 = new Observer(Message::Type::ClearDeck, std::bind(&PlayerUI::ClearCards, this, std::placeholders::_1));
	myObserver5 = new Observer(Message::Type::SameCardSpent, std::bind(&PlayerUI::SameCardSpent, this, std::placeholders::_1));
	myObserver6 = new Observer(Message::Type::CardSwappedToFront, std::bind(&PlayerUI::CardSwappedToFront, this, std::placeholders::_1));
	myObserver7 = new Observer(Message::Type::SetAmmo, std::bind(&PlayerUI::SetAmmo, this, std::placeholders::_1));

	myObserver8 = new Observer(Message::Type::PlayerHit, std::bind(&PlayerUI::TookDamage, this, std::placeholders::_1));


	myCardTextures.resize(8);
	myCardTextures[0] = RELATIVE_ASSET_ASSET_PATH "ui_assets/weapon cards/ui_swordcard.dds";
	myCardTextures[1] = RELATIVE_ASSET_ASSET_PATH "ui_assets/weapon cards/ui_jumpcard.dds";
	myCardTextures[2] = RELATIVE_ASSET_ASSET_PATH "ui_assets/weapon cards/ui_dashcard.dds";
	myCardTextures[3] = RELATIVE_ASSET_ASSET_PATH "ui_assets/weapon cards/ui_stompcard.dds";
	myCardTextures[4] = RELATIVE_ASSET_ASSET_PATH "ui_assets/weapon cards/ui_dirDashcard.dds";
	myCardTextures[5] = RELATIVE_ASSET_ASSET_PATH "ui_assets/weapon cards/ui_grapplecard.dds";
	myCardTextures[6] = RELATIVE_ASSET_ASSET_PATH "ui_assets/weapon cards/ui_teleportcard.dds";
	myCardTextures[7] = RELATIVE_ASSET_ASSET_PATH "ui_assets/weapon cards/ui_bombcard.dds";


	myPlayerCardTextures.resize(3);
	myPlayerCardTextures[0] = RELATIVE_ASSET_ASSET_PATH "UI_Assets/player cards/ui_HPfull.dds";
	myPlayerCardTextures[2] = RELATIVE_ASSET_ASSET_PATH "UI_Assets/player cards/ui_HPmid.dds";
	myPlayerCardTextures[1] = RELATIVE_ASSET_ASSET_PATH "UI_Assets/player cards/ui_HPlow.dds";



	myUIHandler->ReadUILayout();
	myUIHandler->SetVisible(true);
	auto element = myUIHandler->GetElements();
	myCardElements.resize(2);
	for (size_t i = 0; i < element.size(); i++)
	{
		std::string name = element[i]->GetName();
		if (name == "cardSlot0")
		{
			myCardElements[0] = element[i];
			myUsedCardElement = myCardElements[0];
			myStartPos = myCardElements[0]->GetPosition();
			myStartSize = myCardElements[0]->GetSize();
			myStartRot = myCardElements[0]->GetRotation();

		}
		else if (name == "cardSlot1")
		{
			myCardElements[1] = element[i];
			mySecondaryStartPos = myCardElements[1]->GetPosition();
			mySecondaryStartSize = myCardElements[1]->GetSize();
			mySecondaryStartRot = myCardElements[1]->GetRotation();
		}
		else if (name == "playerCard")
		{
			myPlayerCardElement = element[i];
			myPlayerProfileCardShakePos = myPlayerCardElement->GetPosition();
		}
		else if (name == "turrets")
		{
			myTurretElement = element[i];
			myTurretText = myTurretElement->GetComponent<UIText>();

		}
		else if (name == "ammo")
		{
			myAmmoElement = element[i];
			myAmmoText = myAmmoElement->GetComponent<UIText>();
		}
	}
	myCardPositions.push_back(myStartPos);
	myCardPositions.push_back(mySecondaryStartPos);
	myCardRotations.push_back(myStartRot);
	myCardRotations.push_back(mySecondaryStartRot);
	myCardScales.push_back(myStartSize);
	myCardScales.push_back(mySecondaryStartSize);

	myCardElements[1]->SetActive(false);
	myCardElements[0]->SetActive(false);
	//You smart, but not here
	{
		myUsedCardElement.SetPosition(myStartPos);
		myUsedCardElement.SetSize(myCardElements[0]->GetSize());
		myUsedCardElement.SetRotation(myCardElements[0]->GetRotation());

		myCopyCards1[0] = myCardElements[0];
		myCopyCards1[1] = myCardElements[0];
		myCopyCards1[0].SetPosition(myStartPos + Vector2f(25.f, -25.f));
		myCopyCards1[1].SetPosition(myStartPos + Vector2f(50.f, -50.f));
		myCopyCards1[0].SetSize(myCardElements[0]->GetSize());
		myCopyCards1[1].SetSize(myCardElements[0]->GetSize());
		myCopyCards1[0].SetRotation(myCardElements[0]->GetRotation());
		myCopyCards1[1].SetRotation(myCardElements[0]->GetRotation());

		UISprite* sprite = new UISprite(myCardTextures[0], &myUsedCardElement);
		sprite->SetLocalPosition(Vector2f(0, 0));
		sprite->SetLocalRotation(0);
		sprite->SetPosition(myStartPos);
		sprite->SetSize(myCardElements[0]->GetSize());
		sprite->SetRotation(myCardElements[0]->GetRotation());

		UISprite* sprite1 = new UISprite(myCardTextures[0], &myCopyCards1[0]);
		sprite1->SetLocalPosition(Vector2f(0, 0));
		sprite1->SetLocalRotation(0);
		sprite1->SetPosition(myStartPos);
		sprite1->SetSize(myCardElements[0]->GetSize());
		sprite1->SetRotation(myCardElements[0]->GetRotation());

		UISprite* sprite2 = new UISprite(myCardTextures[0], &myCopyCards1[1]);
		sprite2->SetLocalPosition(Vector2f(0, 0));
		sprite2->SetLocalRotation(0);
		sprite2->SetPosition(myStartPos);
		sprite2->SetSize(myCardElements[0]->GetSize());
		sprite2->SetRotation(myCardElements[0]->GetRotation());

		myUsedCardElement.AddComponent(sprite);
		myCopyCards1[0].AddComponent(sprite1);
		myCopyCards1[1].AddComponent(sprite2);
		myCardToFollow1 = myCardElements[0];

		myUsedCardElement.SetActive(false);
	}

	{
		myCopyCards2[0] = myCardElements[1];
		myCopyCards2[1] = myCardElements[1];
		myCopyCards2[0].SetPosition(mySecondaryStartPos + Vector2f(25.f, -25.f));
		myCopyCards2[1].SetPosition(mySecondaryStartPos + Vector2f(50.f, -50.f));
		myCopyCards2[0].SetSize(myCardElements[0]->GetSize());
		myCopyCards2[1].SetSize(myCardElements[0]->GetSize());

		myCopyCards2[0].SetRotation(myCardElements[0]->GetRotation());
		myCopyCards2[1].SetRotation(myCardElements[0]->GetRotation());

		UISprite* sprite1 = new UISprite(myCardTextures[0], &myCopyCards2[0]);
		sprite1->SetLocalPosition(Vector2f(0, 0));
		sprite1->SetLocalRotation(0);
		sprite1->SetPosition(mySecondaryStartPos);
		sprite1->SetSize(myCardElements[0]->GetSize());
		sprite1->SetRotation(myCardElements[0]->GetRotation());

		UISprite* sprite2 = new UISprite(myCardTextures[0], &myCopyCards2[1]);
		sprite2->SetLocalPosition(Vector2f(0, 0));
		sprite2->SetLocalRotation(0);
		sprite2->SetPosition(mySecondaryStartPos);
		sprite2->SetSize(myCardElements[0]->GetSize());
		sprite2->SetRotation(myCardElements[0]->GetRotation());

		myCopyCards2[0].AddComponent(sprite1);
		myCopyCards2[1].AddComponent(sprite2);
		myCardToFollow2 = myCardElements[1];
	}

}

PlayerUI::~PlayerUI()
{
	delete myUIHandler;
	delete myObserver;
	delete myObserver1;
	delete myObserver2;
	delete myObserver3;
	delete myObserver4;
	delete myObserver5;
	delete myObserver6;
	delete myObserver7;
	delete myObserver8;
}

void PlayerUI::Init()
{


	myScene = MainSingleton::GetInstance().GetSceneManager().GetActiveScene();
	int amount = myScene->GetEnemyCount();
	myTurretText->SetText(std::to_string(amount));
}

void PlayerUI::Update(const float aDeltaTime)
{
	myUIHandler->Update(aDeltaTime);
	myUsedCardElement.Update();
	//myScene = MainSingleton::GetInstance().GetSceneManager().GetActiveScene();

	myTurretText->SetText(std::to_string(myScene->GetEnemyCount()));

	if (myWaitLerp && !myIsLerping)
	{
		myIsLerping = true;
		myWaitLerp = false;
		myCardLerpTimer = myCardLerpTime;
	}

	if (myIsLerping)
	{
		if (myCardLerpTimer == myCardLerpTime)
		{

			myUIHandler->SwapElements(myCardElements[myLerpCards[0]]->GetID(), myCardElements[myLerpCards[1]]->GetID());
			std::swap(myCardElements[myLerpCards[0]], myCardElements[myLerpCards[1]]);
		}

		myCardLerpTimer -= aDeltaTime;
		if (LerpPlayerCards(aDeltaTime, myLerpCards[1], myLerpCards[0]))
		{
			myIsLerping = false;
			myCardLerpTimer = myCardLerpTime;
			myUsedCardDesiredPosition = myCardElements[0]->GetPosition() + Vector2f(50.0f, -100.0f);
		}

	}
	for (size_t i = 0; i < 2; i++)
	{
		if (myCopyCards1[i].IsActive())
		{
			myCopyCards1[i].SetPosition(myCardToFollow1->GetPosition() + Vector2f(25.f * (i + 1), -25.f * (i + 1)));
			myCopyCards1[i].SetSize(myCardToFollow1->GetSize());
			myCopyCards1[i].SetRotation(myCardToFollow1->GetRotation());
		}
		if (myCopyCards2[i].IsActive())
		{
			myCopyCards2[i].SetPosition(myCardToFollow2->GetPosition() + Vector2f(25.f * (i + 1), -25.f * (i + 1)));
			myCopyCards2[i].SetSize(myCardToFollow2->GetSize());
			myCopyCards2[i].SetRotation(myCardToFollow2->GetRotation());
		}
	}

	if (myIsCardUsed)
	{
		myUsedCardElement.GetComponent<UISprite>()->GetSprite().DecreaseTimer(aDeltaTime * 1.5f);
		if (UsedCard(aDeltaTime))
		{
			myIsCardUsed = false;
			myUsedCardElement.GetComponent<UISprite>()->SetShaderType(ShaderType::eGlitch);
			myUsedCardElement.SetActive(false);
			myUsedCardElement.SetPosition(myStartPos);
		}
	}

	if (myPlayerProfileCardShake)
	{
		//shake uiElement
		myPlayerProfileCardShakeTimer -= aDeltaTime;

		if (myPlayerProfileCardShakeTimer <= 0)
		{
			myPlayerProfileCardShake = false;
			myPlayerCardElement->SetPosition(myPlayerProfileCardShakePos);
		}
		else
		{
			myPlayerCardElement->SetPosition(myPlayerCardElement->GetPosition() + Vector2f(cos(myPlayerProfileCardShakeTimer * 50) * 2, 0));
		}

	}
}

void PlayerUI::Render()
{
	myNoiseTexture.Bind(1, GraphicsEngine::GetInstance()->GetContext());
	for (int i = 1; i >= 0; i--)
	{
		myCopyCards1[i].Render();
		myCopyCards2[i].Render();
	}
	myUIHandler->Draw();
	myUsedCardElement.Render();
}

bool PlayerUI::UsedCard(const float aDeltaTime)
{
	return LerpPos(aDeltaTime, &myUsedCardElement, myUsedCardDesiredPosition, 11.25f);
}

bool PlayerUI::LerpPlayerCards(const float aDeltaTime, int card1, int card2)
{
	bool lerpDone = LerpPos(aDeltaTime, myCardElements[card1], myCardPositions[1], myLerpSpeed);
	lerpDone &= LerpPos(aDeltaTime, myCardElements[card2], myCardPositions[0], myLerpSpeed);
	lerpDone &= LerpRot(aDeltaTime, myCardElements[card1], myCardRotations[1], myLerpSpeed);
	lerpDone &= LerpRot(aDeltaTime, myCardElements[card2], myCardRotations[0], myLerpSpeed);
	lerpDone &= LerpScale(aDeltaTime, myCardElements[card1], myCardScales[1], myLerpSpeed);
	lerpDone &= LerpScale(aDeltaTime, myCardElements[card2], myCardScales[0], myLerpSpeed);

	return lerpDone;
}

bool PlayerUI::LerpPos(const float aDeltaTime, UIElement* aElement, const Vector2f aTargetPosition, const float aLerpSpeed)
{
	Vector2f tempPos = aElement->GetPosition();
	Vector2f newPos = tempPos + (aTargetPosition - tempPos) * aLerpSpeed * aDeltaTime;
	aElement->SetPosition(newPos);

	return newPos - aTargetPosition <= Vector2f(0.01f, 0.01f);
}

bool PlayerUI::LerpRot(const float aDeltaTime, UIElement* aElement, const float aTargetFloat, const float aLerpSpeed)
{
	float tempFloat = aElement->GetRotation();
	float newFloat = tempFloat + (aTargetFloat - tempFloat) * aLerpSpeed * aDeltaTime;
	aElement->SetRotation(newFloat);

	return newFloat - aTargetFloat <= 0.0001f;
}

bool PlayerUI::LerpScale(const float aDeltaTime, UIElement* aElement, const Vector2f aTargetScale, const float aLerpSpeed)
{
	Vector2f tempScale = aElement->GetSize();
	Vector2f newScale = tempScale + (aTargetScale - tempScale) * aLerpSpeed * aDeltaTime;
	aElement->SetSize(newScale);

	return newScale - aTargetScale <= Vector2f(0.0001f, 0.0001f);
}

void PlayerUI::CardSwapped(Message)
{
	if (myIsLerping)
	{
		if (!myWaitLerp)
		{
			myWaitLerp = true;
			myIsLerping = false;
			myCardElements[0]->SetPosition(myStartPos);
			myCardElements[1]->SetPosition(mySecondaryStartPos);
		}
		else
		{
		}
	}
	else
	{
		myIsLerping = true;
	}
}

void PlayerUI::SetCardIsUsed(Message aMessage)
{
	if (myIsLerping)
	{
		if (!myWaitLerp)
		{
			myWaitLerp = true;
			myIsLerping = false;
			myCardElements[0]->SetPosition(myStartPos);
			myCardElements[1]->SetPosition(mySecondaryStartPos);
		}
		else
		{
		}
	}
	UISprite* sprite = myUsedCardElement.GetComponent<UISprite>();
	myUsedCardElement.SetActive(true);
	sprite->GetSprite().SetTimer(1.f);
	sprite->GetSprite().SetTexture(myCardElements[0]->GetComponent<UISprite>()->GetSprite().GetTexture().GetPath());
	sprite->SetShaderType(ShaderType::eDisintegrate);

	myUsedCardElement.SetPosition(myStartPos);
	myIsCardUsed = true;

	if (!std::any_cast<bool>(aMessage.myData))
	{
		myCardElements[0]->SetActive(false);
		myCardElements[0]->GetComponent<UISprite>()->GetSprite().SetTexture(myCardTextures[0]);
		myCardElements[1]->GetComponent<UISprite>()->GetSprite().SetTexture(myCardTextures[0]);
		myIsLerping = true;
	}
	else
	{
		myCardElements[0]->GetComponent<UISprite>()->GetSprite().SetTexture(myCardTextures[0]);
	}
}

void PlayerUI::SameCardSpent(Message aMessage)
{
	UISprite* sprite = myUsedCardElement.GetComponent<UISprite>();
	myUsedCardElement.SetActive(true);
	sprite->GetSprite().SetTimer(1.f);
	sprite->GetSprite().SetTexture(myCardElements[0]->GetComponent<UISprite>()->GetSprite().GetTexture().GetPath());
	sprite->SetShaderType(ShaderType::eDisintegrate);
	myUsedCardElement.SetPosition(myStartPos);
	myIsCardUsed = true;

	if (std::any_cast<int>(aMessage.myData) == myCardToFollowIndex1)
	{
		if (myCopyCards1[1].IsActive())
		{
			myCopyCards1[1].SetActive(false);
		}
		else
		{
			myCardToFollowIndex1 = -1;
			myCopyCards1[0].SetActive(false);
		}
	}
	else
	{
		if (myCopyCards2[1].IsActive())
		{
			myCopyCards2[1].SetActive(false);
		}
		else
		{
			myCardToFollowIndex2 = -1;
			myCopyCards2[0].SetActive(false);
		}
	}
}

void PlayerUI::CardAdded(Message aMessage)
{
	if (!myCopyCards1[0].IsActive() && myCardTextures[std::any_cast<int>(aMessage.myData)] == myCardToFollow1->GetComponent<UISprite>()->GetSprite().GetTexture().GetPath())
	{
		myCardToFollowIndex1 = std::any_cast<int>(aMessage.myData);
		myCopyCards1[0].SetActive(true);
		myCopyCards1[0].SetPosition(myCardToFollow1->GetPosition() + Vector2f(25.f, -25.f));
		myCopyCards1[0].GetComponent<UISprite>()->GetSprite().SetTexture(myCardTextures[std::any_cast<int>(aMessage.myData)]);
	}
	else if (std::any_cast<int>(aMessage.myData) == myCardToFollowIndex1)
	{
		myCopyCards1[1].SetActive(true);
		myCopyCards1[1].SetPosition(myCardToFollow1->GetPosition() + Vector2f(50.f, -50.f));
		myCopyCards1[1].GetComponent<UISprite>()->GetSprite().SetTexture(myCardTextures[std::any_cast<int>(aMessage.myData)]);
	}
	else if (!myCopyCards2[0].IsActive() && myCardTextures[std::any_cast<int>(aMessage.myData)] == myCardToFollow2->GetComponent<UISprite>()->GetSprite().GetTexture().GetPath())
	{
		myCardToFollowIndex2 = std::any_cast<int>(aMessage.myData);
		myCopyCards2[0].SetActive(true);
		myCopyCards2[0].SetPosition(myCardToFollow2->GetPosition() + Vector2f(25.f, -25.f));
		myCopyCards2[0].GetComponent<UISprite>()->GetSprite().SetTexture(myCardTextures[std::any_cast<int>(aMessage.myData)]);
	}
	else if (std::any_cast<int>(aMessage.myData) == myCardToFollowIndex2)
	{
		myCopyCards2[1].SetActive(true);
		myCopyCards2[1].SetPosition(myCardToFollow2->GetPosition() + Vector2f(50.f, -50.f));
		myCopyCards2[1].GetComponent<UISprite>()->GetSprite().SetTexture(myCardTextures[std::any_cast<int>(aMessage.myData)]);
	}
}

void PlayerUI::NewCardAdded(Message aMessage)
{
  	if (myIsLerping)
	{
		if (!myWaitLerp)
		{
			myWaitLerp = true;
			myIsLerping = false;
			myCardElements[0]->SetPosition(myStartPos);
			myCardElements[1]->SetPosition(mySecondaryStartPos);
		}
		else
		{
		}
	}
	if (!mySawppedToFront)
	{
		myCardElements[1]->SetActive(true);
		myCardElements[1]->GetComponent<UISprite>()->GetSprite().SetTexture(myCardTextures[std::any_cast<int>(aMessage.myData)]);
		if (myCardElements[1] == myCardToFollow1)
		{
			myCardToFollowIndex1 = -1;
			myCopyCards1[0].SetActive(false);
			myCopyCards1[1].SetActive(false);
		}
		else if (myCardElements[1] == myCardToFollow2)
		{
			myCardToFollowIndex2 = -1;
			myCopyCards2[0].SetActive(false);
			myCopyCards2[1].SetActive(false);
		}
	}
	else
	{
		myCardElements[0]->SetActive(true);
		myCardElements[0]->GetComponent<UISprite>()->GetSprite().SetTexture(myCardTextures[std::any_cast<int>(aMessage.myData)]);
		mySawppedToFront = false;
		if (myCardElements[0] == myCardToFollow1)
		{
			myCardToFollowIndex1 = -1;
			myCopyCards1[0].SetActive(false);
			myCopyCards1[1].SetActive(false);
		}
		else if (myCardElements[0] == myCardToFollow2)
		{
			myCardToFollowIndex2 = -1;
			myCopyCards2[0].SetActive(false);
			myCopyCards2[1].SetActive(false);
		}
	}

	if (std::any_cast<int>(aMessage.myData) == 0)
	{
		myIsLerping = true;
	}
	else
	{
		myCardElements[0]->SetActive(true);
	}
}

void PlayerUI::ClearCards(Message)
{
	myCardElements[0]->SetActive(false);
	myCardElements[1]->SetActive(false);
	//reset
	myWaitLerp = false;
	myIsLerping = false;
	myCardElements[0]->SetPosition(myStartPos);
	myCardElements[1]->SetPosition(mySecondaryStartPos);

	myUsedCardElement.SetActive(false);
	myUsedCardElement.SetPosition(myStartPos);

	myCopyCards1[0].SetActive(false);
	myCopyCards1[1].SetActive(false);
	myCopyCards2[0].SetActive(false);
	myCopyCards2[1].SetActive(false);

	myCardToFollow1 = myCardElements[0];
	myCardToFollow2 = myCardElements[1];

	myCardToFollowIndex1 = -1;
	myCardToFollowIndex2 = -1;

	myCopyCards1[0].SetPosition(myStartPos + Vector2f(25.f, -25.f));
	myCopyCards1[1].SetPosition(myStartPos + Vector2f(50.f, -50.f));
	myCopyCards2[0].SetPosition(mySecondaryStartPos + Vector2f(25.f, -25.f));
	myCopyCards2[1].SetPosition(mySecondaryStartPos + Vector2f(50.f, -50.f));
	mySawppedToFront = false;

	myPlayerCardElement->GetComponent<UISprite>()->GetSprite().SetTexture(myPlayerCardTextures[0]);
	myPlayerProfileCardShakeTimer = 0.0f;

}

void PlayerUI::SetAmmo(Message aMessage)
{
	myAmmoText->SetText(std::to_string(std::any_cast<int>(aMessage.myData)));
}

void PlayerUI::TookDamage(Message aMessage)
{
	if (std::any_cast<int>(aMessage.myData) <= 2)
	{
		myPlayerCardElement->GetComponent<UISprite>()->GetSprite().SetTexture(myPlayerCardTextures[std::any_cast<int>(aMessage.myData)]);
		myPlayerProfileCardShake = true;
		myPlayerProfileCardShakeTimer = 0.3f;
	}
}
