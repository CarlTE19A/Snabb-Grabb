#pragma once
#include "UIComponent.h"
#include "mainSingelton\MainSingelton.h"
#include "events\EventSystem.h"
#include "../Text/Text.h"
#include "UISprite.h"

class UIText : public UIComponent
{
public:
	friend class UIHandler;
	UIText(std::string aText, UIElement* aParent)
	{
		myParent = aParent;
		myName = "Text";
		myText.SetText(aText);
		//mySize = { myText.GetWidth(), myText.GetHeight() };
		mySize.x *= myParent->GetSize().x;
		mySize.y *= myParent->GetSize().y;
		myPosition = { 0, 0 };
		//myFontSize = FontSize_14;
		if (myParent->GetComponent<UISprite>() != nullptr)
		{
			mySpritePtr = myParent->GetComponent<UISprite>();
			myText.SetPosition(mySpritePtr->GetSprite().GetPosition());
		}
		else
		{
			myText.SetPosition(myParent->GetPosition());
		}

		mySize = myParent->GetSize();

		myIsActive = true;
		myRotation = 0;
		myText.Init(FontSize_48);
	}

	~UIText() override {}

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
		ImGui::PushID("Text");
		if (ImGui::CollapsingHeader("TextComponent"))
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

			// change rotation with imgui
			float rotation[1] = { myRotation };
			ImGui::DragFloat("Rotation", rotation, 1.f, 0, 360.f);
			if (rotation[0] != myRotation)
			{
				myRotation = rotation[0];
				SetRotation(myParent->GetRotation());
			}

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

			//change color with imgui
			Vector4f color = myText.GetColor();
			float colorArr[4] = { color.x, color.y, color.z, color.w };
			ImGui::ColorEdit4("Color", colorArr);
			if (colorArr[0] != color.x || colorArr[1] != color.y || colorArr[2] != color.z || colorArr[3] != color.w)
			{
				color = { colorArr[0], colorArr[1], colorArr[2], colorArr[3] };
				SetColor(color);
			}



			//set the text
			char text[128];
			strcpy_s(text, myText.GetText().c_str());
			ImGui::InputText("Text", text, 256);
			myText.SetText(text);
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
		Vector2f size = mySize * myParent->GetSize(); /* + aSize*/;
		myText.SetScale(size);
		//mySprite.SetRect(size);
	}

	//void SetFontSize(FontSize aFontSize) { myFontSize = aFontSize; }

	Text& GetSprite() { return myText; }

	const Vector4f GetColor() { return myText.GetColor(); }
	void SetColor(Vector4f& aColor) { myText.SetColor(aColor); }

	//float GetWidth() { return (float)myText.GetWidth(); }
	//float GetHeight() { return (float)myText.GetHeight(); }

	const float GetAlpha() { return myText.GetColor().w; }
	void SetAlpha(float aAlpha) { Vector4f a = myText.GetColor(); a.w = aAlpha;  myText.SetColor(a); }

	void SetText(std::string aText) { myText.SetText(aText); }
	std::string GetText() { return myText.GetText(); }

private:
	Text myText;
	//FontSize myFontSize;
	UISprite* mySpritePtr;
};
