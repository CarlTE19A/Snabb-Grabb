#pragma once
#include <vector>
#include <functional>
#include <any>

class EventSystem;

struct Message
{
	enum class Type
	{
		WonGame,
		LostGame,
		KeyAdded,
		FileDropped,
		CardSpent,
		SameCardSpent,
		NewCardAdded,
		CardAdded,
		CardSwapped,
		CardSwappedToFront,
		ClearDeck,
		SetAmmo,
		InputKeyPressed,
		PlayerHit,
		//InputKeyReleased,
		Collision,
		LostFocus,
		Count
	};
	
	std::any myData;
	Type myType;
	Message(Type aType, std::any aData = nullptr) : myType(aType), myData(aData) {};
};

class Observer
{
public:

	Observer(Message::Type aType, std::function<void(Message)>);
	~Observer();
	void Receive(const Message& aMessage);
	Message::Type myType;
private:
	std::function<void (Message)> myFunction;
	EventSystem& myEventSystem;
};

class EventSystem
{
public:
	EventSystem();
	~EventSystem();
	void SendMsg(const Message& aMessage);
	void Subscribe(Observer* anObserver, const Message::Type aMessageType);
	void UnSubscribe(Observer* anObserver);
	EventSystem(EventSystem const&) = delete;
	void operator=(EventSystem const&) = delete;
	//Not safe, use only for debug purposes (used by the editor)
	std::any aTempData;
private:
	std::vector<std::vector<Observer*>> myObservers;
};