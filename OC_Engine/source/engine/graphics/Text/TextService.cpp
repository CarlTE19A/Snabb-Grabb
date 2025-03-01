#include "stdafx.h"
#include "TextService.h"
#include "graphics\GraphicsEngine.h"
#include <memory>

//
//#include <ft2build.h>
//#include <codecvt>
//#include <string>
//
//#include FT_FREETYPE_H
//#include FT_GLYPH_H
//#include FT_BITMAP_H
//#include FT_STROKER_H
//#include FT_OUTLINE_H
//
//#include "TextService.h"
//#include "graphics\sprite\SpriteDrawer.h"
//#include <fstream>
//#include <sstream>
//#include "graphics\GraphicsEngine.h"
//#include "TextMesh.h"
//
//
//#define X_OFFSET 4
//#define X_START 2
//#define Y_OFFSET 8
//
//using Microsoft::WRL::ComPtr;
//
//struct ID3D11ShaderResourceView;
//
//struct CharData
//{
//	Vector2f mymyStartXV;
//	Vector2f myBottomRightUV;
//
//	short myWidth;
//	short myHeight;
//	short myAdvanceX; 
//	short myAdvanceY;
//	short myBearingX;
//	short myBearingY;
//	char myChar;
//};
//
//class InternalTextAndFontData
//{
//public:
//	~InternalTextAndFontData();
//	CharData myCharData[1024];
//	unsigned int myAtlasWidth;
//	unsigned int myAtlasHeight;
//	float myLineSpacing;
//	unsigned int myWordSpacing;
//	unsigned int myFontHeightWidth;
//	std::vector<int> myAtlas;
//	std::wstring myName;
//	std::unique_ptr<Texture> myTexture;
//	ComPtr<ID3D11ShaderResourceView> myAtlasView;
//};
//
//
//struct TextToRender
//{
//	Vector2f mySize;
//	Vector2f myUvStart;
//	Vector2f myUvEnd;
//	Vector2f myPosition;
//	bool isWhitespace;
//};
//
//struct GlyphLoader
//{
//	struct CountData
//	{
//		int xNCount = 0;
//		int xPCount = 0;
//		int yNCount = 0;
//		int yPCount = 0;
//	};
//
//	struct OutlineOffset
//	{
//		int xDelta = 0;
//		int yDelta = 0;
//
//	};
//
//	void LoadGlyph(int index, int& atlasX, int& atlasY, int& maxY, float atlasWidth, float atlasHeight, InternalTextAndFontData* aFontData, struct FT_FaceRec_* aFace, int aBorderWidth = 0);
//	void LoadOutline(const int index, const int atlasX, const int atlasY, const float atlasWidth, InternalTextAndFontData* aFontData, struct FT_FaceRec_* aFace, int aBorderWidth);
//	void CalculateOutlineOffsets(const int index, struct FT_FaceRec_* aFace, int aBorderWidth);
//	void CalculateGlyphOffsets(const int index, struct FT_GlyphSlotRec_* glyph);
//
//	void CountOffsets(const int& x, const int& y, const int& width, const int& height, CountData& countData);
//	void CountDeltas(const int& width, const int& height, int& deltaX, int& deltaY, CountData& countData);
//
//	OutlineOffset myTextOutlineOffset;
//	struct FT_LibraryRec_* myLibrary;
//};
//
//void GlyphLoader::CountOffsets(const int& x, const int& y, const int& width, const int& height, CountData& countData)
//{
//	if (x < 1)
//	{
//		countData.xNCount++;
//	}
//
//	if (x > width - 1)
//	{
//		countData.xPCount++;
//	}
//
//	if (y < 1)
//	{
//		countData.yNCount++;
//	}
//
//	if (y > height - 1)
//	{
//		countData.yPCount++;
//	}
//}
//
//void GlyphLoader::CountDeltas(const int& width, const int& height, int& deltaX, int& deltaY, CountData& countData)
//{
//	if (countData.xNCount == height)
//	{
//		countData.xNCount = 0;
//		deltaX--;
//	}
//
//	if (countData.xPCount == height)
//	{
//		countData.xPCount = 0;
//		deltaX++;
//	}
//
//	if (countData.yNCount == width)
//	{
//		countData.yNCount = 0;
//		deltaY--;
//	}
//
//	if (countData.yPCount == width)
//	{
//		countData.yPCount = 0;
//		deltaY++;
//	}
//}
//
//void GlyphLoader::CalculateOutlineOffsets(const int index, FT_FaceRec_* aFace, int aBorderOffset)
//{
//	myTextOutlineOffset.xDelta = 0;
//	myTextOutlineOffset.yDelta = 0;
//
//	FT_Error err;
//	FT_Stroker stroker;
//	FT_Glyph glyph;
//
//	err = FT_Load_Char(aFace, index, FT_LOAD_NO_BITMAP);
//	if (err != 0)
//	{
//	}
//	err = FT_Get_Glyph(aFace->glyph, &glyph);
//
//	glyph->format = FT_GLYPH_FORMAT_OUTLINE;
//
//	err = FT_Stroker_New(myLibrary, &stroker);
//	if (err != 0)
//	{
//	}
//
//	FT_Stroker_Set(stroker, aBorderOffset * 64, FT_STROKER_LINECAP_ROUND, FT_STROKER_LINEJOIN_ROUND, 0);
//	err = FT_Glyph_StrokeBorder(&glyph, stroker, 0, 1);
//	if (err != 0)
//	{
//	}
//	err = FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_NORMAL, nullptr, true);
//	if (err != 0)
//	{
//	}
//
//	FT_BitmapGlyph bitmapGlyph = reinterpret_cast<FT_BitmapGlyph>(glyph);
//
//	unsigned int width = bitmapGlyph->bitmap.width;
//	unsigned int height = bitmapGlyph->bitmap.rows;
//
//	int xDelta = 0;
//	int yDelta = 0;
//
//	CountData countData;
//	for (unsigned int x = 0; x < width; x++)
//	{
//		for (unsigned int y = 0; y < height; y++)
//		{
//			unsigned char& data = bitmapGlyph->bitmap.buffer[y * width + x];
//
//			if (data == 0)
//			{
//				CountOffsets(x, y, width, height, countData);
//			}
//			CountDeltas(width, height, xDelta, yDelta, countData);
//		}
//	}
//
//	myTextOutlineOffset.xDelta = xDelta;
//	myTextOutlineOffset.yDelta = yDelta;
//
//	FT_Stroker_Done(stroker);
//}
//
//void GlyphLoader::CalculateGlyphOffsets(const int, FT_GlyphSlotRec_* glyph)
//{
//	int xDelta = 0;
//	int yDelta = 0;
//
//	unsigned int width = glyph->bitmap.width;
//	unsigned int height = glyph->bitmap.rows;
//
//	CountData countData;
//	for (unsigned int x = 0; x < width; x++)
//	{
//		for (unsigned int y = 0; y < height; y++)
//		{
//			unsigned char& data = glyph->bitmap.buffer[y * width + x];
//
//			if (data == 0)
//			{
//				CountOffsets(x, y, width, height, countData);
//			}
//			CountDeltas(width, height, xDelta, yDelta, countData);
//		}
//	}
//
//	myTextOutlineOffset.xDelta = xDelta;
//	myTextOutlineOffset.yDelta = yDelta;
//}
//
//int Color32Reverse2(int x)
//{
//	return ((x & 0xFF000000) >> 8) | ((x & 0x00FF0000) >> 8) | ((x & 0x0000FF00) << 8) | ((x & 0x000000FF) << 24);
//}
//
//int Color32Reverse(int x)
//{
//	int returner = x;
//	returner |= ((x & 0x000000FF) << 24);
//	returner |= ((x & 0x000000FF) << 16);
//	returner |= ((x & 0x000000FF) << 8);
//
//	return returner;
//}
//
//void GlyphLoader::LoadGlyph(int index, int& atlasX, int& atlasY, int& maxY, float atlasWidth, float atlasHeight, InternalTextAndFontData* aFontData, FT_FaceRec_* aFace, int aBorderOffset)
//{
//	FT_Error error = FT_Load_Char(aFace, index, FT_LOAD_RENDER);
//	if (error != 0)
//	{
//		return;
//	}
//
//	FT_GlyphSlot slot = aFace->glyph;
//	FT_Bitmap bitmap = slot->bitmap;
//
//	int height = bitmap.rows;
//	int width = bitmap.width;
//
//	CharData glyphData;
//	glyphData.myChar = static_cast<char>(index);
//	glyphData.myHeight = static_cast<short>(height + (aBorderOffset * 2));
//	glyphData.myWidth = static_cast<short>(width + (aBorderOffset * 2));
//
//	glyphData.mymyStartXV = { (float(atlasX) / atlasWidth), (float(atlasY) / atlasHeight) };
//	glyphData.myBottomRightUV = { (float(atlasX + glyphData.myWidth) / atlasWidth), (float(atlasY + glyphData.myHeight) / atlasHeight) };
//
//	glyphData.myAdvanceX = static_cast<short>(slot->advance.x) >> 6;
//	glyphData.myAdvanceY = static_cast<short>(slot->advance.y) >> 6;
//
//	glyphData.myBearingX = (short)slot->bitmap_left;
//	glyphData.myBearingY = (short)slot->bitmap_top;
//	if (glyphData.mymyStartXV.x > 1 || glyphData.mymyStartXV.y > 1 || glyphData.myBottomRightUV.x > 1 || glyphData.myBottomRightUV.y > 1)
//	{
//		return;
//	}
//
//	CalculateGlyphOffsets(index, slot);
//	for (int x = 0; x < width; x++)
//	{
//		for (int y = 0; y < height; y++)
//		{
//			if (x < 0 || y < 0)
//			{
//				continue;
//			}
//			int& saved = aFontData->myAtlas[((atlasY)+aBorderOffset + myTextOutlineOffset.yDelta + y) *
//				int(atlasWidth) + ((atlasX + aBorderOffset + myTextOutlineOffset.xDelta) + x)];
//			saved |= bitmap.buffer[y * bitmap.width + x];
//
//			saved = Color32Reverse(saved);
//
//			if (y + (atlasY + Y_OFFSET) > maxY)
//			{
//				maxY = y + (atlasY + Y_OFFSET);
//			}
//		}
//	}
//
//	atlasX = atlasX + width + X_OFFSET;
//	aFontData->myCharData[index] = glyphData;
//}
//
//void GlyphLoader::LoadOutline(const int index, const int atlasX, const int atlasY, const float atlasWidth, InternalTextAndFontData* aFontData, FT_FaceRec_* aFace, int aBorderOffset)
//{
//	FT_Error err;
//	FT_Stroker stroker;
//	FT_Glyph glyph;
//
//	err = FT_Load_Char(aFace, index, FT_LOAD_NO_BITMAP);
//	if (err != 0)
//	{
//	}
//
//	err = FT_Get_Glyph(aFace->glyph, &glyph);
//	if (err != 0)
//	{
//	}
//
//	glyph->format = FT_GLYPH_FORMAT_OUTLINE;
//
//	err = FT_Stroker_New(myLibrary, &stroker);
//	if (err != 0)
//	{
//	}
//
//	FT_Stroker_Set(stroker, aBorderOffset * 64, FT_STROKER_LINECAP_ROUND, FT_STROKER_LINEJOIN_ROUND, 0);
//	err = FT_Glyph_StrokeBorder(&glyph, stroker, 0, 1);
//	if (err != 0)
//	{
//	}
//
//
//	err = FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_NORMAL, nullptr, true);
//	if (err != 0)
//	{
//	}
//
//	FT_BitmapGlyph bitmapGlyph = reinterpret_cast<FT_BitmapGlyph>(glyph);
//
//	unsigned int width = bitmapGlyph->bitmap.width;
//	unsigned int height = bitmapGlyph->bitmap.rows;
//
//	CalculateOutlineOffsets(index, aFace, aBorderOffset);
//	for (unsigned int x = 0; x < width; x++)
//	{
//		for (unsigned int y = 0; y < height; y++)
//		{
//
//			int& data = aFontData->myAtlas[((atlasY + myTextOutlineOffset.yDelta) + y) * int(atlasWidth) + ((atlasX + myTextOutlineOffset.xDelta) + x)];
//			data = 0;
//			data |= bitmapGlyph->bitmap.buffer[y * width + x];
//			data = Color32Reverse2(data);
//
//		}
//	}
//
//	FT_Stroker_Done(stroker);
//	FT_Done_Glyph(glyph);
//}
//
//TextToRender processNextCharacter(const InternalTextAndFontData& fontData, char aCharacter, float aSize, float& x, float& y, float& maxY)
//{
//	int charAsNumber = (unsigned char)aCharacter;
//	const CharData& charData = fontData.myCharData[charAsNumber];
//
//	if (maxY < charData.myHeight)
//	{
//		maxY = static_cast<float>(charData.myHeight);
//	}
//
//	TextToRender result = {};
//
//	result.myPosition = Vector2f((x + charData.myBearingX) * aSize, (y + charData.myBearingY) * aSize);
//	result.mySize = Vector2f(static_cast<float>(charData.myWidth), static_cast<float>(charData.myHeight));
//
//	result.myUvStart = charData.mymyStartXV;
//	result.myUvEnd = charData.myBottomRightUV;
//
//	if (aCharacter == '\n')
//	{
//		x = 0;
//		y -= (maxY + 6);
//	}
//	else
//	{
//		x += charData.myAdvanceX;
//	}
//
//	return result;
//}
//
//InternalTextAndFontData::~InternalTextAndFontData()
//{}
//
//TextService::TextService()
//{
//	myLibrary = nullptr;
//}
//
//void TextService::Init()
//{
//	FT_Init_FreeType(&myLibrary);
//}
//
//TextService::~TextService()
//{
//	FT_Done_FreeType(myLibrary);
//}
//
//inline bool FileExists(const std::string& name)
//{
//	std::ifstream f(name.c_str());
//	if (f.good())
//	{
//		f.close();
//		return true;
//	}
//
//	f.close();
//	return false;
//}
//
//Font TextService::GetOrLoad(std::wstring aFontPathAndName, FontSize aFontSize, unsigned char aBorderSize)
//{
//
//	auto Context = GraphicsEngine::GetInstance()->GetContext();
//	auto  Device = GraphicsEngine::GetInstance()->GetDevice();
//	if (aFontSize == -1)
//	{
//		return { nullptr };
//	}
//
//	short fontWidth = (short)aFontSize;
//	const int atlasSize = 1024;
//	float atlasWidth = static_cast<float>(atlasSize);
//	float atlasHeight = static_cast<float>(atlasSize);
//
//	int atlasX = 2;
//	int atlasY = 2;
//	int currentMaxY = 0;
//
//	std::shared_ptr<InternalTextAndFontData> fontData = nullptr;
//
//	std::wstringstream key;
//	key << aFontPathAndName << "-" << fontWidth << "-" << aBorderSize;
//
//	auto it = myFontData.find(key.str());
//	if (it != myFontData.end())
//	{
//		fontData = it->second.lock();
//		if (fontData)
//			return { fontData };
//	}
//
//	fontData = std::make_shared<InternalTextAndFontData>();
//	fontData->myTexture = std::make_unique<Texture>();
//
//	myFontData[key.str()] = fontData;
//
//	fontData->myAtlas.resize(atlasSize * atlasSize);
//	ZeroMemory(fontData->myAtlas.data(), (atlasSize * atlasSize) * sizeof(int));
//	fontData->myFontHeightWidth = fontWidth;
//	fontData->myName = key.str();
//
//	FT_Face face;
//	const int sLength = static_cast<int>(aFontPathAndName.length()) + 1;
//	const int len = WideCharToMultiByte(CP_ACP, 0, aFontPathAndName.c_str(), sLength, 0, 0, 0, 0);
//	std::string result(len, L'\0');
//	WideCharToMultiByte(CP_ACP, 0, aFontPathAndName.c_str(), sLength, &result[0], len, 0, 0);
//
//	std::string aPathAndName = STATIC_ASSET_PATH + result;
//
//	std::string aAsciiFontPath = result;
//	FT_Error error = FT_New_Face(myLibrary, aPathAndName.c_str(), 0, &face);
//	if (error != 0)
//	{
//		return { nullptr };
//	}
//
//	FT_F26Dot6 ftSize = (FT_F26Dot6)(fontData->myFontHeightWidth * (1 << 6));
//	error = FT_Set_Char_Size(face, ftSize, 0, 100, 100);
//	if (error != 0)
//	{
//		std::cout << "FT_Set_Char_Size error" << std::endl;
//		return { nullptr };
//	}
//
//	error = FT_Load_Char(face, 'x', FT_LOAD_DEFAULT);
//	if (error != 0)
//	{
//		std::cout << "Failed to load glyph" << std::endl;
//		return { nullptr };
//	}
//
//	FT_GlyphSlot space = face->glyph;
//
//	fontData->myWordSpacing = static_cast<short>(space->metrics.width / 256);
//
//	int currentMax = 256;
//	int currentI = 32;
//
//	GlyphLoader glyphLoader;
//	glyphLoader.myLibrary = myLibrary;
//	for (int i = currentI; i < currentMax; i++)
//	{
//		error = FT_Load_Char(face, i, FT_LOAD_RENDER);
//		if (error != 0)
//		{
//			std::cout << "Failed to load glyph " << (char)i << std::endl;
//			continue;
//		}
//
//		FT_GlyphSlot slot = face->glyph;
//
//		if (atlasX + slot->bitmap.width + (aBorderSize * 2) > atlasWidth)
//		{
//			atlasX = X_START;
//			atlasY = currentMaxY;
//		}
//		if (aBorderSize > 0)
//		{
//			glyphLoader.LoadOutline(i, atlasX, atlasY, atlasWidth, fontData.get(), face, aBorderSize);
//		}
//
//		glyphLoader.LoadGlyph(i, atlasX, atlasY, currentMaxY, atlasWidth, atlasHeight, fontData.get(), face, aBorderSize);
//	}
//
//
//	D3D11_SUBRESOURCE_DATA data;
//	data.pSysMem = fontData->myAtlas.data();
//	data.SysMemPitch = atlasSize * 4;
//
//	D3D11_TEXTURE2D_DESC info;
//	info.Width = atlasSize;
//	info.Height = atlasSize;
//	info.MipLevels = 0;
//	info.ArraySize = 1;
//	info.SampleDesc.Count = 1;
//	info.SampleDesc.Quality = 0;
//	info.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
//	info.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
//	info.Usage = D3D11_USAGE_DEFAULT;
//	info.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
//	info.CPUAccessFlags = 0;
//
//	ID3D11Texture2D* texture;
//
//	HRESULT hr = Device->CreateTexture2D(&info, nullptr, &texture);
//	if (FAILED(hr))
//	{
//		return { nullptr };
//	}
//	Context->UpdateSubresource(texture, 0, NULL, fontData->myAtlas.data(), atlasSize * 4, 0);
//
//	hr = Device->CreateShaderResourceView(texture, nullptr, fontData->myAtlasView.ReleaseAndGetAddressOf());
//	if (FAILED(hr))
//	{
//		return { nullptr };
//	}
//	Context->GenerateMips(fontData->myAtlasView.Get());
//
//	fontData->myTexture.get()->SetShaderResourceView(fontData->myAtlasView.Get());
//	fontData->myTexture.get()->SetTexture(*texture);
//
//	fontData->myAtlasHeight = atlasSize;
//	fontData->myAtlasWidth = atlasSize;
//	fontData->myLineSpacing = static_cast<float>((face->ascender - face->descender) >> 6);
//
//	FT_Done_Face(face);
//
//	fontData->myTexture = std::make_unique<Texture>(fontData->myAtlasView.Get());
//	fontData->myTexture->SetTexture(*texture);
//	texture->Release();
//	return { fontData };
//}
//
//float TextService::GetSentenceWidth(Text& aText)
//{
//	if (aText.myText.size() <= 0)
//	{
//		return 0.0f;
//	}
//
//	const InternalTextAndFontData* fontData = aText.myFont.myData.get();
//	if (!fontData)
//	{
//		return 0.0f;
//	}
//
//	int count = static_cast<int>(aText.myText.length());
//
//	float maxX = FLT_MIN;
//
//	float drawX = 0.f;
//	float drawY = 0.f;
//	float maxDrawY = 0.f;
//
//	for (int i = 0; i < count; i++)
//	{
//		TextToRender charInfo = processNextCharacter(*fontData, aText.myText[i], aText.myScale, drawX, drawY, maxDrawY);
//		maxX = std::max(maxX, charInfo.myPosition.x + charInfo.mySize.x);
//	}
//
//	return maxX;
//}
//
//float TextService::GetSentenceHeight(Text& aText)
//{
//	if (aText.myText.size() <= 0)
//	{
//		return 0.0f;
//	}
//
//	const InternalTextAndFontData* fontData = aText.myFont.myData.get();
//	if (!fontData)
//	{
//		return 0.0f;
//	}
//
//	int count = static_cast<int>(aText.myText.length());
//
//	float maxY = FLT_MIN;
//
//	float drawX = 0.f;
//	float drawY = 0.f;
//	float maxDrawY = 0.f;
//
//	for (int i = 0; i < count; i++)
//	{
//		TextToRender charInfo = processNextCharacter(*fontData, aText.myText[i], aText.myScale, drawX, drawY, maxDrawY);
//		maxY = std::max(maxY, charInfo.myPosition.y + charInfo.mySize.y);
//	}
//
//	return maxY;
//}
//
//bool TextService::Draw(Text& aText, Shader* aCustomShader)
//{
//	const InternalTextAndFontData* fontData = aText.myFont.myData.get();
//	if (!fontData || !fontData->myTexture)
//	{
//		return false;
//	}
//	aCustomShader;
//	int count = (int)aText.myText.size();
//	const char* str = aText.myText.c_str();
//	float scale = aText.myScale;
//	float rotation = aText.myRotation;
//
//	float midX = 0;
//	float midY = 0;
//	if (rotation != 0.0f)
//	{
//		float minX = FLT_MAX;
//		float minY = FLT_MAX;
//		float maxX = FLT_MIN;
//		float maxY = FLT_MIN;
//
//		float drawX = 0.f;
//		float drawY = 0.f;
//		float maxDrawY = 0.f;
//
//		for (int i = 0; i < count; i++)
//		{
//			TextToRender charInfo = processNextCharacter(*fontData, str[i], scale, drawX, drawY, maxDrawY);
//
//			minX = min(minX, charInfo.myPosition.x);
//			minY = min(minY, charInfo.myPosition.y);
//			maxX = std::max(maxX, charInfo.myPosition.x + charInfo.mySize.x);
//			maxY = std::max(maxY, charInfo.myPosition.y + charInfo.mySize.y);
//		}
//
//		midX = 0.5f * (maxX + minX);
//		midY = 0.5f * (maxY + minY);
//	}
//
//	float drawX = 0.f;
//	float drawY = 0.f;
//	float maxDrawY = 0.f;
//
//	float diffX = midX;
//	float diffY = midY;
//
//	float c = 0.f, s = 0.f;
//	if (rotation != 0.0f)
//	{
//		c = cos(rotation);
//		s = sin(rotation);
//	}
//	TextMesh textMeshTest;
//	textMeshTest.SetTexture(fontData->myTexture.get());
//	textMeshTest.GetTexture().SetShaderResourceView(fontData->myAtlasView.Get());
//
//	for (size_t i = 0; i < aText.myText.size(); i++)
//	{
//		TextToRender charInfo = processNextCharacter(*fontData, str[i], scale, drawX, drawY, maxDrawY);
//
//		if (rotation != 0.0f)
//		{
//			float x = charInfo.myPosition.x - diffX;
//			float y = charInfo.myPosition.y - diffY;
//
//			charInfo.myPosition.x = x * c - y * s + diffX;
//			charInfo.myPosition.y = (x * s + y * c) + diffY;
//		}
//
//		Vector2f test21 =
//		{
//		aText.myPosition.x + charInfo.myPosition.x + drawX,
//		aText.myPosition.y - charInfo.myPosition.y + charInfo.mySize.y / 2
//		};
//
//		textMeshTest.AddCharacter(test21.x, test21.y, charInfo.mySize.x, charInfo.mySize.y, charInfo.myUvStart.x, charInfo.myUvEnd.x, charInfo.myUvStart.y, charInfo.myUvEnd.y);
//	}
//
//	D3D11_BLEND_DESC blendDesc;
//	ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));
//	D3D11_SAMPLER_DESC samplerDesc = {};
//	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
//	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
//	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
//	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
//	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
//	samplerDesc.MinLOD = 0;
//	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
//
//	blendDesc.RenderTarget[0].BlendEnable = true;
//	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
//	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
//	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
//	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
//	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
//	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
//	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
//
//	blendDesc.AlphaToCoverageEnable = false;
//	blendDesc.IndependentBlendEnable = false;
//
//	ID3D11BlendState* pBlendState = nullptr;
//	GraphicsEngine::GetInstance()->GetDevice()->CreateBlendState(&blendDesc, &pBlendState);
//	GraphicsEngine::GetInstance()->GetContext()->OMSetBlendState(pBlendState, nullptr, 0xFFFFFFFF);
//	textMeshTest.GenerateMesh(GraphicsEngine::GetInstance()->GetDevice().Get(), GraphicsEngine::GetInstance()->GetContext(), aText.GetColor());
//
//	return true;
//}

