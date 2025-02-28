#include "Text.h"
#include "stdafx.h"
#include "Text.h"
#include "graphics\GraphicsEngine.h"

#include <d3d11.h>

#include "TextService.h"

#include <string>

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui\imgui_stdlib.h"

#include <DirectXMath.h>

Text::Text()
{
	myColor = { 1,1,1,1 };
	myPosition = { 600,600 };
	myRotation = 0;
	myScale = { 1,1 };
	myText = "Hello World";
	myFontSize = FontSize_48;
	myFontname = L"VCR";
}

Text::~Text()
{
}

bool Text::Init(FontSize aFontSize)
{
	myFontSize = aFontSize;

	SetFontSize(aFontSize);

	return true;
}

void Text::SetFontSize(FontSize aFontSize)
{
	myFontSize = aFontSize;

	auto device = GraphicsEngine::GetInstance()->GetDevice().Get();

	std::string textpath = STATIC_ASSET_PATH "Text";
	std::wstring fontPath = myFontname + std::to_wstring(myFontSize) + L".spritefont";

	std::wstring textpathWide(textpath.begin(), textpath.end());

	std::wstring fullPath = textpathWide + L"/" + fontPath;

	//test;
	myFont = std::make_unique<DirectX::SpriteFont>(device, fullPath.data());
}

FontSize& Text::GetFontSize()
{
	return myFontSize;
}

void Text::SetOriginInMiddle() //Set the origin in the center of the text
{
	Vector2f origin = { DirectX::XMVectorGetX(myFont->MeasureString(myText.c_str())),  DirectX::XMVectorGetY(myFont->MeasureString(myText.c_str())) }/*/ 2.f*/;

	origin.x = origin.x / 2;
	origin.y = origin.y / 2;

	myOrigin = origin;
}

void Text::Draw()
{
	GraphicsEngine::GetInstance()->GetTextService().RenderText(*this);
}