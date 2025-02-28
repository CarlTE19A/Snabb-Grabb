#pragma once
#include"../../external/imgui/imgui.h"

class GameObject;
class Scene;

class Component
{
public:
	Component() {};
	//Component(Component* comp) = delete;
	virtual ~Component()
	{
		myParent = nullptr;
	}
	//Component& operator=(const Component&) = delete;
	void SetOwner(GameObject* aParent){ myParent = aParent;}
	GameObject* GetOwner() { return myParent; }
	virtual void SetActive(const bool& aActive) { myActive = aActive; }
	const bool IsActive() const { return myActive; }

	virtual void Update(const float aDeltaTime, Scene& aScene) = 0;
	virtual void ImguiUpdate() = 0;
	void ImguiNativeUpdate()
	{
		ImGui::Checkbox("Active", &myActive);
	}
	virtual void Reset() = 0;
	//virtual Component* Copy() = 0;
	private:
		bool myActive = true;
protected:
		GameObject* myParent = nullptr;
};