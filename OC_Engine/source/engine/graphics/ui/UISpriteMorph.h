#pragma once
#include "UIComponent.h"
#include "mainSingelton\MainSingelton.h"
#include "events\EventSystem.h"

class UISpriteMorph : public UIComponent
{
public:
	friend class UIHandler;
	UISpriteMorph(std::string aTexturepath, UIElement* aParent)
	{
		myParent = aParent;
		mySprite.Init(aTexturepath);
		myName = "SpriteMorph";
		mySize = { mySprite.GetSize().x, mySprite.GetSize().y };
		mySize.x *= myParent->GetSize().x;
		mySize.y *= myParent->GetSize().y;
		myPosition = { myParent->GetPosition().x, myParent->GetPosition().y };
		myPosition = mySprite.GetPosition();

		myRotation = 0;
	}

	~UISpriteMorph() override {}

	void Update() override;

	void Render() override
	{
		//set constantbuffer values
		if (myIsActive)
		{
			mySprite.Draw(ShaderType::eMorph);
		}
	}

	void ImguiUpdate() override
	{
		ImGui::PushID("SpriteMorph");
		if (ImGui::CollapsingHeader("SpriteMorphComponent"))
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
			//std::vector<Texture&> textures(myMeshes[i]->GetTextureCount());
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
					//std::string temp1 = "SplashScreen";
					payLoadString = RELATIVE_ASSET_ASSET_PATH + temp;
					//payLoadString += temp;

					//texture.Initialize(TextureInitializationType::DDS, payLoadString);
					if (!mySprite.SetTexture(payLoadString))
					{
						//payLoadString = RELATIVE_ASSET_ASSET_PATH;
						//payLoadString += "UI_Assets/player cards";
						//payLoadString += temp;
						//mySprite.SetTexture(payLoadString);
					}
				}
				ImGui::EndDragDropTarget();
			}

			//set morph speed and radius
			float morphSpeed[4] = {
				myMorphSpeed.x,
				myMorphSpeed.y,
				myMorphSpeed.z,
				myMorphSpeed.w
			};
			ImGui::DragFloat4("MorphSpeed", morphSpeed, 0.01f);
			if (morphSpeed[0] != myMorphSpeed.x || morphSpeed[1] != myMorphSpeed.y || morphSpeed[2] != myMorphSpeed.z || morphSpeed[3] != myMorphSpeed.w)
			{
				myMorphSpeed = { morphSpeed[0], morphSpeed[1], morphSpeed[2], morphSpeed[3] };
				SetMorphSpeed(myMorphSpeed);
			}

			float morphRadius[4] = {
				myMorphRadius.x,
				myMorphRadius.y,
				myMorphRadius.z,
				myMorphRadius.w
			};
			ImGui::DragFloat4("MorphRadius", morphRadius, 0.01f);
			if (morphRadius[0] != myMorphRadius.x || morphRadius[1] != myMorphRadius.y || morphRadius[2] != myMorphRadius.z || morphRadius[3] != myMorphRadius.w)
			{
				myMorphRadius = { morphRadius[0], morphRadius[1], morphRadius[2], morphRadius[3] };
				SetMorphRadius(myMorphRadius);
			}
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
		//mySprite.SetRect(size);
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

	void SetMorphSpeed(Vector4f& aMorphSpeed) { myMorphSpeed = aMorphSpeed; mySprite.SetMorphSpeed(myMorphSpeed); }
	void SetMorphRadius(Vector4f& aMorphRadius) { myMorphRadius = aMorphRadius; mySprite.SetMorphRadius(myMorphRadius); }

private:
	Sprite mySprite;
	Vector4f myMorphSpeed;
	Vector4f myMorphRadius;
	Vector4f myColor;
};
