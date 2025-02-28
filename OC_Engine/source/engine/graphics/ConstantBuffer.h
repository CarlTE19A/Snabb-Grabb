#pragma once
#include <d3d11.h>
#include <wrl\client.h>

using Microsoft::WRL::ComPtr;

class ConstantBuffer
{
public:
	ConstantBuffer();
	~ConstantBuffer();

	void Init(int aSize, const void* aData);
	void Update(void* aData);
	ID3D11Buffer* GetBuffer() { return myBuffer; }
	void Bind(int aSlot);

private:
	ID3D11Buffer* myBuffer = nullptr;
};