bool TextService::Init()
{
	spriteBatch = std::make_unique<DirectX::SpriteBatch>(GraphicsEngine::GetInstance()->GetContext());
	myScaleSize = GraphicsEngine::GetInstance()->GetViewportDimensions();
	return true;
}

void TextService::RenderText(Text& aTextToRender)
{
	spriteBatch->Begin();
	//do stuff with position and size here
	Vector2f res = GraphicsEngine::GetInstance()->GetViewportDimensions();
	DirectX::XMFLOAT2 tempPos = { (aTextToRender.GetPositionAsXMFLOAT2().x / myScaleSize.x) * res.x,
		(aTextToRender.GetPositionAsXMFLOAT2().y / myScaleSize.y) * res.y };
	DirectX::XMFLOAT2 tempScale = { (aTextToRender.GetScaleAsXMFLOAT2().x / myScaleSize.x) * res.x,(aTextToRender.GetScaleAsXMFLOAT2().y / myScaleSize.y) * res.y };
	aTextToRender.GetFont().DrawString(spriteBatch.get(), aTextToRender.GetText().c_str(), tempPos, aTextToRender.GetFXMVECTORColor(), aTextToRender.GetRotationInRAD(), aTextToRender.GetOriginAsXMFLOAT2(), tempScale, DirectX::SpriteEffects_None, 0);
	spriteBatch->End();
}
