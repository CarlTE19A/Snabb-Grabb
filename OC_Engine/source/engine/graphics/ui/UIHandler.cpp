#include "stdafx.h"

#include "UIHandler.h"
#include "UIButton.h"
#include "UIElement.h"
#include "UISprite.h"
#include "UIClock.h"
#include "UIText.h"
#include "UITextField.h"
#include "UISpriteSheet.h"
#include "UISpriteMorph.h"

#include "../nlohmann/json.hpp"
#include <fstream>

void UIHandler::Init(std::string aName)
{
	myName = aName;
}

UIHandler::~UIHandler()
{
	for (UIElement* element : myElements)
	{
		delete element;
	}
	myElements.clear();
	myIDMap.clear();
}


void UIHandler::Update(float /*aDeltaTime*/)
{
	for (UIElement* element : myElements)
	{
		element->Update();
	}
}

void UIHandler::Draw()
{
	for (UIElement* element : myElements)
	{
		element->Render();
	}
}

void UIHandler::CreateUIElement()
{
	myElements.push_back(new UIElement(true));
	myIDMap[myElements.back()->GetID()] = myElements.back();
}

void UIHandler::CreateUIButton(const std::vector<std::string> aPaths, Vector2f aPosition, Vector2f& aSize)
{
	myElements.push_back(new UIElement(true));
	myElements.back()->SetPosition(aPosition);
	myElements.back()->SetSize(aSize);
	myElements.back()->AddComponent(new UIButton(aPaths, myElements.back()));
	//myButtons.push_back(new UIButton(aPaths));
	//if (!myButtons.back()->Init(aPath))
	//{
	//	LogError("FAILED TO LOAD FROM PATH: " + aPath);
	//	return;
	//}
	Vector2<int> window = GraphicsEngine::GetInstance()->GetWindowSize();

	//myButtons.back()->GetSprite().SetPosition(aPosition);
	//myButtons.back()->GetSprite().SetSize(aSize);
	//myElements.push_back(myButtons.back());
}

void UIHandler::CreateSprite(const std::string& aPath, Vector2f& aPosition, Vector2f& aSize)
{
	myElements.push_back(new UIElement(true));
	myElements.back()->SetPosition(aPosition);
	myElements.back()->SetSize(aSize);
	myElements.back()->AddComponent(new UISprite(aPath, myElements.back()));
	//mySprites.push_back(new UISprite(true));
	//if (!mySprites.back()->Init(aPath))
	//{
	//	LogError("FAILED TO LOAD FROM PATH: " + aPath);
	//	return;
	//}
	//mySprites.back()->SetPosition(aPosition);
	//mySprites.back()->SetSize(aSize);
	//myElements.push_back(mySprites.back());
}

