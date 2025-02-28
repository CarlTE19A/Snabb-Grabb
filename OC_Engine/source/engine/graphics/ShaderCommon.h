#pragma one
#include "../math/vector.h"
//#include <DirectXMath.h>

enum class ConstantBufferSlot
{
	Frame = 0,
	Camera = 1,
	Light = 2,
	ShaderSettings = 3,
	Object = 4,
	Bones = 5,
	Count,
};

struct SpriteShaderInstanceData
{
	DirectX::XMMATRIX myTransform;
	Vector4<float> myColor;
	Vector4<float> myUV;
	Vector4<float> myUVRect;
};