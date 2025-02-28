#pragma once
#include "UIComponent.h"
#include "mainSingelton\MainSingelton.h"
#include "events\EventSystem.h"

class UISprite : public UIComponent
{
public:
	friend class UIHandler;
	UISprite(std::string aTexturepath, UIElement* aParent)
	{
		myParent = aParent;
		mySprite.Init(aTexturepath);
		myName = "Sprite";
		mySize = { mySprite.GetSize().x, mySprite.GetSize().y };
		mySize.x *= myParent->GetSize().x;
		mySize.y *= myParent->GetSize().y;
		myPosition = { myParent->GetPosition().x, myParent->GetPosition().y };
		myPosition = mySprite.GetPosition();
		myShaderType = ShaderType::eDefault;
		myRotation = 0;
		myShouldRender = true;
	}

	~UISprite() override {}

	void Update() override;

	void Render() override
	{
		if (myIsActive && myShouldRender)
		{
			mySprite.Draw(myShaderType);
		}
	}

	void ImguiUpdate() override
	{
		ImGui::PushID("Sprite");
		if (ImGui::CollapsingHeader("SpriteComponent"))
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

			// change rotation with imgui
			float rotation[1] = { myRotation };
			ImGui::DragFloat("Rotation", rotation, 1.f, 0, 360.f);
			if (rotation[0] != myRotation)
			{
				myRotation = rotation[0];
				SetRotation(myParent->GetRotation());
			}
			//change texture with imgui

			ImGui::Text("Texture");
			ImGui::Image(mySprite.GetTexture().GetShaderResourceView(), ImVec2(150, 150), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 1));
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(".dds"))
				{
					Texture texture;
					std::string* payLoadPath = static_cast<std::string*>(payload->Data);
					std::string payLoadString = *payLoadPath;
					if (payLoadString == "")
					{
						try
						{
							payLoadString = std::any_cast<std::string>(MainSingleton::GetInstance().GetEventSystem().aTempData);
						}
						catch (const std::exception&)
						{
							LogWarning("Failed to cast any to string, you stupid");
						}

					}
					size_t found = payLoadString.rfind("UI_Assets");
					std::string temp = payLoadString.substr(found, payLoadString.size());
					payLoadString = RELATIVE_ASSET_ASSET_PATH + temp;

					if (!mySprite.SetTexture(payLoadString))
					{
					}
				}
				ImGui::EndDragDropTarget();
			}
			//change shadertype with dropdown
			ImGui::Text("ShaderType");
			const char* items[] = { "Default", "Morph(CHANGE COMPONENT, MORPH SPRITE)", "Glitch" };
			static int item_current = static_cast<int>(myShaderType);
			ImGui::Combo("ShaderType", &item_current, items, IM_ARRAYSIZE(items));
			myShaderType = static_cast<ShaderType>(item_current);
			SetShaderType(myShaderType);
		}
		ImGui::PopID();
	}

	void SetPosition(Vector2f /*aPosition*/) override
	{
		Vector2f pos = myPosition + myParent->GetPosition();
		mySprite.SetPosition(pos);
	}
	void SetRotation(float /*aRotation*/) override
	{
		float rot = myRotation + myParent->GetRotation();
		mySprite.SetRotation(rot);
	}
	void SetSize(Vector2f /*aSize*/) override
	{
		Vector2f size = mySize * myParent->GetSize(); /* + aSize*/;
		mySprite.SetSize(size);
	}

	Sprite& GetSprite() { return mySprite; }

	const Vector4f GetColor() { return mySprite.GetColor(); }
	void SetColor(Vector4f& aColor) { mySprite.SetColor(aColor); }

	float GetWidth() { return (float)mySprite.GetTexture().GetWidth(); }
	float GetHeight() { return (float)mySprite.GetTexture().GetHeight(); }

	const float GetAlpha() { return mySprite.GetColor().w; }
	void SetAlpha(float aAlpha) { Vector4f a = mySprite.GetColor(); a.w = aAlpha;  mySprite.SetColor(a); }

	void CropSprite(float top = FLT_MAX, float bottom = FLT_MAX, float right = FLT_MAX, float left = FLT_MAX)
	{
		RECT test = mySprite.GetRect();
		float height = (float)mySprite.GetTexture().GetHeight();
		float width = (float)mySprite.GetTexture().GetWidth();

		if (top != FLT_MAX)
		{
			test.top = (LONG)(top * height);
		}
		if (bottom != FLT_MAX)
		{
			test.bottom = (LONG)(bottom * height);
		}
		if (right != FLT_MAX)
		{
			test.right = (LONG)(right * width);
		}
		if (left != FLT_MAX)
		{
			test.left = (LONG)(left * width);
		}

		mySprite.SetRect(test);
	}

	void SetShaderType(ShaderType aType) { myShaderType = aType; }
	ShaderType GetShaderType() { return myShaderType; }


	void SetLocalPosition(Vector2f aPosition) { myPosition = aPosition; }
	void SetLocalSize(Vector2f aSize) { mySize = aSize; }
	void SetLocalRotation(float aRotation) { myRotation = aRotation; }

	void SetShouldRender(bool aShouldRender) { myShouldRender = aShouldRender; }

private:
	Sprite mySprite;
	ShaderType myShaderType;
	bool myShouldRender = true;
};
