#include "stdafx.h"

#include "HowToPlayState.h"
#include "StateStack.h"

#include "graphics/GraphicsEngine.h"
#include "graphics\sprite\SpriteDrawer.h"

#include "gameObject/GameObject.h"
#include "component/CameraComponent.h"
#include "graphics\ui\UIHandler.h"
#include "graphics\ui\UIElement.h"
#include "graphics\ui\UIButton.h"
#include "../mainSingelton/MainSingelton.h"
#include "events/inputMapper/InputMapper.h"
#include "events/EventSystem.h"

#include <iostream>

HowToPlayState::HowToPlayState(StateStack& aStateStack)
{
	myStateStack = &aStateStack;
	myIsActive = false;

	myUIHandler= new UIHandler();
	myUIHandler->Init("howtoplay");
	myUIHandler->ReadUILayout();
	myUIHandler->SetVisible(true);

	std::vector<UIElement*> elements = myUIHandler->GetElements();
	for (UIElement* element : elements)
	{
		std::string name = element->GetName();
		if (name == "back")
		{
			myButton = element;
		}
	}

}

HowToPlayState::~HowToPlayState()
{
}

void HowToPlayState::Init()
{
	MainSingleton::GetInstance().GetInputMapper().GetInputWrapper().SetFocus(true, false);

	if (!myIsActive)
	{
		auto& inputmapper = MainSingleton::GetInstance().GetInputMapper();

		inputmapper.AddObserver(eInputEvent::eLeftMouseClick, this);
	}
	myIsActive = true;
}

void HowToPlayState::UnInit()
{
	MainSingleton::GetInstance().GetInputMapper().RemoveObserver(eInputEvent::eLeftMouseClick, this);

	myIsActive = false;
}

bool HowToPlayState::Update(float aDeltaTime)
{
	myUIHandler->Update(aDeltaTime);
	return myIsActive;
}

void HowToPlayState::Render()
{
	if (myIsActive)
	{
		myUIHandler->Draw();

	}
}


void HowToPlayState::ImGuiUpdate()
{

}

void HowToPlayState::RecieveEvent(const eInputEvent aEvent, std::any aValue)
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
