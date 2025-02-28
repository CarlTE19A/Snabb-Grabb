#include "CardSystemComponent.h"
#include "gameObject/GameObject.h"
#include "Log.h"

#include "paths.h"
#include <fstream>
#include <nlohmann/json.hpp>

#include "mainSingelton/MainSingelton.h"
#include "events/EventSystem.h"

CardSystemComponent::CardSystemComponent()
{
	//myCardTypes[0] = { 0.8f, 0.5f, 100.f, 1, 1, 1, CardTypeEnum::Sword, 0 };
	//myCardTypes[1] = { 0.8f, 0.5f, 100.f,1, 1, 1, CardTypeEnum::DoubleJump, 0 };
	//myCardTypes[2] = { 0.8f, 0.5f, 100.f,1, 1, 1, CardTypeEnum::Dash, 0 };
	//myCardTypes[3] = { 0.8f, 0.5f, 100.f,1, 1, 1, CardTypeEnum::Stomp, 0 };
	ReadCardsFromJson();
	ClearDeck();
}

CardSystemComponent::~CardSystemComponent() {}


//used for debug
void CardSystemComponent::Update(const float /*aDeltaTime*/, Scene&)
{
}

void CardSystemComponent::ImguiUpdate()
{
	if (ImGui::BeginTabItem("Card System"))
	{
		CardType& card = GetCardType();
		switch (card.myCardType)
		{
			case CardTypeEnum::Sword:
				ImGui::Text("Sword");
				break;
			case CardTypeEnum::DoubleJump:
				ImGui::Text("Double Jump");
				break;
			case CardTypeEnum::Dash:
				ImGui::Text("Dash");
				break;
			case CardTypeEnum::Stomp:
				ImGui::Text("Stomp");
				break;
			case CardTypeEnum::DirectionalDash:
				ImGui::Text("Dir Dash");
				break;
			case CardTypeEnum::Grappling:
				ImGui::Text("Grapple");
				break;
			case CardTypeEnum::Teleporting:
				ImGui::Text("Teleport");
				break;
			case CardTypeEnum::Bomb:
				ImGui::Text("Bomb");
				break;
			case CardTypeEnum::COUNT:
				ImGui::Text("No Card");
				break;
		}
		ImGui::Text("Active Card: %d", card.myCardType);
		ImGui::Text("Accuracy: %f", card.myAccuracy);
		ImGui::Text("Ammo: %d", card.myAmmo);
		ImGui::Text("Bullets per Shot: %d", card.myBulletsPerShot);
		ImGui::Text("Count %d", card.myCount);
		ImGui::Text("Damage: %f", card.myDamage);
		ImGui::Text("Range: %f", card.myRange);
		ImGui::Text("Speed: %f", card.mySpeed);
		ImGui::Text("Time Between Shots: %f", card.myTimeBetweenShots);
		if (ImGui::Button("Reload Cards"))
		{
			ReadCardsFromJson();
			ClearDeck();
		}
		ImGui::EndTabItem();
	}
}

void CardSystemComponent::Reset()
{
	ClearDeck();
}

void CardSystemComponent::UseCard()
{
	if (myCardsInDeck[0].myCardType == CardTypeEnum::Sword) { return; }


	myCardsInDeck[0].myAmmo -= myCardsInDeck[0].myBulletsPerShot;
	MainSingleton::GetInstance().GetEventSystem().SendMsg(Message(Message::Type::SetAmmo, myCardsInDeck[0].myAmmo));
	
	//event update ammo
	if (myCardsInDeck[0].myAmmo <= 0)
	{
		CardSpent();
	}
}

void CardSystemComponent::CardSpent()
{
	if (myCardsInDeck[0].myCardType == CardTypeEnum::Sword) { return; }

	myCardsInDeck[0].myCount--;
	//event card spent
	if (myCardsInDeck[0].myCount <= 0)
	{
		myCardsInDeck[0].myCardType = CardTypeEnum::COUNT;

		int amountOfCards = 0;
		for (size_t i = 0; i < myCardsInDeck.size(); i++)
		{
			if (myCardsInDeck[i].myCardType != CardTypeEnum::COUNT)
			{
				amountOfCards++;
			}
		}
		if (amountOfCards == 1)
		{
			MainSingleton::GetInstance().GetEventSystem().SendMsg(Message(Message::Type::CardSpent, false));
			SwapCardFromBack();
		}
		else if (amountOfCards == 2)
		{
			MainSingleton::GetInstance().GetEventSystem().SendMsg(Message(Message::Type::CardSpent, true));
			SwapCard();
		}
		MainSingleton::GetInstance().GetEventSystem().SendMsg(Message(Message::Type::SetAmmo, myCardsInDeck[0].myAmmo));
	}
	else
	{
		MainSingleton::GetInstance().GetEventSystem().SendMsg(Message(Message::Type::SameCardSpent, (int)myCardsInDeck[0].myCardType));
	}
}

