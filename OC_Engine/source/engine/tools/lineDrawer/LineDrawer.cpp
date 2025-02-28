#include "stdafx.h"

#include "LineDrawer.h"

#include "graphics/GraphicsEngine.h"

//#include <d3d11.h>
#include <fstream>
//#include "Paths.h"

//#include <iostream>
#include "math/AABB3D.h"
LineDrawer::LineDrawer()
{}
LineDrawer::~LineDrawer()
{
}

void LineDrawer::Init()
{
#ifdef _DEBUG
	InitShaders();
	CreateBuffer();
#endif
}

void LineDrawer::InitShaders()
{
#ifdef _DEBUG
	GraphicsEngine& engine = *GraphicsEngine::GetInstance();
	auto& device = engine.GetDevice();

	HRESULT result;
	std::string vsData;
	{
		std::string path = "line";
		// Load shaders

		//Load Vertex Shader
		std::ifstream vsFile;
		vsFile.open(STATIC_SHADER_PATH + path + "_VS.cso", std::ios::binary);

		vsData = { std::istreambuf_iterator<char>(vsFile), std::istreambuf_iterator<char>() };
		result = device->CreateVertexShader(vsData.data(), vsData.size(), nullptr, &myVertexShader);
		if (FAILED(result))
		{
			assert(false && "Line Drawer wants more crack VS");
			return;
		}
		vsFile.close();
		std::ifstream psFile;
		psFile.open(STATIC_SHADER_PATH + path + "_PS.cso", std::ios::binary);

		//Load Pixel Shader
		std::string psData = { std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>() };
		result = device->CreatePixelShader(psData.data(), psData.size(), nullptr, &myPixelShader);
		if (FAILED(result))
		{
			assert(false && "Line Drawer wants more crack PS");
			return;
		}
		psFile.close();
	}

	{
		// Create input layout
		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		int numElements = sizeof(layout) / sizeof(D3D11_INPUT_ELEMENT_DESC);
		result = device->CreateInputLayout(layout, numElements, vsData.data(), vsData.size(), &myInputLayout);
		if (FAILED(result))
		{
			assert(false && "Line Drawer wants more crack IL");
			return;
		}
	}
#endif
}

void LineDrawer::CreateBuffer()
{
	GraphicsEngine& engine = *GraphicsEngine::GetInstance();
	auto& device = engine.GetDevice();
	HRESULT result;

	{
		// Create vertex buffer
		D3D11_BUFFER_DESC vertexBufferDesc;

		vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		vertexBufferDesc.ByteWidth = sizeof(SimpleVertex) * 2000;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		vertexBufferDesc.MiscFlags = 0;
		vertexBufferDesc.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA vertexData;

		vertexData.pSysMem = myVertices;
		vertexData.SysMemPitch = 0;
		vertexData.SysMemSlicePitch = 0;

		result = device->CreateBuffer(&vertexBufferDesc, nullptr, myVertexBuffer.ReleaseAndGetAddressOf());
		if (FAILED(result))
		{
			assert(false && "Line Drawer wants more crack Vertex Buffer");
			return;
		}
	}
}


void LineDrawer::Draw(DebugLine& aLine)
{
#ifdef _DEBUG
	myLines.push_back(aLine);
#else
	aLine;
#endif
}

void LineDrawer::Render(DebugLine& aLine)
{

	auto* instance = GraphicsEngine::GetInstance();
	auto* context = instance->GetContext();
	//SetShader();
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	context->IASetInputLayout(myInputLayout.Get());

	UpdateVertexes(aLine);
	unsigned int stride = sizeof(SimpleVertex);
	unsigned int offset = 0;
	GraphicsEngine::GetInstance()->GetContext()->IASetVertexBuffers(0, 1, myVertexBuffer.GetAddressOf(), &stride, &offset);

	context->VSSetShader(myVertexShader.Get(), nullptr, 0);
	context->PSSetShader(myPixelShader.Get(), nullptr, 0);

	context->Draw(2, 0);
}

void LineDrawer::DrawAllLines()
{
#ifdef _DEBUG
	if (myActive == false)
		return;
	for (int i = 0; i < myLines.size(); ++i)
	{
		Render(myLines[i]);
	}
	myLines.clear();
#endif
}

