#pragma once
#include "UIComponent.h"
#include "mainSingelton\MainSingelton.h"
#include "events/inputMapper/InputObserver.h"
#include "events\EventSystem.h"
#include "../Text/Text.h"
#include "UISprite.h"

class UITextField : public UIComponent
{
public:
	friend class UIHandler;
	UITextField(std::string aText, UIElement* aParent)
	{
		myParent = aParent;
		myName = "TextField";
		myText.SetText(aText);
		//mySize = { myText.GetWidth(), myText.GetHeight() };
		mySize.x *= myParent->GetSize().x;
		mySize.y *= myParent->GetSize().y;
		myPosition = { 0, 0 };

		if (myParent->GetComponent<UISprite>() != nullptr)
		{
			mySpritePtr = myParent->GetComponent<UISprite>();
			Vector2f pos = mySpritePtr->GetSprite().GetPosition();
			pos.x -= mySpritePtr->GetSprite().GetTexture().GetWidth() * mySpritePtr->GetSprite().GetSize().x / 2;
			myText.SetPosition(pos);
		}
		else
		{
			myText.SetPosition(myParent->GetPosition());
		}
		myText.Init(FontSize_48);

		myRotation = 0;
		myTextObserver = new Observer(Message::Type::InputKeyPressed, std::bind(&UITextField::Input, this, std::placeholders::_1));
		myScaleSize = Vector2f(1920, 1080);
		myIsActive = false;
	}

	~UITextField() override {}

	void Update() override;

	void Render() override
	{
		if (myIsActive)
		{
			myText.Draw();
		}
	}

	void ImguiUpdate() override
	{
		ImGui::PushID("TextField");
		//namme of the component
		if (ImGui::CollapsingHeader("TextFieldComponent"))
		{
			float position[2] = {
					myPosition.x,
					myPosition.y
			};

			ImGui::DragFloat2("Position", position, 1.0f);
			if (position[0] != myPosition.x || position[1] != myPosition.y)
			{
				myPosition = { position[0], position[1] };
				SetPosition(myParent->GetPosition());
			}

			// change size with imgui
			//Vector2f sizeVec = mySize;
			float size[2] = {
					mySize.x,
					mySize.y
			};
			ImGui::DragFloat2("Size", size, 0.01f);
			if (size[0] != mySize.x || size[0] != mySize.y)
			{
				mySize = { size[0], size[1] };
				SetSize(myParent->GetSize());
			}

			std::string text = "Text:";
			text += myText.GetText();
			ImGui::Text(text.c_str());
		}
		ImGui::PopID();
	}
	void SetPosition(Vector2f /*aPosition*/) override
	{
		Vector2f pos = myPosition + myParent->GetPosition();
		myText.SetPosition(pos);
	}
	void SetRotation(float /*aRotation*/) override
	{
		float rot = myRotation + myParent->GetRotation();
		myText.SetRotation(rot);
	}
	void SetSize(Vector2f /*aSize*/) override
	{
		//Vector2f size = mySize * myParent->GetSize(); /* + aSize*/;
		//myText.SetSize(size);
		//mySprite.SetRect(size);
	}

	//Text& GetSprite() { return myText; }

	const Vector4f GetColor() { return myText.GetColor(); }
	void SetColor(Vector4f& aColor) { myText.SetColor(aColor); }

	//float GetWidth() { return (float)myText.GetWidth(); }
	//float GetHeight() { return (float)myText.GetHeight(); }

	const float GetAlpha() { return myText.GetColor().w; }
	void SetAlpha(float aAlpha) { Vector4f a = myText.GetColor(); a.w = aAlpha;  myText.SetColor(a); }

	void SetText(std::string aText) { myText.SetText(aText); }
	std::string GetText() { return myText.GetText(); }

	void Input(Message aMessage);

	void SetIsCenterOfAttention(bool aIsCenterOfAttention) { myIsCenterOfAttention = aIsCenterOfAttention; }

	bool IsSelected();
private:
	Text myText;
	UISprite* mySpritePtr;
	bool myIsCenterOfAttention;
	Observer* myTextObserver;
	Vector2f myScaleSize;

};
