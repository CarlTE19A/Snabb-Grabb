#include "stdafx.h"

#include "DeathState.h"
#include "State.h"
#include "StateStack.h"
#include "events/inputMapper/InputMapper.h"
#include "events/EventSystem.h"
#include "../mainSingelton/MainSingelton.h"
#include "imgui/imgui.h"
#include "graphics\ui\UIHandler.h"

DeathState::DeathState(StateStack& aStateStack)
{
	myStateStack = &aStateStack;
	myUIHandler = new UIHandler();
	myUIHandler->Init("failScreen");
	myUIHandler->ReadUILayout();
	myUIHandler->SetVisible(true);
}

DeathState::~DeathState() {}


void DeathState::Init()
{
	InputWrapper& inputWrapper = MainSingleton::GetInstance().GetInputMapper().GetInputWrapper();
	inputWrapper.SetFocus(true, true);

	if(!myIsActive)
	{
		InputMapper& inputmapper = MainSingleton::GetInstance().GetInputMapper();
		inputmapper.AddObserver(eInputEvent::eKey_F_Pressed, this);
	}
	
	myIsActive = true;
}

void DeathState::UnInit()
{
	MainSingleton::GetInstance().GetInputMapper().RemoveObserver(eInputEvent::eKey_F_Pressed, this);
	myIsActive = false;
}

bool DeathState::Update(float aDeltaTime)
{
	aDeltaTime;
#ifdef _DEBUG
	ImGuiUpdate();
#endif
	return myIsActive;
}

void DeathState::Render()
{
	myUIHandler->Draw();

	if (myStateStack->GetCurrentState() != this)
	{
		myIsActive = false;
	}
	if (myIsActive)
	{

	}
}

void DeathState::RecieveEvent(const eInputEvent aEvent, std::any aValue)
{
	if (!myIsActive) { return; }
	aValue;
	switch (aEvent)
	{
	case eInputEvent::eKey_F_Pressed:
		myStateStack->PopState();
		break;
	default:
		break;
	}
}



void DeathState::ImGuiUpdate()
{
	if (ImGui::Begin("Death Screen"))
	{
		ImGui::SetWindowPos(ImVec2(0, 0));
		ImGui::SetWindowSize(ImVec2(200, 100));
		ImGui::Text("You dead");
		ImGui::Text("Press F to restart");
	}
	ImGui::End();
}