#pragma once
#include "graphics\sprite\Sprite.h"
#include "math\Vector.h"
#include "UIComponent.h"
#include "UISprite.h"

enum class eButtonFunction
{
	eGame,
	eLevelSelect,
	eHowToPlay,
	eSettings,
	eCredits,
	eQuit,
	eCount
};

class UIButton : public UIComponent
{
public:
	friend class UIHandler;

	UIButton(std::vector<std::string> someTexturePaths, UIElement* aParent)
	{ 
		myParent = aParent;
		myIsHighlighted = false;
		myPaths = someTexturePaths;
		myName = "Button";
		for (int i = 0; i < someTexturePaths.size(); i++)
		{
			mySprites.push_back(new Sprite());
			mySprites[i]->Init(someTexturePaths[i]);
		}
		mySpritePtr = myParent->GetComponent<UISprite>();
		myPosition = mySpritePtr->GetPosition() + myParent->GetPosition();
		mySize = myParent->GetSize();
		mySize.x *= mySpritePtr->GetSize().x;
		mySize.y *= mySpritePtr->GetSize().y;
		myRotation = myParent->GetRotation();
		myRotation += mySpritePtr->GetRotation();
		myShouldRender = true;
		myButtonFunction = eButtonFunction::eCount;

		//myScaleSize = GraphicsEngine::GetInstance()->GetViewportDimensions();
		//myScaleSize = Vector2f((float)Engine::GetInstance()->GetWindowSize().x, (float)Engine::GetInstance()->GetWindowSize().y);
		myScaleSize = Vector2f(1920,1080);
	}

	~UIButton() override
	{
		for (int i = 0; i < mySprites.size(); i++)
		{
			delete mySprites[i];
		}
		mySprites.clear();
	}

	void Update() override;

	void Render() override;

	void ImguiUpdate() override
	{
		ImGui::PushID("Button" + myParent->GetID());
		if (ImGui::CollapsingHeader("ButtonComponent"))
		{
			ImGui::Checkbox("IsHighlighted", &myIsHighlighted);
			ImGui::Checkbox("active", &myIsActive);
			SetPosition(myPosition);
			SetSize(mySize);
			SetRotation(myRotation);
			ImGui::Text("Texture");

			ImGui::Image(mySprites[0]->GetTexture().GetShaderResourceView(), ImVec2(150, 150), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 1));
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
					if (!mySprites[0]->SetTexture(payLoadString))
					{
						//payLoadString = RELATIVE_ASSET_ASSET_PATH;
						//payLoadString += "UI_Assets/player cards";
						//payLoadString += temp;
						//mySprite.SetTexture(payLoadString);
					}
					myPaths[0] = payLoadString;
				}
				ImGui::EndDragDropTarget();
			}
			//change in a dropdown myButtonFunction
			const char* items1[] = { "Game", "LevelSelect", "HowToPlay", "Settings", "Credits", "Quit", "none"};
			int item_current1 = static_cast<int>(myButtonFunction);
			ImGui::Combo("Btn Funtion", &item_current1, items1, IM_ARRAYSIZE(items1));
			myButtonFunction = static_cast<eButtonFunction>(item_current1);
			SetFunction(myButtonFunction);

		}
		ImGui::PopID();
	}

	void SetPosition(Vector2f /*aPosition*/) override
	{
		myPosition = mySpritePtr->GetSprite().GetPosition();
		for (size_t i = 0; i < mySprites.size(); i++)
		{
			mySprites[i]->SetPosition(myPosition);
		}
	}

	void SetRotation(float /*aRotation*/) override
	{
		myRotation = mySpritePtr->GetSprite().GetRotation();
		for (size_t i = 0; i < mySprites.size(); i++)
		{
			mySprites[i]->SetRotation(myRotation);
		}

	}

	void SetSize(Vector2f /*aSize*/) override
	{
		mySize = mySpritePtr->GetSprite().GetSize();
		for (size_t i = 0; i < mySprites.size(); i++)
		{
			mySprites[i]->SetSize(mySize);
		}
	}

	const bool& GetIsHighlighted() { return myIsHighlighted; }
	//const bool& GetIsSelected() { return myIsSelected; }

	void SetShouldRender(bool aShouldRender) { myShouldRender = aShouldRender; }


	std::vector<std::string> GetPaths() { return myPaths; }

	bool IsSelected();

	eButtonFunction GetFunction() { return myButtonFunction; }
	void SetFunction(eButtonFunction aFunction) { myButtonFunction = aFunction; }

private:
	std::vector<Sprite*> mySprites;
	std::vector<std::string> myPaths;
	UISprite* mySpritePtr;
	Vector2<int> myMousePos;
	bool myIsHighlighted;
	bool myShouldRender;
	eButtonFunction myButtonFunction;

	Vector2f myScaleSize;
	//bool myIsSelected;
};