void UIHandler::ReadUILayout()
{
	std::ifstream file;
	std::string filepath = STATIC_ASSET_PATH;
	filepath += "UILayouts/";
	filepath += myName;


	file.open(filepath.c_str());
	if (file.is_open())
	{
		nlohmann::json json;
		file >> json;
		try
		{
			for (size_t i = 0; i < json["elements"].size(); i++)
			{
				int id = json["elements"][i]["id"];
				Vector2f position = Vector2f(json["elements"][i]["position"]["x"], json["elements"][i]["position"]["y"]);
				Vector2f size = Vector2f(json["elements"][i]["size"]["x"], json["elements"][i]["size"]["y"]);
				float rotation = json["elements"][i]["rotation"];

				myElements.push_back(new UIElement(id, json["elements"][i]["active"]));
				myIDMap[myElements[i]->GetID()] = myElements[i];
				myElements[i]->SetName(json["elements"][i]["name"]);
				myElements[i]->SetPosition(position);
				myElements[i]->SetSize(size);
				myElements[i]->SetRotation(rotation);
			}

			for (size_t i = 0; i < json["spriteComponent"].size(); i++)
			{
				int id = json["spriteComponent"][i]["id"];
				std::string path = json["spriteComponent"][i]["path"];
				UISprite* sprite = new UISprite(path, myIDMap[id]);
				myIDMap[id]->AddComponent(sprite);
				sprite->myPosition = Vector2f(json["spriteComponent"][i]["position"]["x"], json["spriteComponent"][i]["position"]["y"]);
				sprite->mySize = Vector2f(json["spriteComponent"][i]["size"]["x"], json["spriteComponent"][i]["size"]["y"]);
				sprite->myRotation = json["spriteComponent"][i]["rotation"];
				sprite->myShaderType = json["spriteComponent"][i]["shaderType"];
				sprite->SetPosition(myIDMap[id]->GetPosition());
				sprite->SetSize(myIDMap[id]->GetSize());
				sprite->SetRotation(myIDMap[id]->GetRotation());
			}

			for (size_t i = 0; i < json["spriteSheetComponent"].size(); i++)
			{
				int id = json["spriteSheetComponent"][i]["id"];
				std::string path = json["spriteSheetComponent"][i]["path"];
				UISpriteSheet* spriteSheet = new UISpriteSheet(path, myIDMap[id]);
				myIDMap[id]->AddComponent(spriteSheet);
				spriteSheet->myPosition = Vector2f(json["spriteSheetComponent"][i]["position"]["x"], json["spriteSheetComponent"][i]["position"]["y"]);
				spriteSheet->mySize = Vector2f(json["spriteSheetComponent"][i]["size"]["x"], json["spriteSheetComponent"][i]["size"]["y"]);
				spriteSheet->myRotation = json["spriteSheetComponent"][i]["rotation"];
				spriteSheet->myCol = json["spriteSheetComponent"][i]["columns"];
				spriteSheet->myRow = json["spriteSheetComponent"][i]["rows"];
				spriteSheet->myMissing = json["spriteSheetComponent"][i]["missing"];
				spriteSheet->myTime = json["spriteSheetComponent"][i]["time"];
				spriteSheet->myLoop = json["spriteSheetComponent"][i]["loop"];
				spriteSheet->SetPosition(myIDMap[id]->GetPosition());
				spriteSheet->SetSize(myIDMap[id]->GetSize());
				spriteSheet->SetRotation(myIDMap[id]->GetRotation());
				spriteSheet->SetUVs();
			}

			for (size_t i = 0; i < json["spriteMorphComponent"].size(); i++)
			{
				int id = json["spriteMorphComponent"][i]["id"];
				std::string path = json["spriteMorphComponent"][i]["path"];
				UISpriteMorph* spriteMorph = new UISpriteMorph(path, myIDMap[id]);
				myIDMap[id]->AddComponent(spriteMorph);
				spriteMorph->myPosition = Vector2f(json["spriteMorphComponent"][i]["position"]["x"], json["spriteMorphComponent"][i]["position"]["y"]);
				spriteMorph->mySize = Vector2f(json["spriteMorphComponent"][i]["size"]["x"], json["spriteMorphComponent"][i]["size"]["y"]);
				spriteMorph->myRotation = json["spriteMorphComponent"][i]["rotation"];
				spriteMorph->myMorphRadius = Vector4f(json["spriteMorphComponent"][i]["morphRadius"]["x"], json["spriteMorphComponent"][i]["morphRadius"]["y"], json["spriteMorphComponent"][i]["morphRadius"]["z"], json["spriteMorphComponent"][i]["morphRadius"]["w"]);
				spriteMorph->myMorphSpeed = Vector4f(json["spriteMorphComponent"][i]["morphSpeed"]["x"], json["spriteMorphComponent"][i]["morphSpeed"]["y"], json["spriteMorphComponent"][i]["morphSpeed"]["z"], json["spriteMorphComponent"][i]["morphSpeed"]["w"]);

				spriteMorph->SetPosition(myIDMap[id]->GetPosition());
				spriteMorph->SetSize(myIDMap[id]->GetSize());
				spriteMorph->SetRotation(myIDMap[id]->GetRotation());
				spriteMorph->SetMorphRadius(spriteMorph->myMorphRadius);
				spriteMorph->SetMorphSpeed(spriteMorph->myMorphSpeed);
			}

			for (size_t i = 0; i < json["buttonComponent"].size(); i++)
			{
				int id = json["buttonComponent"][i]["id"];
				std::vector<std::string> paths;
				for (size_t j = 0; j < json["buttonComponent"][i]["paths"].size(); j++)
				{
					paths.push_back(json["buttonComponent"][i]["paths"][j]);
				}
				myIDMap[id]->AddComponent(new UIButton(paths, myIDMap[id]));
				myIDMap[id]->GetComponents<UIButton>()[0]->myButtonFunction = json["buttonComponent"][i]["function"];
				myIDMap[id]->GetComponents<UIButton>()[0]->SetPosition(myIDMap[id]->GetPosition());
			}

			for (size_t i = 0; i < json["textComponent"].size(); i++)
			{
				int id = json["textComponent"][i]["id"];
				std::string text = json["textComponent"][i]["text"];
				UIText* textComponent = new UIText(text, myIDMap[id]);
				myIDMap[id]->AddComponent(textComponent);
				textComponent->myPosition = Vector2f(json["textComponent"][i]["position"]["x"], json["textComponent"][i]["position"]["y"]);
				textComponent->mySize = Vector2f(json["textComponent"][i]["size"]["x"], json["textComponent"][i]["size"]["y"]);
				textComponent->myRotation = json["textComponent"][i]["rotation"];
				textComponent->SetText(text);
				//textComponent->myFontSize = static_cast<FontSize>(json["textComponent"][i]["fontSize"]);
				Vector4f color = Vector4f(json["textComponent"][i]["color"]["r"], json["textComponent"][i]["color"]["g"], json["textComponent"][i]["color"]["b"], json["textComponent"][i]["color"]["a"]);
				textComponent->SetColor(color);
				textComponent->SetPosition(myIDMap[id]->GetPosition());
				textComponent->SetSize(myIDMap[id]->GetSize());
				textComponent->SetRotation(myIDMap[id]->GetRotation());
			}

			for (size_t i = 0; i < json["textFieldComponent"].size(); i++)
			{
				int id = json["textFieldComponent"][i]["id"];
				std::string text = json["textFieldComponent"][i]["text"];
				UITextField* textField = new UITextField(text, myIDMap[id]);
				myIDMap[id]->AddComponent(textField);
				textField->myPosition = Vector2f(json["textFieldComponent"][i]["position"]["x"], json["textFieldComponent"][i]["position"]["y"]);
				textField->mySize = Vector2f(json["textFieldComponent"][i]["size"]["x"], json["textFieldComponent"][i]["size"]["y"]);
				textField->myRotation = json["textFieldComponent"][i]["rotation"];
				textField->SetText(text);
				textField->SetPosition(myIDMap[id]->GetPosition());
				textField->SetSize(myIDMap[id]->GetSize());
				textField->SetRotation(myIDMap[id]->GetRotation());
			}
		}
		catch (const std::exception&)
		{
			LogError("Failed to load UILayout for " + filepath);
		}
	}
	else
	{
		nlohmann::json json;
		int savedSIt = 0;
		int savedBIt = 0;
		int savedTIt = 0;
		int savedTFIt = 0;
		int savedSSIt = 0;
		int savedSMIt = 0;
		for (size_t i = 0; i < myElements.size(); i++)
		{
			int spriteIt = 0;
			int buttonIt = 0;
			int textIt = 0;
			int textFieldIt = 0;
			int spriteSheetIt = 0;
			int spriteMorphIt = 0;
			json["elements"][i]["id"] = myElements[i]->GetID();
			json["elements"][i]["active"] = myElements[i]->IsActive();
			json["elements"][i]["name"] = myElements[i]->GetName();
			json["elements"][i]["position"]["x"] = myElements[i]->GetPosition().x;
			json["elements"][i]["position"]["y"] = myElements[i]->GetPosition().y;
			json["elements"][i]["size"]["x"] = myElements[i]->GetSize().x;
			json["elements"][i]["size"]["y"] = myElements[i]->GetSize().y;
			json["elements"][i]["rotation"] = myElements[i]->GetRotation();

			for (size_t j = 0; j < myElements[i]->GetComponents<UISprite>().size(); j++)
			{
				UISprite* sprite = myElements[i]->GetComponents<UISprite>()[j];
				json["spriteComponent"][savedSIt + j]["id"] = myElements[i]->GetID();
				json["spriteComponent"][savedSIt + j]["path"] = sprite->GetSprite().GetTexture().GetPath();
				json["spriteComponent"][savedSIt + j]["position"]["x"] = sprite->GetPosition().x;
				json["spriteComponent"][savedSIt + j]["position"]["y"] = sprite->GetPosition().y;
				json["spriteComponent"][savedSIt + j]["size"]["x"] = sprite->GetSize().x;
				json["spriteComponent"][savedSIt + j]["size"]["y"] = sprite->GetSize().y;
				json["spriteComponent"][savedSIt + j]["rotation"] = sprite->GetRotation();
				json["spriteComponent"][savedSIt + j]["shaderType"] = static_cast<int>(sprite->GetShaderType());
				spriteIt++;
			}

			for (size_t j = 0; j < myElements[i]->GetComponents<UISpriteSheet>().size(); j++)
			{
				UISpriteSheet* spriteSheet = myElements[i]->GetComponents<UISpriteSheet>()[j];
				json["spriteSheetComponent"][savedSSIt + j]["id"] = myElements[i]->GetID();
				json["spriteSheetComponent"][savedSSIt + j]["path"] = spriteSheet->GetSprite().GetTexture().GetPath();
				json["spriteSheetComponent"][savedSSIt + j]["position"]["x"] = spriteSheet->GetPosition().x;
				json["spriteSheetComponent"][savedSSIt + j]["position"]["y"] = spriteSheet->GetPosition().y;
				json["spriteSheetComponent"][savedSSIt + j]["size"]["x"] = spriteSheet->GetSize().x;
				json["spriteSheetComponent"][savedSSIt + j]["size"]["y"] = spriteSheet->GetSize().y;
				json["spriteSheetComponent"][savedSSIt + j]["rotation"] = spriteSheet->GetRotation();
				json["spriteSheetComponent"][savedSSIt + j]["columns"] = spriteSheet->myCol;
				json["spriteSheetComponent"][savedSSIt + j]["rows"] = spriteSheet->myRow;
				json["spriteSheetComponent"][savedSSIt + j]["missing"] = spriteSheet->myMissing;
				json["spriteSheetComponent"][savedSSIt + j]["time"] = spriteSheet->myTime;
				json["spriteSheetComponent"][savedSSIt + j]["loop"] = spriteSheet->myLoop;
				spriteSheetIt++;
			}

			for (size_t j = 0; j < myElements[i]->GetComponents<UISpriteMorph>().size(); j++)
			{
				UISpriteMorph* spriteMorph = myElements[i]->GetComponents<UISpriteMorph>()[j];
				json["spriteMorphComponent"][savedSMIt + j]["id"] = myElements[i]->GetID();
				json["spriteMorphComponent"][savedSMIt + j]["path"] = spriteMorph->GetSprite().GetTexture().GetPath();
				json["spriteMorphComponent"][savedSMIt + j]["position"]["x"] = spriteMorph->GetPosition().x;
				json["spriteMorphComponent"][savedSMIt + j]["position"]["y"] = spriteMorph->GetPosition().y;
				json["spriteMorphComponent"][savedSMIt + j]["size"]["x"] = spriteMorph->GetSize().x;
				json["spriteMorphComponent"][savedSMIt + j]["size"]["y"] = spriteMorph->GetSize().y;
				json["spriteMorphComponent"][savedSMIt + j]["rotation"] = spriteMorph->GetRotation();
				json["spriteMorphComponent"][savedSMIt + j]["morphRadius"]["x"] = spriteMorph->myMorphRadius.x;
				json["spriteMorphComponent"][savedSMIt + j]["morphRadius"]["y"] = spriteMorph->myMorphRadius.y;
				json["spriteMorphComponent"][savedSMIt + j]["morphRadius"]["z"] = spriteMorph->myMorphRadius.z;
				json["spriteMorphComponent"][savedSMIt + j]["morphRadius"]["w"] = spriteMorph->myMorphRadius.w;
				json["spriteMorphComponent"][savedSMIt + j]["morphSpeed"]["x"] = spriteMorph->myMorphSpeed.x;
				json["spriteMorphComponent"][savedSMIt + j]["morphSpeed"]["y"] = spriteMorph->myMorphSpeed.y;
				json["spriteMorphComponent"][savedSMIt + j]["morphSpeed"]["z"] = spriteMorph->myMorphSpeed.z;
				json["spriteMorphComponent"][savedSMIt + j]["morphSpeed"]["w"] = spriteMorph->myMorphSpeed.w;
				spriteMorphIt++;
			}


			for (size_t j = 0; j < myElements[i]->GetComponents<UIButton>().size(); j++)
			{
				json["buttonComponent"][savedBIt + j]["id"] = myElements[i]->GetID();
				for (size_t k = 0; k < myElements[i]->GetComponents<UIButton>()[j]->GetPaths().size(); k++)
				{
					json["buttonComponent"][savedBIt + j]["paths"][k] = myElements[i]->GetComponents<UIButton>()[j]->GetPaths()[k];
				}
				json["buttonComponent"][savedBIt + j]["function"] = static_cast<int>(myElements[i]->GetComponents<UIButton>()[j]->GetFunction());

				buttonIt++;
			}

			for (size_t j = 0; j < myElements[i]->GetComponents<UIText>().size(); j++)
			{
				UIText* text = myElements[i]->GetComponents<UIText>()[j];
				json["textComponent"][savedTIt + j]["id"] = myElements[i]->GetID();
				json["textComponent"][savedTIt + j]["text"] = text->GetText();
				json["textComponent"][savedTIt + j]["position"]["x"] = text->GetPosition().x;
				json["textComponent"][savedTIt + j]["position"]["y"] = text->GetPosition().y;
				json["textComponent"][savedTIt + j]["size"]["x"] = text->GetSize().x;
				json["textComponent"][savedTIt + j]["size"]["y"] = text->GetSize().y;
				json["textComponent"][savedTIt + j]["rotation"] = text->GetRotation();
				//json["textComponent"][savedTIt + j]["fontSize"] = text->myFontSize;
				json["textComponent"][savedTIt + j]["color"]["r"] = text->GetColor().x;
				json["textComponent"][savedTIt + j]["color"]["g"] = text->GetColor().y;
				json["textComponent"][savedTIt + j]["color"]["b"] = text->GetColor().z;
				json["textComponent"][savedTIt + j]["color"]["a"] = text->GetColor().w;
				textIt++;
			}

			for (size_t j = 0; j < myElements[i]->GetComponents<UITextField>().size(); j++)
			{
				UITextField* textField = myElements[i]->GetComponents<UITextField>()[j];
				json["textFieldComponent"][savedTFIt + j]["id"] = myElements[i]->GetID();
				json["textFieldComponent"][savedTFIt + j]["text"] = textField->GetText();
				json["textFieldComponent"][savedTFIt + j]["position"]["x"] = textField->GetPosition().x;
				json["textFieldComponent"][savedTFIt + j]["position"]["y"] = textField->GetPosition().y;
				json["textFieldComponent"][savedTFIt + j]["size"]["x"] = textField->GetSize().x;
				json["textFieldComponent"][savedTFIt + j]["size"]["y"] = textField->GetSize().y;
				json["textFieldComponent"][savedTFIt + j]["rotation"] = textField->GetRotation();
				textFieldIt++;
			}

			savedSIt += spriteIt;
			savedBIt += buttonIt;
			savedTIt += textIt;
			savedTFIt += textFieldIt;
			savedSSIt += spriteSheetIt;
			savedSMIt += spriteMorphIt;
		}
		std::ofstream fileOut;
		fileOut.open(filepath);
		if (fileOut.is_open())
		{
			fileOut << json.dump(4);
		}
		fileOut.close();
	}
	file.close();
}