bool CardSystemComponent::AddCard(CardTypeEnum aCardType)
{
	for (size_t i = 0; i < myCardsInDeck.size(); i++)
	{
		if (myCardsInDeck[i].myCardType == aCardType)
		{
			myCardsInDeck[i].myAmmo += myCardTypes[(int)aCardType].myAmmo;

			if (myCardsInDeck[i].myCount < 3)
			{
				myCardsInDeck[i].myCount++;
			}

			if (i == 1)
			{
				SwapCard();
			}
			MainSingleton::GetInstance().GetEventSystem().SendMsg(Message(Message::Type::SetAmmo, myCardsInDeck[0].myAmmo));
			MainSingleton::GetInstance().GetEventSystem().SendMsg(Message(Message::Type::CardAdded, (int)aCardType));
			return true;
		}
	}

	for (size_t i = 0; i < myCardsInDeck.size(); i++)
	{
		if (myCardsInDeck[i].myCardType == CardTypeEnum::COUNT)
		{
			myCardsInDeck[i] = myCardTypes[(int)aCardType];
			if (i == 1)
			{
				SwapCard();
			}
			else if (i == 2)
			{
				SwapCardsToFront();
			}
			MainSingleton::GetInstance().GetEventSystem().SendMsg(Message(Message::Type::SetAmmo, myCardsInDeck[0].myAmmo));
			MainSingleton::GetInstance().GetEventSystem().SendMsg(Message(Message::Type::NewCardAdded, (int)aCardType));
			return true;
		}
	}

	myCardsInDeck[1] = myCardTypes[(int)aCardType];
	SwapCard();
	MainSingleton::GetInstance().GetEventSystem().SendMsg(Message(Message::Type::SetAmmo, myCardsInDeck[0].myAmmo));
	MainSingleton::GetInstance().GetEventSystem().SendMsg(Message(Message::Type::NewCardAdded, (int)aCardType));
	return true;
}

void CardSystemComponent::SwapCard()
{
	if (myCardsInDeck[1].myCardType == CardTypeEnum::COUNT) { return; }

	CardType temp = myCardsInDeck[0];
	if (myCardsInDeck[0].myCardType == CardTypeEnum::COUNT)
	{
		temp = myCardsInDeck[2];
		myCardsInDeck[2].myCardType = CardTypeEnum::COUNT;
	}
	myCardsInDeck[0] = myCardsInDeck[1];
	myCardsInDeck[1] = temp;
	MainSingleton::GetInstance().GetEventSystem().SendMsg(Message(Message::Type::CardSwapped));
	MainSingleton::GetInstance().GetEventSystem().SendMsg(Message(Message::Type::SetAmmo, myCardsInDeck[0].myAmmo));
}

void CardSystemComponent::ClearDeck()
{
	myCardsInDeck[0] = { 0.8f, 0.5f, 0.f, 0.f, 1, 1, 1, CardTypeEnum::COUNT, 0 };
	myCardsInDeck[1] = { 0.8f, 0.5f, 0.f, 0.f, 1, 1, 1, CardTypeEnum::COUNT, 0 };
	myCardsInDeck[2] = { 0.8f, 0.5f, 0.f, 0.f, 1, 1, 1, CardTypeEnum::COUNT, 0 };
	MainSingleton::GetInstance().GetEventSystem().SendMsg(Message(Message::Type::ClearDeck));
	AddCard(CardTypeEnum::Sword);
	//send event clear deck
}

void CardSystemComponent::ReadCardsFromJson()
{
	std::ifstream file;
	file.open(STATIC_ASSET_PATH "json/cardSettings.json");
	if (file.is_open())
	{
		nlohmann::json json;
		file >> json;
		for(int i = 0; i < (int)CardTypeEnum::COUNT; i++)
		{
			myCardTypes[i].myCardType = (CardTypeEnum)i;

			myCardTypes[i].myAccuracy = json["Cards"][i]["Accuracy"];
			myCardTypes[i].myTimeBetweenShots = json["Cards"][i]["TimeBetweenShots"];
			myCardTypes[i].myRange = json["Cards"][i]["Range"];
			myCardTypes[i].mySpeed = json["Cards"][i]["Speed"];
			myCardTypes[i].myBulletsPerShot = json["Cards"][i]["BulletsPerShot"];
			myCardTypes[i].myAmmo = json["Cards"][i]["Ammo"];
			myCardTypes[i].myDamage = json["Cards"][i]["Damage"];
			myCardTypes[i].myCount = 1;
		}
	}
}

void CardSystemComponent::SwapCardsToFront()
{
	if (myCardsInDeck[1].myCardType == CardTypeEnum::COUNT) { return; }

	CardType temp = myCardsInDeck[0];
	CardType temp2 = myCardsInDeck[1];

	if (myCardsInDeck[0].myCardType == CardTypeEnum::Sword)
	{
		myCardsInDeck[1] = temp2;
		myCardsInDeck[0] = myCardsInDeck[2];
		myCardsInDeck[2] = temp;
		MainSingleton::GetInstance().GetEventSystem().SendMsg(Message(Message::Type::CardSwappedToFront));
	}
	else
	{
		myCardsInDeck[1] = temp;
		myCardsInDeck[0] = myCardsInDeck[2];
		myCardsInDeck[2] = temp2;
		MainSingleton::GetInstance().GetEventSystem().SendMsg(Message(Message::Type::CardSwapped));
	}
}

void CardSystemComponent::SwapCardsToBack()
{
	if (myCardsInDeck[1].myCardType == CardTypeEnum::COUNT) { return; }

	CardType temp = myCardsInDeck[0];
	CardType temp2 = myCardsInDeck[1];
	myCardsInDeck[0] = temp2;
	myCardsInDeck[1] = myCardsInDeck[2];
	myCardsInDeck[2] = temp;
}

void CardSystemComponent::SwapCardFromBack()
{
	if (myCardsInDeck[2].myCardType == CardTypeEnum::COUNT) 
	{
		SwapCard();
		return;
	}
	CardType temp = myCardsInDeck[0];
	myCardsInDeck[0] = myCardsInDeck[2];
	myCardsInDeck[2] = temp;
	//send event swap from back
}
