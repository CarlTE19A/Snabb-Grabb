#pragma once
#include <DirectXTK\SpriteBatch.h>
#include <wrl/client.h>
#include <memory>

using Microsoft::WRL::ComPtr;
struct ID3D11DeviceContext;
struct ID3D11Buffer;

class Sprite;

enum class ShaderType 
{
	eDefault,
	eMorph, 
	eGlitch,
	eDisintegrate,
	eCount
};

class SpriteDrawer
{
public:
	SpriteDrawer();
	~SpriteDrawer();
	
	bool Init(ComPtr<ID3D11DeviceContext> aContext);

	void DrawSprite(Sprite& aSprite);
	void DrawSprite(Sprite& aSprite, ShaderType aShadertype);

private:
	ID3D11VertexShader* myVertexShader;
	ID3D11PixelShader* myPixelShader;
	ID3D11PixelShader* myPixelDisintegrateShader;

	std::unique_ptr<DirectX::SpriteBatch> mySpriteBatch; 
	ComPtr<ID3D11Buffer> myMorphBuffer;

	Vector2f myScaleSize;
};

