#include "Sprite.h"
#include "stdafx.h"
#include "Sprite.h"
#include "graphics\GraphicsEngine.h"
#include "SpriteDrawer.h"

Sprite::Sprite()
{
	myTexture = new Texture();
	if (!myTexture->Initialize(TextureInitializationType::DDS, "TestAssets/SpriteDefault.dds", true))
	{
		std::cout << "Can't load default texture for sprite (It's okey if Init is called and returns true" << std::endl;
	}
	myPosition = { 0,0 };
	mySize = { 1,1 };
	myRect = { 0,0, myTexture->GetWidth() + myRect.left,myTexture->GetHeight() + myRect.top };
	myColor = { 1,1,1,1 };
	myRotation = 0;
}

Sprite::~Sprite()
{
	delete myTexture;
	myTexture = nullptr;
}

bool Sprite::Init(std::string aTexturePath)
{
	if (!myTexture->Initialize(TextureInitializationType::DDS, aTexturePath, true))
	{
		if (!myTexture->Initialize(TextureInitializationType::PNG, aTexturePath, true))
		{
			std::cout << "Can't load texture" << std::endl;
			return false;
		}
	}
	myRect = { 0,0, myTexture->GetWidth() + myRect.left,myTexture->GetHeight() + myRect.top };
	Vector2<int> tempGraphics = GraphicsEngine::GetInstance()->GetWindowSize();
	myPosition = { (float)tempGraphics.x / 2,(float)tempGraphics.y / 2 };
	mySize = { 1,1 };
	myColor = { 1,1,1,1 };

	return true;
}

void Sprite::Draw()
{
	GraphicsEngine::GetInstance()->GetSpriteDrawer().DrawSprite(*this);
}

void Sprite::Draw(ShaderType aShaderType)
{
	GraphicsEngine::GetInstance()->GetSpriteDrawer().DrawSprite(*this, aShaderType);
}



void Sprite::SetRect(RECT& aRect)
{
	myRect = aRect;
}

bool Sprite::SetTexture(std::string aTexturePath)
{
	if (!myTexture->Initialize(TextureInitializationType::DDS, aTexturePath, true))
	{
		if (!myTexture->Initialize(TextureInitializationType::PNG, aTexturePath, true))
		{
			std::cout << "Can't load texture" << std::endl;
			return false;
		}
	}
	myRect = { 0,0, myTexture->GetWidth() + myRect.left,myTexture->GetHeight() + myRect.top };
	return true;
}















