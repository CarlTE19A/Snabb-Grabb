#include "stdafx.h"

#include "CreditsState.h"
#include "State.h"
#include "StateStack.h"
#include "events/inputMapper/InputMapper.h"
#include "events/EventSystem.h"
#include "../mainSingelton/MainSingelton.h"
#include "../Sound/SoundManager.h"
#include <iostream>
#include "graphics\sprite\SpriteDrawer.h"

#include "gameObject/GameObject.h"

#include "graphics\ui\UIHandler.h"
#include "graphics\ui\UIElement.h"
#include "graphics\ui\UIButton.h"


CreditsState::CreditsState(StateStack& aStateStack)
{
	myStateStack = &aStateStack;

	myIsActive = false;


	myUIHandler = new UIHandler();
	myUIHandler->Init("credits");
	myUIHandler->ReadUILayout();
	myUIHandler->SetVisible(true);

	std::vector<UIElement*> elements = myUIHandler->GetElements();
	for (UIElement* element : elements)
	{
		std::string name = element->GetName();
		if (name == "back")
		{
			myButton = element;
			myButton->SetShouldRender(true);
		}
	}

}

CreditsState::~CreditsState()
{
}

void CreditsState::Init()
{
	MainSingleton::GetInstance().GetInputMapper().GetInputWrapper().SetFocus(true, false);

	if (!myIsActive)
	{
		auto& inputmapper = MainSingleton::GetInstance().GetInputMapper();

		inputmapper.AddObserver(eInputEvent::eLeftMouseClick, this);
	}
	myIsActive = true;

}

void CreditsState::UnInit()
{
	MainSingleton::GetInstance().GetInputMapper().RemoveObserver(eInputEvent::eLeftMouseClick, this);

	myIsActive = false;
}

bool CreditsState::Update(float aDeltaTime)
{
	aDeltaTime;
	myUIHandler->Update(aDeltaTime);

	return myIsActive;
}

void CreditsState::Render()
{

	if (myStateStack->GetCurrentState() != this)
	{
		myIsActive = false;
	}
	if (myIsActive)
	{
	myUIHandler->Draw();

	}

}

void CreditsState::RecieveEvent(const eInputEvent aEvent, std::any aValue)
{
	if (!myIsActive)
	{
		return;
	}
	aValue;
	if (aEvent == eInputEvent::eLeftMouseClick /*|| aEvent == eInputEvent::eBack*/)
	{
		if (myButton->GetComponent<UIButton>()->GetIsHighlighted())
		{
			myStateStack->PopState(); // pop credits
			myIsActive = false;
		}
	}
}

void CreditsState::ImGuiUpdate()
{

}
