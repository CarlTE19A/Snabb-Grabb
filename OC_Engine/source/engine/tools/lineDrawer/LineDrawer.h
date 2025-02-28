#pragma once
#include "graphics/model/Vertex.h"
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;
struct ID3D11Buffer;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11InputLayout;


template<class> class AABB3D;
class CircleCollision;

struct DebugLine
{
	Vector4f color;
	Vector3f start;
	Vector3f end;
};

class LineDrawer
{
public:
	LineDrawer();
	~LineDrawer();

	void Init();
	void Draw(DebugLine& aLine);
	void Render(DebugLine& aLine);
	void DrawAllLines();

	void DrawBoxCollider(const AABB3D<float>& anAABB3D);
	//void Draw(Line* aLine, int aCount);
	bool myActive = true;
private:
	LineDrawer& operator=(const LineDrawer&) = delete;
	SimpleVertex myVertices[2];
	std::vector<DebugLine> myLines;
	void CreateBuffer();
	void UpdateVertexes(DebugLine& aLine);
	ComPtr<ID3D11Buffer> myVertexBuffer;
	ComPtr<ID3D11VertexShader> myVertexShader;
	ComPtr<ID3D11PixelShader> myPixelShader;
	ComPtr<ID3D11InputLayout> myInputLayout;
	
	void InitShaders();
};