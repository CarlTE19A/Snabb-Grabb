#pragma once
#include "UIComponent.h"
#include "mainSingelton\MainSingelton.h"
#include "events\EventSystem.h"
#include "Paths.h"
#include "graphics/Texture.h"

struct UV
{
	UV(Vector2f aStart, Vector2f aEnd) { myStart = aStart; myEnd = aEnd; }
	Vector2f myStart;
	Vector2f myEnd;
};


class UISpriteSheet : public UIComponent
{
public:
	friend class UIHandler;
	UISpriteSheet(std::string aTexturepath, UIElement* aParent)
	{
		myParent = aParent;
		mySprite.Init(aTexturepath);
		myName = "UISpriteSheet";
		mySize = { mySprite.GetSize().x, mySprite.GetSize().y };
		mySize.x *= myParent->GetSize().x;
		mySize.y *= myParent->GetSize().y;
		myPosition = { myParent->GetPosition().x, myParent->GetPosition().y };
		myPosition = mySprite.GetPosition();
		myRotation = 0;

		myRow = 1;
		myCol = 1;
		myMissing = 0;
		myTime = 2.0f;
		myIsPlaying = false;
		myLoop = false;
		myTimer = 0.0f;
		myIndex = 0;
		myHeight = (long)mySprite.GetTexture().GetHeight();
		myWidth = (long)mySprite.GetTexture().GetWidth();
		SetUVs();
	}

	~UISpriteSheet() override {}

	void Update() override;

	void Render() override
	{
		if (myIsActive)
		{
			mySprite.Draw();
		}
	}

	void ImguiUpdate() override
	{
		ImGui::PushID("UISpriteSheet");
		if (ImGui::CollapsingHeader("SpriteSheetComponent"))
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

			//change rows and colums with imgui
			int row = myRow;
			ImGui::DragInt("Row", &row, 1.f, 0, 100);
			if (row != myRow)
			{
				myRow = row;
				SetRow(myRow);
			}

			int col = myCol;
			ImGui::DragInt("Col", &col, 1.f, 0, 100);
			if (col != myCol)
			{
				myCol = col;
				SetCol(myCol);
			}
				
			int missing = myMissing;
			ImGui::DragInt("Missing", &missing, 1.f, 0, 100);
			if (missing != myMissing)
			{
				myMissing = missing;
				SetMissing(myMissing);
			}

			//set time with imgui
			float time = myTime;
			ImGui::DragFloat("Time", &time, 0.1f, 0.0f, 100.0f);
			if (time != myTime)
			{
				myTime = time;
			}


			//set is looping with imgui
			bool loop = myLoop;
			ImGui::Checkbox("Loop", &loop);

			if (loop != myLoop)
			{
				myLoop = loop;
				myIsPlaying = loop;
			}

			//play with imgui
			if (ImGui::Button("Play"))
			{
				Play();
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
					payLoadString = RELATIVE_ASSET_ASSET_PATH + temp;

					if (!mySprite.SetTexture(payLoadString))
					{

					}
					myHeight = (long)mySprite.GetTexture().GetHeight();
					myWidth = (long)mySprite.GetTexture().GetWidth();
				}
				ImGui::EndDragDropTarget();
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

	void SetRow(int aRow) { myRow = aRow; SetUVs(); }
	void SetCol(int aCol) { myCol = aCol; SetUVs(); }
	void SetMissing(int aMissing) { myMissing = aMissing; SetUVs(); }

	void Play()
	{
		myTimer = 0.0f;
		myIsPlaying = true;
		myIndex = 0;
	}

	void SetUVs(std::vector<UV> aUVs) { myUvs = aUVs; }

	void SetUVs() 
	{
		myUvs.clear();
		const float addingUVX = 1.0f / myRow;
		const float addingUVY = 1.0f / myCol;
		for (int j = 0; j < myCol; j++)
		{
			for (int i = 0; i < myRow; i++)
			{
				if (j == myCol - 1 && i > myRow - myMissing)
				{
					break;
				}
				myUvs.push_back(UV({ addingUVX * i, addingUVY * j }, { (addingUVX * i) + addingUVX, (addingUVY * j) + addingUVY }));
			}
		}

		//myTextureRect.left = (long)myUvs[0].myStart.x * myHeight;
		//myTextureRect.top = (long)myUvs[0].myStart.y * myHeight;
		//myTextureRect.right = (long)myUvs[0].myEnd.x * myWidth;
		//myTextureRect.bottom = (long)myUvs[0].myEnd.y * myWidth;
		LONG left = (LONG)(myUvs[0].myStart.x * (float)myHeight);
		LONG top = (LONG)(myUvs[0].myStart.y * (float)myHeight);
		LONG right = (LONG)(myUvs[0].myEnd.x * (float)myWidth);
		LONG bottom = (LONG)(myUvs[0].myEnd.y * (float)myWidth);
		myTextureRect = { left, top, right, bottom };
		//RECT textureRect = { (LONG)myUvs[myIndex].myStart.x * myHeight, (LONG)myUvs[myIndex].myStart.y * myHeight, (LONG)myUvs[myIndex].myEnd.x * myWidth, (LONG)myUvs[myIndex].myEnd.y * myWidth };
		mySprite.SetRect(myTextureRect);
	}

private:
	Sprite mySprite;
	unsigned short myIndex;
	std::vector<UV> myUvs;
	int myRow;
	int myCol;
	int myMissing;
	float myTime;
	bool myIsPlaying;
	bool myLoop;
	float myTimer;
	RECT myTextureRect;
	long myHeight;
	long myWidth;
};
