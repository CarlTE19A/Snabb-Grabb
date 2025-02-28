#pragma once
#include <vector>
#include "../math/Transform.h"
#include <string>

class Scene;
class Component;


class GameObject
{
public:
	//For debugging purposes only
	std::string myName = "";
	bool imGuiOpen = false;
private:
	int myID;
	bool myActive;
	friend class JsonSceneImporter; 
	friend class JsonSceneExporter;
	friend class EditorState;
	friend class EnemyPool;
	friend class AmmoPool;
	friend class ExplosionPool;
	friend class Door;
	Transform* myResetTransform;
	bool useWorldSpace = true;
protected:
	GameObject(int aID, const bool aIsActive);
	std::vector<Component*> myComponents;
	Transform* myTransform;
	std::string myTag;		//These should be enums
	std::string myLayer;	//These should be enums
	int parentID;
	GameObject* myParent;
	std::vector<GameObject*> myChildren;

public:
	//Standard
	GameObject(const bool aIsActive = true);
	GameObject(const GameObject&) = delete;
	~GameObject();
	virtual void Update(const float aDeltaTime, Scene& aScene);
	//Component Interface
	template<typename T = Component>
	void AddComponent(T* aComponent)
	{
		//T* component = new T();
		myComponents.push_back(aComponent);
		aComponent->SetOwner(this);
		//return component;
	}

	//Gets the first component of type T
	template<typename T = Component>
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

	//Gets all components of type T
	template<typename T = Component>
	std::vector<T*> GetComponents()
	{
		std::vector<T*> components;

		for (auto& c : myComponents)
		{
			T* val = dynamic_cast<T*>(c);
			if (val == nullptr) continue;
			components.push_back(val);
		}

		return components;
	}

	//Gets all components
	std::vector<Component*> GetComponents()
	{
		return myComponents;
	}

	//void InsertComponent(Component* aComponent);

	//Activation Interface
	const bool& IsActive() const { return myActive; }
	virtual void SetActive(const bool& aActive){ myActive = aActive;}
	virtual void SetComponentsActive(const bool& aActive);

	//Transform Interface
	Vector3f GetPosition() const { return myTransform->GetPosition(); }
	void SetPosition(const Vector3f& aPosition) { myTransform->SetPosition(aPosition); }
	inline void SetUseWorldSpace(const bool aUseWorldSpace) { useWorldSpace = aUseWorldSpace; }

	Vector3f GetRotation() const { return myTransform->GetRotation(); }
	//void SetRotation(const Vector3f& aRotation) { myTransform->SetRotationFromUnity(aRotation); }

	Vector3f GetScale() const { return myTransform->GetScale(); }
	void SetScale(const Vector3f& aScale) { myTransform->SetScale(aScale); }

	//If this crashed after active camera was reset it's because the camera was deleted and the pointer is invalid
	Transform* GetTransform() const { return myTransform; }
	void SetTransform(const Transform* aTransform) { *myTransform = *aTransform; }
	void SetTransform(const Transform aTransform) { *myTransform = aTransform; }
	//void SetTransformAdress(Transform* aTransform) { myTransform = aTransform; }

	GameObject* GetParent() const;
	void SetParent(GameObject* aParent);
	
	std::vector<GameObject*> GetChildren() const { return myChildren; }

	bool IsInParents(GameObject* aGameObject) const;

	const int& GetID() const { return myID; }

	inline std::string GetTag() const { return myTag; }
	void InitReset();
	void Reset();

	Transform MultiplyWithParent();
};