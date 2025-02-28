//#pragma once
//#include "UIElement.h"
//
//class UISprite;
//
//class UIClock : public UIElement
//{
//public:
//	UIClock() { myValue = 0; myCooldownTime = 0.f; myIsOnCooldown = false; }
//	~UIClock() override {}
//	bool Init(const std::string aTexturePath) override;
//	void Update() override;
//	void UpdateValue(float aValue);
//	void SetPosition(const Vector3f& aPosition) override;
//	void Draw() override;
//	void SetScale(const Vector2f& aScale) override;
//	void CropSprite(Vector2f aCropEnd, Vector2f aCropStart, bool setUV = true) override;
//	void StartCooldown() { Reset(); myIsOnCooldown = true; }
//	void Reset();
//	void SetCooldownTime(float aCooldownTime) { myCooldownTime = aCooldownTime; }
//private:
//	float myValue;
//	std::vector<UISprite*> mySprites;
//	std::vector<TextureRect> myTextureRects;
//	float myCooldownTime;
//	bool myIsOnCooldown;
//};
