#include "stdafx.h"

#include "GameObject.h"
#include "../component/Component.h"

#include <iostream>

static int autoIncrementedID = 0;
//static std::vector<int> allIDsArray;

GameObject::GameObject(int aID, const bool aIsActive)
{
	myID = aID;
	myActive = aIsActive;
	myTransform = nullptr;
	parentID = 0;
	myParent = nullptr;
}

GameObject::GameObject(const bool aIsActive)
{
	myActive = aIsActive;

	//TODO : Make sure that the ID is unique and not to slow
	//while (std::count(allIDsArray.begin(), allIDsArray.end(), autoIncrementedID++)) 
	//{}	//Do nothing, just increment the ID until it is unique
	autoIncrementedID++;
	myID = autoIncrementedID;
	myTransform = new Transform();
	parentID = 0;
	myParent = nullptr;
}

GameObject::~GameObject()
{
	for (auto& c : myComponents)
		if (c != nullptr)
		{
			delete c;
			c = nullptr;
		}
		else
		{
			LogError("Component was nullptr in " + std::to_string(myID));
		}
	myComponents.clear();
	delete myTransform;
	myTransform = nullptr;
	delete myResetTransform;
	myResetTransform = nullptr;
}

void GameObject::Update(const float aDeltaTime, Scene& aScene)
{
	if (!myActive) return;

	/*for (auto& c : myComponents)*/
	for (int i = 0; i < (int)myComponents.size(); i++)
	{
		auto& c = myComponents[i];
		if (!c->IsActive()) continue;
		c->Update(aDeltaTime, aScene);
	}
}
void GameObject::SetComponentsActive(const bool& aActive)
{
	for (auto& c : myComponents)
		c->SetActive(aActive);
}

GameObject* GameObject::GetParent() const
{
	return myParent;
}

void GameObject::SetParent(GameObject* aParent)
{
	if (myParent != nullptr)
	{
		myParent->myChildren.erase(std::remove(myParent->myChildren.begin(), myParent->myChildren.end(), this), myParent->myChildren.end());
	}

	if (aParent == nullptr)
	{
		myParent = nullptr;
		parentID = 0;
		return;
	}
	myParent = aParent;
	parentID = aParent->GetID();
	myParent->myChildren.push_back(this);
}

bool GameObject::IsInParents(GameObject* aGameObject) const
{
	if (myParent == nullptr)
	{
		return false;
	}
	if (myParent == aGameObject)
	{
		return true;
	}
	return myParent->IsInParents(aGameObject);
}

void GameObject::InitReset()
{
	myResetTransform = new Transform();
	memcpy(myResetTransform, myTransform, sizeof(Transform));
}

void GameObject::Reset()
{
	if (myResetTransform == nullptr)
	{
		return;
	}
	memcpy(myTransform, myResetTransform, sizeof(Transform));

	for (int i = 0; i < myComponents.size(); i++)
	{
		if (myComponents[i] != nullptr)
		{
			myComponents[i]->Reset();
		}
	}
}

Transform GameObject::MultiplyWithParent()
{
	Transform transformInParentSpace = *GetTransform();
	if (useWorldSpace)
	{
		return transformInParentSpace;
	}
	if (myParent == nullptr)
	{
		return transformInParentSpace;
	}
	transformInParentSpace.SetMatrix(transformInParentSpace.GetMatrix() * myParent->MultiplyWithParent().GetMatrix());
	return transformInParentSpace;
}
