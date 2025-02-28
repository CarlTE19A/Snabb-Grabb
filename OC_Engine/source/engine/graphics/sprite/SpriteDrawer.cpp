#include "stdafx.h"
#include "SpriteDrawer.h"
#include "Sprite.h"
#include "graphics\GraphicsEngine.h"

struct MorphBufferStruct
{
	Vector4f myMorphRadius;
	Vector4f myMorphSpeed;
	float myTimer;
	float padding[3];
};



SpriteDrawer::SpriteDrawer()
{
	mySpriteBatch = nullptr;
}

SpriteDrawer::~SpriteDrawer()
{
}

bool SpriteDrawer::Init(ComPtr<ID3D11DeviceContext> aContext)
{
	mySpriteBatch = std::make_unique<DirectX::SpriteBatch>(aContext.Get());
	if (mySpriteBatch == nullptr)
		return false;

	char* vsData = nullptr;
	int vsDataSize = 0;
	{ // Vertex Shader
		FILE* fileptr;
		fopen_s(&fileptr, STATIC_SHADER_PATH "SpriteVertexMorph_VS.cso", "rb");
		if (fileptr == 0)
			return false;

		fseek(fileptr, 0L, SEEK_END);
		vsDataSize = ftell(fileptr);
		fseek(fileptr, 0L, SEEK_SET);

		vsData = (char*)malloc(sizeof(char) * vsDataSize);
		if (vsData == 0)
		{
			puts("Failed to create buffer for loading model!");
			return false;
		}
		fread_s(vsData, vsDataSize, vsDataSize, 1, fileptr);
		fclose(fileptr);

		HRESULT result = GraphicsEngine::GetInstance()->GetDevice()->CreateVertexShader(vsData, vsDataSize, nullptr, &myVertexShader);
		if (FAILED(result))
			return false;
	}

	char* psData = nullptr;
	int psDataSize = 0;
	{ // Pixel Shader
		FILE* fileptr;
		fopen_s(&fileptr, STATIC_SHADER_PATH "SpritePixelGlitch_PS.cso", "rb");
		if (fileptr == 0)
			return false;

		fseek(fileptr, 0L, SEEK_END);
		psDataSize = ftell(fileptr);
		fseek(fileptr, 0L, SEEK_SET);

		psData = (char*)malloc(sizeof(char) * psDataSize);
		if (vsData == 0)
		{
			puts("Failed to create buffer for loading model!");
			return false;
		}
		fread_s(psData, psDataSize, psDataSize, 1, fileptr);
		fclose(fileptr);

		HRESULT result = GraphicsEngine::GetInstance()->GetDevice()->CreatePixelShader(psData, psDataSize, nullptr, &myPixelShader);
		if (FAILED(result))
			return false;

	}

	char* psData1 = nullptr;
	int psDataSize1 = 0;
	{ // Pixel Shader
		FILE* fileptr;
		fopen_s(&fileptr, STATIC_SHADER_PATH "SpritePixelDisintegrate_PS.cso", "rb");
		if (fileptr == 0)
			return false;

		fseek(fileptr, 0L, SEEK_END);
		psDataSize1 = ftell(fileptr);
		fseek(fileptr, 0L, SEEK_SET);

		psData1 = (char*)malloc(sizeof(char) * psDataSize1);
		if (vsData == 0)
		{
			puts("Failed to create buffer for loading model!");
			return false;
		}
		fread_s(psData1, psDataSize1, psDataSize1, 1, fileptr);
		fclose(fileptr);

		HRESULT result = GraphicsEngine::GetInstance()->GetDevice()->CreatePixelShader(psData1, psDataSize1, nullptr, &myPixelDisintegrateShader);
		if (FAILED(result))
			return false;

	}

	free(vsData);
	free(psData);
	free(psData1);


	{
		HRESULT result = S_OK;

		D3D11_BUFFER_DESC bufferDesc;
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.ByteWidth = sizeof(MorphBufferStruct);
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;

		result = GraphicsEngine::GetInstance()->GetDevice()->CreateBuffer(&bufferDesc, NULL, myMorphBuffer.ReleaseAndGetAddressOf());
		if (FAILED(result))
		{
			return false;
		}
	}
	myScaleSize = GraphicsEngine::GetInstance()->GetViewportDimensions();
	return true;
}