void UIHandler::SaveUILayout()
{
	std::ifstream file;
	std::string filepath = STATIC_ASSET_PATH;
	filepath += "UILayouts/";
	filepath += myName;
	nlohmann::json json;

	int savedSIt = 0;
	int savedBIt = 0;
	int savedTIt = 0;
	int savedTFIt = 0;
	int savedSSIt = 0;
	int savedSMIt = 0;
	for (size_t i = 0; i < myElements.size(); i++)
	{
		int spriteIt = 0;
		int buttonIt = 0;
		int textIt = 0;
		int textFieldIt = 0;
		int spriteSheetIt = 0;
		int spriteMorphIt = 0;
		json["elements"][i]["id"] = myElements[i]->GetID();
		json["elements"][i]["active"] = myElements[i]->IsActive();
		json["elements"][i]["name"] = myElements[i]->GetName();
		json["elements"][i]["position"]["x"] = myElements[i]->GetPosition().x;
		json["elements"][i]["position"]["y"] = myElements[i]->GetPosition().y;
		json["elements"][i]["size"]["x"] = myElements[i]->GetSize().x;
		json["elements"][i]["size"]["y"] = myElements[i]->GetSize().y;
		json["elements"][i]["rotation"] = myElements[i]->GetRotation();

		for (size_t j = 0; j < myElements[i]->GetComponents<UISprite>().size(); j++)
		{
			UISprite* sprite = myElements[i]->GetComponents<UISprite>()[j];
			json["spriteComponent"][savedSIt + j]["id"] = myElements[i]->GetID();
			json["spriteComponent"][savedSIt + j]["path"] = sprite->GetSprite().GetTexture().GetPath();
			json["spriteComponent"][savedSIt + j]["position"]["x"] = sprite->GetPosition().x;
			json["spriteComponent"][savedSIt + j]["position"]["y"] = sprite->GetPosition().y;
			json["spriteComponent"][savedSIt + j]["size"]["x"] = sprite->GetSize().x;
			json["spriteComponent"][savedSIt + j]["size"]["y"] = sprite->GetSize().y;
			json["spriteComponent"][savedSIt + j]["rotation"] = sprite->GetRotation();
			json["spriteComponent"][savedSIt + j]["shaderType"] = static_cast<int>(sprite->GetShaderType());
			spriteIt++;
		}

		for (size_t j = 0; j < myElements[i]->GetComponents<UISpriteSheet>().size(); j++)
		{
			UISpriteSheet* spriteSheet = myElements[i]->GetComponents<UISpriteSheet>()[j];
			json["spriteSheetComponent"][savedSSIt + j]["id"] = myElements[i]->GetID();
			json["spriteSheetComponent"][savedSSIt + j]["path"] = spriteSheet->GetSprite().GetTexture().GetPath();
			json["spriteSheetComponent"][savedSSIt + j]["position"]["x"] = spriteSheet->GetPosition().x;
			json["spriteSheetComponent"][savedSSIt + j]["position"]["y"] = spriteSheet->GetPosition().y;
			json["spriteSheetComponent"][savedSSIt + j]["size"]["x"] = spriteSheet->GetSize().x;
			json["spriteSheetComponent"][savedSSIt + j]["size"]["y"] = spriteSheet->GetSize().y;
			json["spriteSheetComponent"][savedSSIt + j]["rotation"] = spriteSheet->GetRotation();
			json["spriteSheetComponent"][savedSSIt + j]["columns"] = spriteSheet->myCol;
			json["spriteSheetComponent"][savedSSIt + j]["rows"] = spriteSheet->myRow;
			json["spriteSheetComponent"][savedSSIt + j]["missing"] = spriteSheet->myMissing;
			json["spriteSheetComponent"][savedSSIt + j]["time"] = spriteSheet->myTime;
			json["spriteSheetComponent"][savedSSIt + j]["loop"] = spriteSheet->myLoop;
			spriteSheetIt++;
		}

		for (size_t j = 0; j < myElements[i]->GetComponents<UISpriteMorph>().size(); j++)
		{
			UISpriteMorph* spriteMorph = myElements[i]->GetComponents<UISpriteMorph>()[j];
			json["spriteMorphComponent"][savedSMIt + j]["id"] = myElements[i]->GetID();
			json["spriteMorphComponent"][savedSMIt + j]["path"] = spriteMorph->GetSprite().GetTexture().GetPath();
			json["spriteMorphComponent"][savedSMIt + j]["position"]["x"] = spriteMorph->GetPosition().x;
			json["spriteMorphComponent"][savedSMIt + j]["position"]["y"] = spriteMorph->GetPosition().y;
			json["spriteMorphComponent"][savedSMIt + j]["size"]["x"] = spriteMorph->GetSize().x;
			json["spriteMorphComponent"][savedSMIt + j]["size"]["y"] = spriteMorph->GetSize().y;
			json["spriteMorphComponent"][savedSMIt + j]["rotation"] = spriteMorph->GetRotation();
			json["spriteMorphComponent"][savedSMIt + j]["morphRadius"]["x"] = spriteMorph->myMorphRadius.x;
			json["spriteMorphComponent"][savedSMIt + j]["morphRadius"]["y"] = spriteMorph->myMorphRadius.y;
			json["spriteMorphComponent"][savedSMIt + j]["morphRadius"]["z"] = spriteMorph->myMorphRadius.z;
			json["spriteMorphComponent"][savedSMIt + j]["morphRadius"]["w"] = spriteMorph->myMorphRadius.w;
			json["spriteMorphComponent"][savedSMIt + j]["morphSpeed"]["x"] = spriteMorph->myMorphSpeed.x;
			json["spriteMorphComponent"][savedSMIt + j]["morphSpeed"]["y"] = spriteMorph->myMorphSpeed.y;
			json["spriteMorphComponent"][savedSMIt + j]["morphSpeed"]["z"] = spriteMorph->myMorphSpeed.z;
			json["spriteMorphComponent"][savedSMIt + j]["morphSpeed"]["w"] = spriteMorph->myMorphSpeed.w;
			spriteMorphIt++;
		}

		for (size_t j = 0; j < myElements[i]->GetComponents<UIButton>().size(); j++)
		{
			json["buttonComponent"][savedBIt + j]["id"] = myElements[i]->GetID();
			for (size_t k = 0; k < myElements[i]->GetComponents<UIButton>()[j]->GetPaths().size(); k++)
			{
				json["buttonComponent"][savedBIt + j]["paths"][k] = myElements[i]->GetComponents<UIButton>()[j]->GetPaths()[k];
			}
			json["buttonComponent"][savedBIt + j]["function"] = static_cast<int>(myElements[i]->GetComponents<UIButton>()[j]->GetFunction());
			buttonIt++;
		}

		for (size_t j = 0; j < myElements[i]->GetComponents<UIText>().size(); j++)
		{
			UIText* text = myElements[i]->GetComponents<UIText>()[j];
			json["textComponent"][savedTIt + j]["id"] = myElements[i]->GetID();
			json["textComponent"][savedTIt + j]["text"] = text->GetText();
			json["textComponent"][savedTIt + j]["position"]["x"] = text->GetPosition().x;
			json["textComponent"][savedTIt + j]["position"]["y"] = text->GetPosition().y;
			json["textComponent"][savedTIt + j]["size"]["x"] = text->GetSize().x;
			json["textComponent"][savedTIt + j]["size"]["y"] = text->GetSize().y;
			json["textComponent"][savedTIt + j]["rotation"] = text->GetRotation();
			//json["textComponent"][savedTIt + j]["fontSize"] = text->myFontSize;
			json["textComponent"][savedTIt + j]["color"]["r"] = text->GetColor().x;
			json["textComponent"][savedTIt + j]["color"]["g"] = text->GetColor().y;
			json["textComponent"][savedTIt + j]["color"]["b"] = text->GetColor().z;
			json["textComponent"][savedTIt + j]["color"]["a"] = text->GetColor().w;
			textIt++;
		}

		for (size_t j = 0; j < myElements[i]->GetComponents<UITextField>().size(); j++)
		{
			UITextField* textField = myElements[i]->GetComponents<UITextField>()[j];
			json["textFieldComponent"][savedTFIt + j]["id"] = myElements[i]->GetID();
			json["textFieldComponent"][savedTFIt + j]["text"] = textField->GetText();
			json["textFieldComponent"][savedTFIt + j]["position"]["x"] = textField->GetPosition().x;
			json["textFieldComponent"][savedTFIt + j]["position"]["y"] = textField->GetPosition().y;
			json["textFieldComponent"][savedTFIt + j]["size"]["x"] = textField->GetSize().x;
			json["textFieldComponent"][savedTFIt + j]["size"]["y"] = textField->GetSize().y;
			json["textFieldComponent"][savedTFIt + j]["rotation"] = textField->GetRotation();
			textFieldIt++;
		}

		savedSIt += spriteIt;
		savedBIt += buttonIt;
		savedTIt += textIt;
		savedTFIt += textFieldIt;
		savedSSIt += spriteSheetIt;
		savedSMIt += spriteMorphIt;
	}

	file.open(filepath.c_str());
	std::ofstream fileOut;
	fileOut.open(filepath);
	if (fileOut.is_open())
	{
		fileOut << json.dump(4);
	}
	fileOut.close();
}

