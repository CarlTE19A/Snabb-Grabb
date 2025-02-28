#include "stdafx.h"
#include "EventSystem.h"
#include <iostream>
#include "mainSingelton\MainSingelton.h"

Observer::Observer(Message::Type aType, std::function<void(Message)> aFunction) : myEventSystem(MainSingleton::GetInstance().GetEventSystem())
{
	myFunction = aFunction;
	myType = aType;
	myEventSystem.Subscribe(this, aType);
	//LogInfo("Observer: Observer Created");
}
Observer::~Observer()
{
	myEventSystem.UnSubscribe(this);
	//LogInfo("Observer: Observer Deleted");
}
void Observer::Receive(const Message& aMessage)
{
	myFunction(aMessage);
}

EventSystem::EventSystem()
{
	for (size_t i = 0; i < (int)Message::Type::Count; i++)
	{
		myObservers.push_back(std::vector<Observer*>());
	}
	//LogInfo("EventSystem: EventSystem Created");

}
EventSystem::~EventSystem()
{
	myObservers.clear();
	//LogInfo("EventSystem: EventSystem Deleted");
}

void EventSystem::SendMsg(const Message& aMessage)
{
	if (myObservers.size() > 0)
	{
		if (myObservers[(int)aMessage.myType].size() > 0)
		{
			for (int i = 0; i < myObservers[(int)aMessage.myType].size(); i++)
			{
				myObservers[(int)aMessage.myType][i]->Receive(aMessage);
			}
		}
	}
}

void EventSystem::Subscribe(Observer* anObserver, const Message::Type aMessageType)
{
	myObservers[(int)aMessageType].push_back(anObserver);
}

void EventSystem::UnSubscribe(Observer* anObserver)
{
	for (int i = 0; i < myObservers.size(); i++)
	{
		for (int j = 0; j < myObservers[i].size(); j++)
		{
			if (myObservers[i][j] == anObserver)
			{
				myObservers[i].erase(myObservers[i].begin() + j);
			}
		}
	}
}