void SpriteDrawer::DrawSprite(Sprite& aSprite)
{
	Vector2f res = GraphicsEngine::GetInstance()->GetViewportDimensions();
	mySpriteBatch->Begin(DirectX::SpriteSortMode_Deferred, &GraphicsEngine::GetInstance()->GetBlendState(BlendState::AlphaBlend));
	//DirectX::XMFLOAT2 tempSpritePos = { aSprite.GetPosition().x,aSprite.GetPosition().y };
	DirectX::XMFLOAT2 tempSpritePos = { (aSprite.GetPosition().x / myScaleSize.x) * res.x,
		(aSprite.GetPosition().y / myScaleSize.y) * res.y };

	DirectX::FXMVECTOR tempColor = { aSprite.GetColor().x,aSprite.GetColor().y,aSprite.GetColor().z,aSprite.GetColor().w };

	DirectX::XMFLOAT2 tempOrigin = { static_cast<float>(aSprite.GetRect().right + aSprite.GetRect().left) / 2 - (float)aSprite.GetRect().left
		, static_cast<float>(aSprite.GetRect().bottom + aSprite.GetRect().top) / 2 - (float)aSprite.GetRect().top };

	//DirectX::XMFLOAT2 tempScale = { aSprite.GetSize().x,aSprite.GetSize().y };
	DirectX::XMFLOAT2 tempScale = { (aSprite.GetSize().x / myScaleSize.x) * res.x,(aSprite.GetSize().y / myScaleSize.y) * res.y };

	mySpriteBatch->Draw(aSprite.GetTexture().GetShaderResourceView(), tempSpritePos, &aSprite.GetRect(), tempColor, aSprite.GetRotation(), tempOrigin, tempScale, DirectX::SpriteEffects_None, 0.f);

	mySpriteBatch->End();
}

void SpriteDrawer::DrawSprite(Sprite& aSprite, ShaderType aShadertype)
{
	if (aShadertype == ShaderType::eDefault) { DrawSprite(aSprite); return; }

	auto* gfx = GraphicsEngine::GetInstance();

	mySpriteBatch->Begin(DirectX::SpriteSortMode_Deferred, &gfx->GetBlendState(BlendState::AlphaBlend), nullptr, nullptr, nullptr,
		[this, &aSprite, &aShadertype]() {
			auto* gfxContext = GraphicsEngine::GetInstance()->GetContext();
			switch (aShadertype)
			{
			case ShaderType::eMorph:
				gfxContext->VSSetShader(myVertexShader, nullptr, 0);
				break;
			case ShaderType::eGlitch:
				gfxContext->PSSetShader(myPixelShader, nullptr, 0);
				break;
			case ShaderType::eDisintegrate:
				gfxContext->PSSetShader(myPixelDisintegrateShader, nullptr, 0);
				break;
			default:
				break;
			}
		}
	);


	Vector2f res = gfx->GetViewportDimensions();
	//DirectX::XMFLOAT2 tempSpritePos = { aSprite.GetPosition().x,aSprite.GetPosition().y };
	DirectX::XMFLOAT2 tempSpritePos = { (aSprite.GetPosition().x / myScaleSize.x) * res.x,
		(aSprite.GetPosition().y / myScaleSize.y) * res.y };

	DirectX::FXMVECTOR tempColor = { aSprite.GetColor().x,aSprite.GetColor().y,aSprite.GetColor().z,aSprite.GetColor().w };

	DirectX::XMFLOAT2 tempOrigin = { static_cast<float>(aSprite.GetRect().right + aSprite.GetRect().left) / 2 - (float)aSprite.GetRect().left
		, static_cast<float>(aSprite.GetRect().bottom + aSprite.GetRect().top) / 2 - (float)aSprite.GetRect().top };

	//DirectX::XMFLOAT2 tempScale = { aSprite.GetSize().x,aSprite.GetSize().y };
	DirectX::XMFLOAT2 tempScale = { (aSprite.GetSize().x / myScaleSize.x) * res.x,(aSprite.GetSize().y / myScaleSize.y) * res.y };

	if (aShadertype == ShaderType::eMorph)
	{
		MorphBufferStruct buffer;
		buffer.myMorphRadius = aSprite.GetMorphRadius();
		buffer.myMorphSpeed = aSprite.GetMorphSpeed();

		D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
		auto* gfxContext = gfx->GetContext();
		gfxContext->Map(myMorphBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
		memcpy(mappedBuffer.pData, &buffer, sizeof(MorphBufferStruct));
		gfxContext->Unmap(myMorphBuffer.Get(), 0);
		gfxContext->VSSetConstantBuffers(11, 1, myMorphBuffer.GetAddressOf());
	}
	else if (aShadertype == ShaderType::eDisintegrate)
	{
		MorphBufferStruct buffer;
		buffer.myMorphRadius = Vector4f(0, 0, 0, 0);
		buffer.myMorphSpeed = Vector4f(0, 0, 0, 0);
		buffer.myTimer = aSprite.GetTimer();

		D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
		auto* gfxContext = gfx->GetContext();
		gfxContext->Map(myMorphBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
		memcpy(mappedBuffer.pData, &buffer, sizeof(MorphBufferStruct));
		gfxContext->Unmap(myMorphBuffer.Get(), 0);
		gfxContext->PSSetConstantBuffers(11, 1, myMorphBuffer.GetAddressOf());
	}

	mySpriteBatch->Draw(aSprite.GetTexture().GetShaderResourceView(), tempSpritePos, &aSprite.GetRect(), tempColor, aSprite.GetRotation(), tempOrigin, tempScale, DirectX::SpriteEffects_None, 0.f);

	mySpriteBatch->End();
}

