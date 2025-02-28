#include <graphics\ConstantBuffer.h>


class Text;
class TextMesh
{
public:
	struct Vertex
	{
		Vector4<float> position;
		Vector4<float> color;
		Vector2<float> uv;
	};
	TextMesh();

	void AddCharacter(float x, float y, float width, float height, float myStartX, float myEndX, float myStartY, float myEndY);

	void GenerateMesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, Text* aText);

	Texture& GetTexture() { return aTexture; }
	void SetTexture(Texture* aTexture1);

	std::vector<D3D11_INPUT_ELEMENT_DESC> GetInputLayout();


private:
	std::vector<Vertex> vertices;
	DirectX::XMMATRIX myModelToWorld;
	std::vector<int> indices;
	std::vector<Vector3f> myPositions;
	std::vector<Vector3f> scalevector;


	Texture aTexture;

	Shader myShader;
	struct alignas(16) Align16
	{
		DirectX::XMMATRIX modelToWorld;
		Vector3f color;
		float alphaValue;
	};

	Align16 myData;

	ConstantBuffer myConstantBuffer;

	ID3D11Buffer* textMeshVertexBuffer;
	ID3D11Buffer* textMeshIndexBuffer;
};