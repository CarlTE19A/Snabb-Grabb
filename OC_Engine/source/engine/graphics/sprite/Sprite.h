#pragma once
#include "graphics\Texture.h"
#include "math\Vector.h"

enum class ShaderType;

class Sprite
{
public:
	Sprite();
	~Sprite();

	bool Init(std::string aTexturePath);

	void Draw();
	void Draw(ShaderType aShaderType);

	void SetPosition(Vector2f& aPosition) { myPosition = aPosition; }
	Vector2f& GetPosition() { return myPosition; }
	const Vector2f& GetPosition()const { return myPosition; }

	void SetSize(Vector2f& aSize) { mySize = aSize; }
	Vector2f& GetSize() { return mySize; }
	const Vector2f& GetSize() const { return mySize; }

	void SetRect(RECT& aRect);
	RECT& GetRect() { return myRect; }
	const RECT& GetRect()const { return myRect; }

	void SetColor(Vector4f& aColor) { myColor = aColor; }
	Vector4f& GetColor() { return myColor; }
	const Vector4f& GetColor()const { return myColor; }

	void SetTexture(Texture& aTexture) { myTexture = &aTexture; }
	bool SetTexture(std::string aTexturePath);
	Texture& GetTexture() { return *myTexture; }
	const Texture& GetTexture() const { return *myTexture; }

	void SetRotation(float& aRotation) { myRotation = aRotation; }
	float& GetRotation() { return myRotation; }
	const float& GetRotation() const { return myRotation; }

	void SetMorphSpeed(Vector4f& aMorphSpeed) { myMorphSpeed = aMorphSpeed; }
	void SetMorphRadius(Vector4f& aMorphRadius) { myMorphRadius = aMorphRadius; }
	Vector4f& GetMorphSpeed() { return myMorphSpeed; }
	Vector4f& GetMorphRadius() { return myMorphRadius; }

	void SetTimer(float aTimer) { myTimer = aTimer; }
	void DecreaseTimer(float aDelta) { myTimer -= aDelta; }
	float GetTimer() { return myTimer; }
private:
	Vector2f myPosition;
	Vector2f mySize;
	RECT myRect;
	Texture* myTexture;
	Vector4f myColor;
	float myRotation;
	Vector4f myMorphSpeed;
	Vector4f myMorphRadius;
	float myTimer;
};
