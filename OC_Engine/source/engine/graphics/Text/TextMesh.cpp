#include "stdafx.h"
#include "TextMesh.h"
#include "graphics\GraphicsEngine.h"

#include "Text.h"

TextMesh::TextMesh()
{
	myShader.Init("shaders/Text_VS.cso", "shaders/TextShader_PS.cso", GetInputLayout());
	myModelToWorld = DirectX::XMMatrixScaling(0.05f, 0.05f, 1.f) * DirectX::XMMatrixIdentity();
	myModelToWorld.r[3].m128_f32[0] = 0.5f;
	myModelToWorld.r[3].m128_f32[1] = 0.5f;
	myModelToWorld.r[3].m128_f32[2] = 1.f;
	myConstantBuffer.Init(sizeof(myData), &myData);
}

void TextMesh::AddCharacter(float x, float y, float width, float height, float myStartX, float myEndX, float myStartY, float myEndY)
{
	myPositions.push_back(Vector3f{ x, y ,1.f });
	scalevector.push_back(Vector3f{ width ,height,1.f });

	Vertex topLeft;
	Vertex topRight;
	Vertex bottomLeft;
	Vertex bottomRight;

	topLeft.position = Vector4<float>(-1.f, -1.f, 1, 1.0f);
	topLeft.color = { 1, 1, 1, 1 };
	topLeft.uv = Vector2<float>(myStartX, myEndY);


	topRight.position = Vector4<float>(1.f, -1.f, 1, 1.0f);

	topRight.color = { 1, 1, 1, 1 };
	topRight.uv = Vector2<float>(myEndX, myEndY);

	bottomLeft.position = Vector4<float>(1.f, 1.f, 1, 1.0f);

	bottomLeft.color = { 1, 1, 1, 1 };
	bottomLeft.uv = Vector2<float>(myEndX, myStartY);

	bottomRight.position = Vector4<float>(-1.f, 1.f, 1, 1.0f);

	bottomRight.color = { 1, 1, 1, 1 };
	bottomRight.uv = Vector2<float>(myStartX, myStartY);

	vertices.push_back(topLeft);
	vertices.push_back(topRight);
	vertices.push_back(bottomLeft);
	vertices.push_back(bottomRight);

	int baseIndex = (int)vertices.size() - 4;

	indices.push_back(baseIndex);
	indices.push_back(baseIndex + 2);
	indices.push_back(baseIndex + 1);
	indices.push_back(baseIndex);
	indices.push_back(baseIndex + 3);
	indices.push_back(baseIndex + 2);
}

void TextMesh::GenerateMesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, Text* aText)
{
	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(TextMesh::Vertex) * (UINT)vertices.size();
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexData;
	ZeroMemory(&vertexData, sizeof(vertexData));
	vertexData.pSysMem = vertices.data();
	HRESULT result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &textMeshVertexBuffer);

	if (FAILED(result))
	{
		return;
	}

	//myData.modelToWorld = myModelToWorld;

	//myData.color = Vector3f{ aColor.x ,aColor.y,aColor.z };
	//myData.alphaValue = 1;

	//myConstantBuffer.Update(&myData);

	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(UINT) * (UINT)indices.size();
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA indexData;
	ZeroMemory(&indexData, sizeof(indexData));
	indexData.pSysMem = indices.data();
	device->CreateBuffer(&indexBufferDesc, &indexData, &textMeshIndexBuffer);

	if (FAILED(result))
	{
		return;
	}

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	myShader.SetShader();
	aTexture.Bind(1, deviceContext);

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	deviceContext->IASetVertexBuffers(0, 1, &textMeshVertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(textMeshIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	//float rad = (DirectX::XM_PI / 180) * aText->GetRotation();
	for (int i = 0; i < indices.size(); i += 6)
	{
		myModelToWorld = DirectX::XMMatrixScaling(scalevector[i / 6].x * 0.001f, scalevector[i / 6].y * 0.001f, 1.f) * DirectX::XMMatrixRotationZ(aText->GetRotation()) * DirectX::XMMatrixIdentity();
		//myModelToWorld = DirectX::XMMatrixScaling(1, 1, 1) * DirectX::XMMatrixRotationZ(0)/* * DirectX::XMMatrixTranslation(myPositions[i / 6].x, myPositions[i / 6].y, myPositions[i / 6].z)*/;

		float ndcX = (2.0f * ((myPositions[i / 6].x / GraphicsEngine::GetInstance()->GetWindowSize().x)) - 1.0f);
		float ndcY = 1.0f - (2.0f * ((myPositions[i / 6].y) / GraphicsEngine::GetInstance()->GetWindowSize().y));

		ndcX, ndcY;
		Vector4 aColor = aText->GetColor();
		myModelToWorld.r[3].m128_f32[0] = ndcX;
		myModelToWorld.r[3].m128_f32[1] = ndcY;
		myModelToWorld.r[3].m128_f32[2] = 0.f;
		myData.modelToWorld = myModelToWorld;
		myData.color = { aColor.x, aColor.y, aColor.z };
		myData.alphaValue = aColor.w;
		myConstantBuffer.Update(&myData);
		myConstantBuffer.Bind(1);
		deviceContext->DrawIndexed(6, i, 0);
	}
}
void TextMesh::SetTexture(Texture* aTexture1)
{
	aTexture = *aTexture1;
}

std::vector<D3D11_INPUT_ELEMENT_DESC> TextMesh::GetInputLayout()
{
	std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayout =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	return inputLayout;
}
