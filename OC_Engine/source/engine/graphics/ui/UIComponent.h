#pragma once
#include"../../external/imgui/imgui.h"
#include "graphics\sprite\SpriteDrawer.h"
#include "graphics\GraphicsEngine.h"
#include "graphics\sprite\Sprite.h" 
#include "UIElement.h"

enum class eUIComponentType
{
	eSprite,
	eButton,
	eText,
	eSlider,
	eProgressBar,
	eCustom
};

class UIComponent
{
public:
	friend class UIHander;

	UIComponent() {};

	virtual ~UIComponent() 
	{
		myParent = nullptr;
	}
	void SetParent(UIElement* aParent) { myParent = aParent; }

	//virtual bool Init(const std::string aTexturePath) { if (!mySprite.Init(aTexturePath))return false; return true; };
	//virtual bool Init() = 0;
	virtual void Update() = 0;
	virtual void Render() = 0;
	
	virtual void ImguiUpdate() = 0;
	void ImguiNativeUpdate()
	{
		ImGui::Checkbox("Active", &myIsActive);
	}

	Vector2f GetPosition() const { return myPosition; }
	virtual void SetPosition(Vector2f aPosition) = 0;

	float GetRotation() const { return myRotation; }
	virtual void SetRotation(float aRotation) = 0;

	Vector2f GetSize() const { return mySize; }
	virtual void SetSize(Vector2f aScale) = 0;

	void SetActive(bool aVisible) { myIsActive = aVisible; }
	bool& IsActive() { return myIsActive; }

	std::string GetName() const { return myName; }
	void SetName(std::string aName) { myName = aName; }

protected:
	UIElement* myParent = nullptr;
	Vector2f myPosition;
	Vector2f mySize;
	std::string myName;
	float myRotation;
	bool myIsActive = true;

private:
};

