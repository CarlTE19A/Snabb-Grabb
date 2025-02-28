#include "stdafx.h"
#include "UIElement.h"
#include "UIComponent.h"

static int autoIncrementedUIID = 1;
static std::vector<int> allUIIDsArray;


UIElement::UIElement()
{
	myIsActive = true;

	if (allUIIDsArray.size() == 0)
	{

	}
	else
	{
		while (std::find(allUIIDsArray.begin(), allUIIDsArray.end(), autoIncrementedUIID) != std::end(allUIIDsArray))
		{
			autoIncrementedUIID++;
		}
	}
	myID = autoIncrementedUIID;
	allUIIDsArray.push_back(myID);
	myPosition = Vector2f(0, 0);
	mySize = Vector2f(1, 1);
	myRotation = 0;
	myName = "UIElement";
}

UIElement::UIElement(int aID, const bool aIsActive)
{
	allUIIDsArray.push_back(aID);
	myID = aID;
	myIsActive = aIsActive;
	myPosition = Vector2f(0, 0);
	mySize = Vector2f(1, 1);
	myRotation = 0;
	myName = "UIElement";
};

UIElement::UIElement(const bool aIsActive)
{
	myIsActive = aIsActive;

	if (allUIIDsArray.size() == 0)
	{

	}
	else
	{
		while (std::find(allUIIDsArray.begin(), allUIIDsArray.end(), autoIncrementedUIID) != std::end(allUIIDsArray))
		{
			autoIncrementedUIID++;
		}
	}
	myID = autoIncrementedUIID;
	allUIIDsArray.push_back(myID);
	myPosition = Vector2f(0, 0);
	mySize = Vector2f(1, 1);
	myRotation = 0;
	myName = "UIElement";
}

UIElement::~UIElement()
{
	for (auto& c : myComponents)
		if (c != nullptr)
			delete c;
		else
			LogError("Component was nullptr in " + std::to_string(myID));
	myComponents.clear();
};

void UIElement::Update()
{
	if (!myIsActive) { return; }
	for (auto& c : myComponents)
	{
		if (!c->IsActive()) continue;
		c->Update();
	}
}

void UIElement::Render()
{
	if (!myIsActive || !myRender) { return; }
	for (auto& c : myComponents)
	{
		if (!c->IsActive()) continue;
		c->Render();
	}
}

void UIElement::SetComponetsActive(const bool& aActive)
{
	for (auto& c : myComponents)
		c->SetActive(aActive);
}

void UIElement::SetPosition(Vector2f aPosition) 
{
	myPosition = aPosition;
	for (auto& c : myComponents)
	{
		c->SetPosition(aPosition);
	}
}

void UIElement::SetRotation(float aRotation) 
{
	myRotation = aRotation;
	for (auto& c : myComponents)
	{
		c->SetRotation(aRotation);
	}
}

void UIElement::SetSize(Vector2f aScale)
{
	mySize = aScale;
	for (auto& c : myComponents)
	{
		c->SetSize(aScale);
	}
}