void UIHandler::MoveElementToFront(int aID)
{
	UIElement* tempElement = myIDMap[aID];
	for (size_t i = 0; i < myElements.size(); i++)
	{
		if (myElements[i]->GetID() == aID)
		{
			if (i == myElements.size() - 1) return;
			myElements.erase(myElements.begin() + i);
			myElements.push_back(tempElement);
			break;
		}
	}
	myElements.erase(std::remove(myElements.begin(), myElements.end(), tempElement), myElements.end());
	myElements.push_back(tempElement);
}

void UIHandler::MoveElementToBack(int aID)
{
	UIElement* tempElement = myIDMap[aID];
	for (size_t i = 0; i < myElements.size(); i++)
	{
		if (myElements[i]->GetID() == aID)
		{
			if (i == 0) return;
			myElements.erase(myElements.begin() + i);
			myElements.insert(myElements.begin(), tempElement);
			break;
		}
	}
	myElements.erase(std::remove(myElements.begin(), myElements.end(), tempElement), myElements.end());
	myElements.insert(myElements.begin(), tempElement);
}

void UIHandler::MoveElementForward(int aID)
{
	//move element forward one step
	UIElement* tempElement = myIDMap[aID];
	for (size_t i = 0; i < myElements.size(); i++)
	{
		if (myElements[i]->GetID() == aID)
		{
			if (i == myElements.size() - 1) return;
			myElements.erase(myElements.begin() + i);
			myElements.insert(myElements.begin() + i + 1, tempElement);
			break;
		}
	}
	//save the id map position and then move it one step forward
	std::map<int, UIElement*>::iterator it = myIDMap.find(aID);
	it++;
	myIDMap.erase(aID);
	myIDMap.insert(it, std::pair<int, UIElement*>(aID, tempElement));
}

