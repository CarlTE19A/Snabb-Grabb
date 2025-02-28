#pragma once
#include "graphics\sprite\SpriteDrawer.h"
#include "graphics\GraphicsEngine.h"
#include "graphics\sprite\Sprite.h" 
#include "Log.h"

class UIComponent;


class UIElement
{
public:
	UIElement();

	UIElement(int aID, const bool aIsActive);

	UIElement(const bool aIsActive);

	~UIElement();

	void Update();

	void Render();
	
	void SetComponetsActive(const bool& aActive);

	Vector2f GetPosition() const { return myPosition; }
	void SetPosition(Vector2f aPosition);

	float GetRotation() const { return myRotation; }
	void SetRotation(float aRotation);

	Vector2f GetSize() const { return mySize; }
	void SetSize(Vector2f aScale);

	std::string GetName() const { return myName; }
	void SetName(std::string aName) { myName = aName; }

	void SetActive(bool aVisible) { myIsActive = aVisible; }
	bool& IsActive() { return myIsActive; }

	template<typename T = UIComponent>
	void AddComponent(T* aComponent) 
	{ 
		myComponents.push_back(aComponent); 
		aComponent->SetParent(this);
	}

	template<typename T = UIComponent>
	T* GetComponent()
	{
		for (auto& c : myComponents)
		{
			T* val = dynamic_cast<T*>(c);
			if (val == nullptr) continue;
			return val;
		}

		return nullptr;
	}

	template<typename T = UIComponent>
	std::vector<T*> GetComponents()
	{
		std::vector<T*> comps;
		for (auto& c : myComponents)
		{
			T* val = dynamic_cast<T*>(c);
			if (val == nullptr) continue;
			comps.push_back(val);
		}

		return comps;
	}

	template<typename T = UIComponent>
	void RemoveComponents()
	{
		for (auto& c : myComponents)
		{
			T* val = dynamic_cast<T*>(c);
			if (val == nullptr) continue;
			myComponents.erase(std::remove(myComponents.begin(), myComponents.end(), val), myComponents.end());
			delete val;
			return;
		}
	}

	template<typename T = UIComponent>
	void RemoveComponent(T* aComponent)
	{
		myComponents.erase(std::remove(myComponents.begin(), myComponents.end(), aComponent), myComponents.end());
		delete aComponent;
	}

	std::vector<UIComponent*>& GetComponents() { return myComponents; }

	int GetID() const { return myID; }
	void SetID(int aID) { myID = aID; }

	void SetShouldRender(bool aRender) { myRender = aRender; }
	bool GetShouldRender() { return myRender; }

private:
	std::vector<UIComponent*> myComponents;
	Vector2f myPosition;
	Vector2f mySize;
	std::string myName;
	float myRotation;

	int myID;
	bool myIsActive = true;

	bool myRender = true;
};

