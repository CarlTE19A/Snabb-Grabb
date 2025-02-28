#pragma once
#include <DirectXTK\SpriteFont.h>
#include <wrl/client.h>
//#include <memory>

using Microsoft::WRL::ComPtr;
struct ID3D11DeviceContext;
struct ID3D11Buffer;

enum FontSize
{
	FontSize_6 = 6,
	FontSize_8 = 8,
	FontSize_9 = 9,
	FontSize_10 = 10,
	FontSize_11 = 11,
	FontSize_12 = 12,
	FontSize_14 = 14,
	FontSize_18 = 18,
	FontSize_24 = 24,
	FontSize_30 = 30,
	FontSize_36 = 36,
	FontSize_48 = 48,
	FontSize_60 = 60,
	FontSize_72 = 72,
	FontSize_Count
};

class Text
{
public:

	Text();
	~Text();

	bool Init(FontSize aFontSize);

	void Draw();

	void SetText(std::string aText) { myText = aText; }
	void AddText(std::string aText) { myText += aText; }
	std::string& GetText() { return myText; }

	void SetColor(const Vector4f aColor) { myColor = aColor; };
	Vector4f GetColor() const { return myColor; }
	DirectX::FXMVECTOR GetFXMVECTORColor() const { return DirectX::FXMVECTOR{ myColor.x, myColor.y, myColor.z, myColor.w }; }

	void SetPosition(const Vector2f aPosition) { myPosition = aPosition; }
	Vector2f GetPosition() const { return myPosition; }
	DirectX::XMFLOAT2 GetPositionAsXMFLOAT2() { return DirectX::XMFLOAT2{ myPosition.x, myPosition.y }; }

	void SetFontSize(FontSize aFontSize);
	FontSize& GetFontSize();

	void SetOrogin(Vector2f aOrigin) { myOrigin = aOrigin; }
	Vector2f GetOrigin() const { return myOrigin; }
	DirectX::XMFLOAT2 GetOriginAsXMFLOAT2() { return DirectX::XMFLOAT2{ myOrigin.x, myOrigin.y }; }

	void SetOriginInMiddle();

	void SetScale(Vector2f aScale) { myScale = aScale; }
	Vector2f GetScale() const { return myScale; }
	DirectX::XMFLOAT2 GetScaleAsXMFLOAT2() { return DirectX::XMFLOAT2{ myScale.x, myScale.y }; }

	void SetRotation(float aRotation) { myRotationInRad = aRotation * DirectX::XM_PI / 180.0f;    myRotation = aRotation; }
	float& GetRotation() { return myRotation; }
	float& GetRotationInRAD() { return myRotationInRad; }

	DirectX::SpriteFont& GetFont() { return *myFont; }

private:
	std::unique_ptr<DirectX::SpriteFont> myFont;

	FontSize myFontSize;
	std::wstring myFontname;
	Vector2f myPosition;
	Vector2f myScale;
	Vector4f myColor;
	Vector2f myOrigin;
	std::string myText;
	float myRotation;
	float myRotationInRad;
};

//
//class InternalTextAndFontData;
//struct Font
//{
//	std::shared_ptr<const InternalTextAndFontData> myData;
//};
//
//class Text
//{
//	friend class TextService;
//public:
//	Text(const Font& aFont);
//
//	Text(const wchar_t* aPathAndName = L"Text/arial.ttf", FontSize aFontSize = FontSize_18, unsigned char aBorderSize = 0);
//	~Text();
//	void Render();
//
//	float GetWidth();
//	float GetHeight();
//
//	void SetColor(const Vector4f& aColor);
//	Vector4f GetColor() const;
//
//	void SetText(const std::string& aText);
//	std::string GetText() const;
//
//	void SetPosition(const Vector2f& aPosition);
//	Vector2f GetPosition() const;
//
//	void SetFontSize(FontSize aFontSize);
//	FontSize GetFontSize();
//
//	void SetScale(float aScale);
//	float GetScale() const;
//
//	void SetRotation(float aRotation) { myRotation = aRotation; }
//	float GetRotation() const { return myRotation; }
//
//protected:
//	TextService* myTextService;
//	Font myFont;
//	const wchar_t* myFontPath;
//	FontSize myFontSize;
//	std::string myText;
//	Vector2f myPosition;
//	float myScale;
//	float myRotation;
//	Vector4f myColor;
//};