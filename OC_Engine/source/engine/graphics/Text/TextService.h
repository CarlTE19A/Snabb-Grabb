#pragma once
#include "graphics\Text\Text.h"
#include <unordered_map>
#include <DirectXTK\SpriteBatch.h>

#include <wrl/client.h>

using Microsoft::WRL::ComPtr;
struct ID3D11DeviceContext;
struct ID3D11Buffer;

class Texture;
class Text;
class Shader;
class TextService
{

public:

	bool Init();

	void RenderText(Text& aTextToRender);

private:
	std::unique_ptr<DirectX::SpriteBatch> spriteBatch;
	Vector2f myScaleSize;

	//public:
//	TextService();
//	~TextService();
//
//	void Init();
//
//	Font GetOrLoad(std::wstring aFontPathAndName, FontSize aFontSize, unsigned char aBorderSize = 0);
//	bool Draw(Text& aText, Shader* aCustomShaderToRenderWith = nullptr);
//	float GetSentenceWidth(Text& aText);
//	float GetSentenceHeight(Text& aText);
//
//private:
//	friend Text;
//	struct FT_LibraryRec_* myLibrary;
//
//	std::unordered_map<std::wstring, std::weak_ptr<InternalTextAndFontData>> myFontData;
};
