#include "stdafx.h"

#include "ConstantBuffer.h"
#include "GraphicsEngine.h"

ConstantBuffer::ConstantBuffer() = default;

ConstantBuffer::~ConstantBuffer() 
{
	if (myBuffer)
	{
		myBuffer->Release();
	}
}

#include <cassert>

void ConstantBuffer::Init(int aSize, const void* aData)
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = aSize;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	HRESULT result = GraphicsEngine::GetInstance()->GetDevice()->CreateBuffer(&bd, nullptr, &myBuffer);
	assert(SUCCEEDED(result));
	if (FAILED(result))
	{
		puts("Failed to create constant buffer!");
		return;
	}

	GraphicsEngine::GetInstance()->GetContext()->UpdateSubresource(myBuffer, 0, nullptr, aData, 0, 0);
}

void ConstantBuffer::Update(void* aData)
{
	GraphicsEngine::GetInstance()->GetContext()->UpdateSubresource(myBuffer, 0, nullptr, aData, 0, 0);
}

void ConstantBuffer::Bind(int aSlot)
{
	GraphicsEngine::GetInstance()->GetContext()->VSSetConstantBuffers(aSlot, 1, &myBuffer);
	GraphicsEngine::GetInstance()->GetContext()->PSSetConstantBuffers(aSlot, 1, &myBuffer);
}