//
//#include "../GraphicsEngine.h"
//#include "component/CameraComponent.h"
//#include "graphics\sprite\SpriteDrawer.h"
//
//Sprite::Sprite()
//{
//	myVertexBuffer = nullptr;
//	myIndexBuffer = nullptr;
//
//	myAspectRatio = 0;
//	myModelToWorld = {};
//
//	myTexture = nullptr;
//
//	myPos = {};
//	myScale = {};
//	myRotation = 0;
//	myUV = Vector2f(0.0f, 0.0f);
//	myAlpha = 1.f;
//	myColor = { 1,1,1,1 };
//
//	myScreenWidth = (float)GraphicsEngine::GetInstance()->GetWindowSize().x;
//	myScreenHeight = (float)GraphicsEngine::GetInstance()->GetWindowSize().y;
//}
//
//Sprite::~Sprite()
//{
//	if (myVertexBuffer != nullptr)
//	{
//		myVertexBuffer->Release();
//		myVertexBuffer = nullptr;
//	}
//
//	if (myIndexBuffer != nullptr)
//	{
//		myIndexBuffer->Release();
//		myIndexBuffer = nullptr;
//	}
//
//	myVertices.clear();
//
//	myIndices.clear();
//}
//
//void Sprite::Init()
//{
//}
//
//void Sprite::Render()
//{
//}
//
//bool Sprite::Init(std::string aTexturePath, Vector3f aPosition, const char* aVertexShaderPath, const char* aPixelShaderPath)
//{
//	myColor = { 1,1,1,1 };
//	aPixelShaderPath;
//	aVertexShaderPath;
//	myIndices = CreateSquareIndices();
//	myVertices = CreateSquareVertices();
//	myTextureRect = { 0.0f,0.0f,1.f,1.f };
//
//	if (!myTexture.Initialize(TextureInitializationType::DDS, aTexturePath))
//	{
//		if (!myTexture.Initialize(TextureInitializationType::PNG, aTexturePath))
//		{
//			return false;
//		}
//	}
//
//	D3D11_BUFFER_DESC vertexBufferDesc;
//	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
//	vertexBufferDesc.ByteWidth = sizeof(Sprite::Vertex) * (int)myVertices.size();
//	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
//	vertexBufferDesc.MiscFlags = 0;
//	vertexBufferDesc.StructureByteStride = 0;
//
//	D3D11_SUBRESOURCE_DATA vertexData;
//	vertexData.pSysMem = &myVertices[0];
//	vertexData.SysMemPitch = 0;
//	vertexData.SysMemSlicePitch = 0;
//
//	HRESULT result = GraphicsEngine::GetInstance()->GetDevice()->CreateBuffer(&vertexBufferDesc, &vertexData, &myVertexBuffer);
//
//	if (FAILED(result))
//	{
//		return false;
//	}
//
//	D3D11_BUFFER_DESC indexBufferDesc;
//	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
//	indexBufferDesc.ByteWidth = sizeof(unsigned int) * (int)myIndices.size();
//	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
//	indexBufferDesc.CPUAccessFlags = 0;
//	indexBufferDesc.MiscFlags = 0;
//	indexBufferDesc.StructureByteStride = 0;
//
//	D3D11_SUBRESOURCE_DATA indexData;
//	indexData.pSysMem = &myIndices[0];
//	indexData.SysMemPitch = 0;
//	indexData.SysMemSlicePitch = 0;
//
//	result = GraphicsEngine::GetInstance()->GetDevice()->CreateBuffer(&indexBufferDesc, &indexData, &myIndexBuffer);
//
//	if (FAILED(result))
//	{
//		return false;
//	}
//
//	myPos = aPosition;
//
//	myAspectRatio = myScreenWidth / myScreenHeight;
//
//	myTextureSize = { (float)myTexture.GetWidth(), (float)myTexture.GetHeight() };
//	SetScale({ 1,1 });
//	myScalingFactor = (1 / myPos.z);
//
//	SetPosition(myPos);
//
//	if (!myCustomShader.Init(aVertexShaderPath, aPixelShaderPath, GetInstanceInputLayout()))
//	{
//		return false;
//	}
//	return true;
//}
//
//DirectX::XMMATRIX& Sprite::GetTransform()
//{
//	return myModelToWorld;
//}
//
//const DirectX::XMMATRIX& Sprite::GetTransform() const
//{
//	return myModelToWorld;
//}
//
//void Sprite::Update()
//{}
//
//void Sprite::Draw()
//{
//	GraphicsEngine::GetInstance()->GetSpriteDrawer().DrawSprite(*this);
//}
//
//void Sprite::SetPosition(const Vector3f& aVector)
//{
//	myPos = aVector;
//
//	float ndcX = (2.0f * (myPos.x / (float)GraphicsEngine::GetInstance()->GetWindowSize().x)) - 1.0f;
//	float ndcY = 1.0f - (2.0f * (myPos.y / (float)GraphicsEngine::GetInstance()->GetWindowSize().y));
//
//	ndcX *= myAspectRatio;
//
//	ndcX, ndcY;
//	myModelToWorld.r[3].m128_f32[0] = ndcX;
//	myModelToWorld.r[3].m128_f32[1] = ndcY;
//	myModelToWorld.r[3].m128_f32[2] = 0.f;
//}
//
//void Sprite::SetRotation(const float& aRotaion)
//{
//	myRotation = aRotaion;
//
//	float angleInDegreesZ = myRotation;
//
//	float angleInRadiansZ = angleInDegreesZ * (3.14159265359f / 180.0f);
//
//	DirectX::XMMATRIX scal = DirectX::XMMatrixScaling(myScale.x, (myScale.y), 0);
//	Vector3f pivot = Vector3f(0.5f, 0.5f, 0.5f);
//	DirectX::XMMATRIX translation1 = DirectX::XMMatrixTranslation(-pivot.x, -pivot.y, -pivot.z);
//	DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationZ(angleInRadiansZ);
//
//
//	myModelToWorld = translation1 * rotationMatrix * scal;
//
//	SetPosition(myPos);
//}
//
//void Sprite::SetScale(const Vector2f& aScale)
//{
//	auto graphicsEngine = GraphicsEngine::GetInstance();
//	myScale.x = aScale.x * (myTextureSize.x / (float)graphicsEngine->GetWindowSize().x) * myAspectRatio;
//	myScale.y = aScale.y * (myTextureSize.y / (float)graphicsEngine->GetWindowSize().y);
//
//	float angleInDegrees = myRotation;
//	float angleInRadians = angleInDegrees * (3.14159265359f / 180.0f);
//
//	DirectX::XMMATRIX scal = DirectX::XMMatrixScaling(myScale.x, (myScale.y) /** myScalingFactor*/, 1.f);
//	Vector3f pivot = Vector3f(0.5f, 0.5f, 0.5f);
//	DirectX::XMMATRIX translation1 = DirectX::XMMatrixTranslation(-pivot.x, -pivot.y, -pivot.z);
//	DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationZ(angleInRadians);
//
//	myModelToWorld = translation1 * rotationMatrix * scal;
//
//	SetPosition(myPos);
//}
//
//void Sprite::SetAlphaValue(float aValue)
//{
//	myAlpha = aValue;
//}
//
//ID3D11Buffer* Sprite::GetVertexBuffer()
//{
//	return myVertexBuffer;
//}
//
//ID3D11Buffer* Sprite::GetIndexBuffer()
//{
//	return myIndexBuffer;
//}
//
//int Sprite::GetIndexCount()
//{
//	return (int)myIndices.size();
//}
//
//std::vector<D3D11_INPUT_ELEMENT_DESC> Sprite::GetInstanceInputLayout()
//{
//	std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayout =
//	{
//		{"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
//		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
//		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
//		{"VALUE", 0, DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
//
//	{"INSTANCE_MODEL_TO_WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1},
//	{ "INSTANCE_MODEL_TO_WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
//	{ "INSTANCE_MODEL_TO_WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
//	{ "INSTANCE_MODEL_TO_WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
//	{ "INSTANCE_COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
//	};
//
//	return inputLayout;
//}
//
//void Sprite::SetTexture(Texture* aTexture)
//{
//	myTexture = *aTexture;
//}
//
//void Sprite::SetShader(Shader* aFUCKINGSHADER)
//{
//	myCustomShader = *aFUCKINGSHADER;
//}
//
//void Sprite::SetSRV()
//{}
//
//void Sprite::SetCharacterUV(bool setUV)
//{
//	ID3D11DeviceContext* deviceContext = GraphicsEngine::GetInstance()->GetContext();
//
//	D3D11_MAPPED_SUBRESOURCE mappedResource;
//	HRESULT result = deviceContext->Map(myVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
//
//	if (SUCCEEDED(result))
//	{
//		char* dataVPtr = (char*)mappedResource.pData;
//		memcpy(dataVPtr, myVertices.data(), sizeof(Sprite::Vertex) * (int)myVertices.size());
//
//		Vertex* vertices = reinterpret_cast<Vertex*>(mappedResource.pData);
//
//		if (setUV)
//		{
//			vertices[0].uv = { myTextureRect.myStartX, myTextureRect.myEndY };
//			vertices[1].uv = { myTextureRect.myEndX, myTextureRect.myEndY };
//			vertices[2].uv = { myTextureRect.myEndX, myTextureRect.myStartY };
//			vertices[3].uv = { myTextureRect.myStartX, myTextureRect.myStartY };
//
//			auto testendX = myTextureRect.myEndX;
//			auto testendY = myTextureRect.myEndY;
//
//			auto teststartX = myTextureRect.myStartX;
//			auto teststartY = myTextureRect.myStartY;
//
//			if (teststartX != 0)
//			{
//				vertices[0].position.x = 1.0f * (teststartX - 0.5f) * 2;
//				vertices[3].position.x = 1.0f * (teststartX - 0.5f) * 2;
//			}
//			if (teststartY != 0)
//			{
//				vertices[3].position.y = -1.0f * (teststartY - 0.5f) * 2;
//				vertices[2].position.y = -1.0f * (teststartY - 0.5f) * 2;
//			}
//			if (testendX != 1)
//			{
//				vertices[1].position.x = 1.0f * (testendX - 0.5f) * 2;
//				vertices[2].position.x = 1.0f * (testendX - 0.5f) * 2;
//			}
//			if (testendY != 1)
//			{
//				vertices[0].position.y = -1.0f * (testendY - 0.5f) * 2;
//				vertices[1].position.y = -1.0f * (testendY - 0.5f) * 2;
//			}
//		}
//		else
//		{
//			vertices[0].value = myTextureRect.myStartY;
//			vertices[1].value = myTextureRect.myStartY;
//			vertices[2].value = myTextureRect.myStartY;
//			vertices[3].value = myTextureRect.myStartY;
//		}
//		deviceContext->Unmap(myVertexBuffer, 0);
//	}
//}
//
//void Sprite::SetCharacterUVClock(bool aUpper)
//{
//	ID3D11DeviceContext* deviceContext = GraphicsEngine::GetInstance()->GetContext();
//
//	D3D11_MAPPED_SUBRESOURCE mappedResource;
//	HRESULT result = deviceContext->Map(myVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
//
//	if (SUCCEEDED(result))
//	{
//		char* dataVPtr = (char*)mappedResource.pData;
//		memcpy(dataVPtr, myVertices.data(), sizeof(Sprite::Vertex) * (int)myVertices.size());
//
//		Vertex* vertices = reinterpret_cast<Vertex*>(mappedResource.pData);
//
//		vertices[0].uv = { 0, 1 };
//		vertices[1].uv = { 1, 1 };
//		vertices[2].uv = { 1, 0 };
//		vertices[3].uv = { myTextureRect.myStartX, 0 };
//
//		auto testendX = myTextureRect.myEndX;
//		auto testendY = myTextureRect.myEndY;
//
//		auto teststartX = myTextureRect.myStartX;
//		auto teststartY = myTextureRect.myStartY;
//
//		if (teststartX != 0)
//		{
//			if (!aUpper)
//			{
//				vertices[0].position.x = -1.0f * (teststartX - 0.5f) * 2;
//			}
//			vertices[3].position.x = 1.0f * (teststartX - 0.5f) * 2;
//		}
//		if (teststartY != 0)
//		{
//			if (aUpper)
//			{
//				vertices[3].position.y = -1.0f * (teststartY - 0.5f) * 2;
//			}
//			vertices[2].position.y = -1.0f * (teststartY - 0.5f) * 2;
//		}
//		if (testendX != 1)
//		{
//			vertices[1].position.x = 1.0f * (testendX - 0.5f) * 2;
//			if (!aUpper)
//			{
//				vertices[2].position.x = 1.0f * (testendX - 0.5f) * 2;
//			}
//		}
//		if (testendY != 1)
//		{
//			vertices[0].position.y = -1.0f * (testendY - 0.5f) * 2;
//			if (aUpper)
//			{
//				vertices[1].position.y = -1.0f * (testendY - 0.5f) * 2;
//			}
//		}
//
//		deviceContext->Unmap(myVertexBuffer, 0);
//	}
//}
//
//void Sprite::SetCharacterUVFlipbook(TextureRect aStartTextureRect)
//{
//	ID3D11DeviceContext* deviceContext = GraphicsEngine::GetInstance()->GetContext();
//
//	D3D11_MAPPED_SUBRESOURCE mappedResource;
//	HRESULT result = deviceContext->Map(myVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
//
//	if (SUCCEEDED(result))
//	{
//		char* dataVPtr = (char*)mappedResource.pData;
//		memcpy(dataVPtr, myVertices.data(), sizeof(Sprite::Vertex) * (int)myVertices.size());
//
//		Vertex* vertices = reinterpret_cast<Vertex*>(mappedResource.pData);
//
//		vertices[0].uv = { myTextureRect.myStartX, myTextureRect.myEndY };
//		vertices[1].uv = { myTextureRect.myEndX, myTextureRect.myEndY };
//		vertices[2].uv = { myTextureRect.myEndX, myTextureRect.myStartY };
//		vertices[3].uv = { myTextureRect.myStartX, myTextureRect.myStartY };
//
//		auto testendX = aStartTextureRect.myEndX;
//		auto testendY = aStartTextureRect.myEndY;
//
//		auto teststartX = aStartTextureRect.myStartX;
//		auto teststartY = aStartTextureRect.myStartY;
//		if (teststartX != 0)
//		{
//			vertices[0].position.x = 1.0f * (teststartX - 0.5f) * 2;
//			vertices[3].position.x = 1.0f * (teststartX - 0.5f) * 2;
//		}
//		if (teststartY != 0)
//		{
//			vertices[3].position.y = -1.0f * (teststartY - 0.5f) * 2;
//			vertices[2].position.y = -1.0f * (teststartY - 0.5f) * 2;
//		}
//		if (testendX != 1)
//		{
//			vertices[1].position.x = 1.0f * (testendX - 0.5f) * 2;
//			vertices[2].position.x = 1.0f * (testendX - 0.5f) * 2;
//		}
//		if (testendY != 1)
//		{
//			vertices[0].position.y = -1.0f * (testendY - 0.5f) * 2;
//			vertices[1].position.y = -1.0f * (testendY - 0.5f) * 2;
//		}
//		deviceContext->Unmap(myVertexBuffer, 0);
//	}
//}
//
//
//std::vector<Sprite::Vertex> Sprite::CreateSquareVertices()
//{
//	std::vector<Vertex> vertices = {
//	{ { -1.0f, -1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.f }, { 0.0f, 1.0f } }, // Vertex 0
//	{ { 1.0f, -1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.f}, { 1.0f, 1.0f } }, // Vertex 1
//	{ { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.f}, { 1.0f, 0.0f } }, // Vertex 2
//	{ { -1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.f}, { 0.0f, 0.0f } }  // Vertex 3
//	};
//
//	return vertices;
//}
//
//std::vector<unsigned int> Sprite::CreateSquareIndices()
//{
//	std::vector<unsigned int> indices = {
//	0, 2, 1,
//	0, 3, 2
//	};
//
//	return indices;
//}