void LineDrawer::DrawBoxCollider(const AABB3D<float>& anAABB3D)
{
#ifdef _DEBUG
	if (myActive == false)
		return;
	Vector3f positions[8] =
	{
		anAABB3D.GetMin(),
		Vector3f(anAABB3D.GetMax().x, anAABB3D.GetMin().y, anAABB3D.GetMin().z),
		Vector3f(anAABB3D.GetMin().x, anAABB3D.GetMin().y, anAABB3D.GetMax().z),
		Vector3f(anAABB3D.GetMin().x, anAABB3D.GetMax().y, anAABB3D.GetMin().z),
		Vector3f(anAABB3D.GetMax().x, anAABB3D.GetMin().y, anAABB3D.GetMax().z),
		Vector3f(anAABB3D.GetMin().x, anAABB3D.GetMax().y, anAABB3D.GetMax().z),
		Vector3f(anAABB3D.GetMax().x, anAABB3D.GetMax().y, anAABB3D.GetMin().z),
		anAABB3D.GetMax()
	};
	DebugLine line1;
	line1.start = positions[0];
	line1.end = positions[1];
	line1.color = Vector4f(0, 1, 0, 1);
	Draw(line1);


	line1.start = positions[0];
	line1.end = positions[2];
	line1.color = Vector4f(1, 0, 0, 1);
	Draw(line1);



	line1.start = positions[0];
	line1.end = positions[3];
	line1.color = Vector4f(0, 0, 1, 1);
	Draw(line1);



	line1.start = positions[1];
	line1.end = positions[4];
	line1.color = Vector4f(0, 0, 1, 1);
	Draw(line1);



	line1.start = positions[2];
	line1.end = positions[4];
	line1.color = Vector4f(0, 0, 1, 1);
	Draw(line1);


	;
	line1.start = positions[4];
	line1.end = positions[7];
	line1.color = Vector4f(0, 0, 1, 1);
	Draw(line1);



	line1.start = positions[5];
	line1.end = positions[3];
	line1.color = Vector4f(0, 0, 1, 1);
	Draw(line1);



	line1.start = positions[6];
	line1.end = positions[7];
	line1.color = Vector4f(0, 0, 1, 1);
	Draw(line1);



	line1.start = positions[3];
	line1.end = positions[6];
	line1.color = Vector4f(0, 0, 1, 1);
	Draw(line1);



	line1.start = positions[1];
	line1.end = positions[6];
	line1.color = Vector4f(0, 0, 1, 1);
	Draw(line1);



	line1.start = positions[5];
	line1.end = positions[7];
	line1.color = Vector4f(0, 0, 1, 1);
	Draw(line1);



	line1.start = positions[5];
	line1.end = positions[2];
	line1.color = Vector4f(0, 0, 1, 1);
	Draw(line1);
#else
	anAABB3D;
#endif
}

void LineDrawer::UpdateVertexes(DebugLine& aLine)
{
#ifdef _DEBUG
	D3D11_MAPPED_SUBRESOURCE mappedVertexResource;
	SimpleVertex* dataPtr;
	auto context = GraphicsEngine::GetInstance()->GetContext();
	HRESULT result = context->Map(myVertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedVertexResource);
	if (FAILED(result))
	{
		LogError("Failed to map vertex buffer (LineDrawer)");
		return;
	}

	dataPtr = (SimpleVertex*)mappedVertexResource.pData;

	dataPtr[0].x = aLine.start.x;
	dataPtr[0].y = aLine.start.y;
	dataPtr[0].z = aLine.start.z;

	dataPtr[1].x = aLine.end.x;
	dataPtr[1].y = aLine.end.y;
	dataPtr[1].z = aLine.end.z;

	dataPtr[0].r = aLine.color.x;
	dataPtr[0].g = aLine.color.y;
	dataPtr[0].b = aLine.color.z;
	dataPtr[0].a = aLine.color.w;

	dataPtr[1].r = aLine.color.x;
	dataPtr[1].g = aLine.color.y;
	dataPtr[1].b = aLine.color.z;
	dataPtr[1].a = aLine.color.w;

	context->Unmap(myVertexBuffer.Get(), 0);
#else
	aLine;
#endif
}