void UIHandler::MoveElementBackward(int aID)
{
	//move element backward one step
	UIElement* tempElement = myIDMap[aID];
	for (size_t i = 0; i < myElements.size(); i++)
	{
		if (myElements[i]->GetID() == aID)
		{
			if (i == 0) return;
			myElements.erase(myElements.begin() + i);
			myElements.insert(myElements.begin() + i - 1, tempElement);
			break;
		}
	}
	//save the id map position and then move it one step backwards

	std::map<int, UIElement*>::iterator it = myIDMap.find(aID);
	it--;
	myIDMap.erase(aID);
	myIDMap.insert(it, std::pair<int, UIElement*>(aID, tempElement));
}

void UIHandler::SwapElements(int aID1, int aID2)
{
	int tempElement1 = -1;
	int tempElement2 = -1;
	int a = 0;
	for (size_t i = 0; i < myElements.size(); i++)
	{
		if (myElements[i]->GetID() == aID1)
		{
			tempElement1 = (int)i;
			a++;
		}
		else if (myElements[i]->GetID() == aID2)
		{
			tempElement2 = (int)i;
			a++;
		}
	}
	if (a > 1)
	{
		std::swap(myElements[tempElement1], myElements[tempElement2]);
	}

	//std::map<int, UIElement*>::iterator it1 = myIDMap.find(aID1);
	//std::map<int, UIElement*>::iterator it2 = myIDMap.find(aID2);
	//myIDMap.erase(aID1);
	//myIDMap.erase(aID2);
	//myIDMap.insert(it1, std::pair<int, UIElement*>(aID1, tempElement2));
	//myIDMap.insert(it2, std::pair<int, UIElement*>(aID2, tempElement1));



}

void UIHandler::RemoveElement(int aID)
{
	for (size_t i = 0; i < myElements.size(); i++)
	{
		if (myElements[i]->GetID() == aID)
		{
			myElements.erase(myElements.begin() + i);
			break;
		}
	}
	myIDMap.erase(aID);
}
