#pragma once
#include "graphics\GraphicsData.h"
#include "../PhysXCode/PhysXManager.h"
#include <PhysX\PxParticleGpu.h>
#include "math\Vector.h"

class Mesh;
class ModelDrawer;

enum ParticleEffectType
{
	eExplosionEffect = 0,
	eBurstEffect = 1,
	eDustParticles = 2,
	eWaterFallEffect = 3,
	eFireEffect = 4,
	eTrailEffect = 5,
	eCount
};

struct StructuredBuffer
{
	ComPtr<ID3D11Buffer> buffer;
	ComPtr<ID3D11ShaderResourceView> srv;
	ComPtr<ID3D11UnorderedAccessView> uav;
	int element_max_count; // in elements
	int element_size; // in bytes
	//ID3D11Buffer* stagingBuffer;
};

#pragma pack(push, 1)
struct PARTICLE
{
	DirectX::XMFLOAT4 pos;
	DirectX::XMFLOAT4 velo;
	DirectX::XMFLOAT4 color;
	float size;
	float time;
	float maxLifetime;
	int shouldBeStatic;
};

struct ParticlesPerFrame
{
	int totaltParticlesCreated;
	//float garbage[3];
};

struct ParticleGlobalSeed
{
	float globalSeed;
	//float garbage[3];
};

struct ParticleBuffer
{
	DirectX::XMMATRIX centerPos;
	DirectX::XMFLOAT4 explostionPosition;
	DirectX::XMFLOAT4 color;
	float gravityForce;
	float size;
	float maxLifeTime;
	float particlefrequency;
	float angelOfParticleTrail;
	float maxRadius;
	float maxAngel;
	int maxParticlesToCreatePerCreateCall;
	int maxNumberOfparticles; //How many particle the the particle system can show at onces
	int trailEffectId;
	int garbage[2];
};
#pragma pack(pop)

struct PARTICLE_VERTEX
{
	DirectX::XMFLOAT4 Color;
};

struct CB_CS
{
	UINT param[4];
	float paramf[4];
	float explosionStrength;
	float falloffDistance;
	float garabage[2];
};

class ParticleSystem
{
public:
	ParticleSystem();
	~ParticleSystem();

	bool SaveParticleSystemSettings(const std::string& aFileName);
	bool LoadParticleSystemSettings(std::string_view aString);

	bool Init(unsigned int aNbOfParticles);

	void Update();

	void EmitParticles();

	void SetSpread(float aSpread) { myParticleSpread = aSpread; }
	float* GetParticleSpread() { return &myParticleSpread; }

	void SetTrailAngel(float aTrailAngel) { myAngleOfPTrailParticle = aTrailAngel; }
	float& GetTrailAngel() { return myAngleOfPTrailParticle; }

	void SetParticleSystemType(int aType) { myParticleSystemType = aType; }
	int& GetParticleSystemType() { return myParticleSystemType; }

	void SetColor(DirectX::XMFLOAT4 aColor) { myColor = aColor; }
	DirectX::XMFLOAT4& GetColor() { return myColor; }


	void SetExplostionPosition(DirectX::XMFLOAT4 aPos) { myExplostionPosition = aPos; }
	DirectX::XMFLOAT4& GetExplostionPosition() { return myExplostionPosition; }

	void SetTransform(DirectX::XMMATRIX aMatrix) { myPos = aMatrix; }
	void SetPosition(DirectX::XMVECTOR aPos) { myPos.r[3] = aPos; }
	void SetPosition(Vector3f aPos);
	DirectX::XMVECTOR& GetPosition() { return myPos.r[3]; }

	void SetRotation(DirectX::XMVECTOR aQuaternion)
	{
		DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(DirectX::XMVectorGetX(aQuaternion), DirectX::XMVectorGetY(aQuaternion), DirectX::XMVectorGetZ(aQuaternion));

		// Assuming myPos is your XMMATRIX that you wish to set the rotation for
		// Apply the rotation by multiplying the matrices
		myPos = /*DirectX::XMMatrixScaling(1, 1, 1) **/ rotationMatrix * DirectX::XMMatrixTranslationFromVector(myPos.r[3]);
	}

	DirectX::XMVECTOR& GetRotation()
	{
		return myRotation;
	}

	void GetSize(float aSize) { mySize = aSize; }
	float& GetSize() { return mySize; }

	void SetLifeTime(float aFloat) { myLifeTime = aFloat; }
	float& GetLifeTime() { return myLifeTime; }

	void GetVelocity(DirectX::XMFLOAT4 aVelo) { myVelocity = aVelo; };
	DirectX::XMFLOAT4& GetVelocity() { return myVelocity; };

	void SetNumberOfParticles(int aNbOfParticles) { myNumberOfParticles = aNbOfParticles; }
	int& GetNumberOfParticles() { return myNumberOfParticles; }

	void SetExplosionStrength(int aValue) { myExplosionStrength = aValue; }
	int& GetExplosionStrength() { return myExplosionStrength; }

	void SetFalloffDistance(int aValue) { myExplosionFalloffDistance = aValue; }
	int& GetFalloffDistance() { return myExplosionFalloffDistance; }

	void SetAmoutParticlesToCreatePerFrame(int aValue) { myMaxParticlesToCreatePerCreateCall = aValue; }
	int& GetMaxAmoutThatCanBeCreatedPerCreateCall() { return myMaxParticlesToCreatePerCreateCall; }

	void SetTrailEffectType(int aValue) { myTrailEffectToPlay = aValue; }
	int& GetTrailEffectType() { return myTrailEffectToPlay; }

	void SetGravityForce(float aFloat) { myGravity = aFloat; }
	float& GetGravityForce() { return myGravity; }

	void SetParticleTime(float aFloat) { myParticleSpawnFrequency = aFloat; }
	float& GetParticleTime() { return myParticleSpawnFrequency; }

	Texture& GetTextureC() { return myTexture_c; }
	void SetTextureC(Texture aTexture) { myTexture_c = aTexture; }

	Texture& GetTextureM() { return myTexture_m; }
	void SetTextureM(Texture aTexture) { myTexture_m = aTexture; }

	Texture& GetTexture_n() { return myTexture_n; }
	void SetTexture_n(Texture aTexture) { myTexture_n = aTexture; }

	Texture& GetTexture_fx() { return myTexture_fx; }
	void SetTexture_fx(Texture aTexture) { myTexture_fx = aTexture; }

	void SetTexturePath(std::string aString) { myTexturePath = aString; }
	std::string& GetTexturePath() { return myTexturePath; }

	void UpdateBuffers();

private:
	int myMaxParticlesToCreatePerCreateCall = 25;
	int myParticleSystemType = 0;
	int myExplosionStrength;
	int myExplosionFalloffDistance;
	int myNumberOfParticles;
	int myTrailEffectToPlay = 0;
	std::string myTexturePath;

	float myAngleOfPTrailParticle;
	float myParticleSpawnFrequency = 0.05f;
	float timer;
	float mySize;
	float myGravity;
	float myLifeTime;
	float myParticleSpread = 1000.0f;
	float myMaxRadius = 50;
	float myMaxAngel = DirectX::XM_PI * 2;

	DirectX::XMMATRIX myPos;
	DirectX::XMFLOAT4 myColor;
	DirectX::XMFLOAT4 myVelocity;
	DirectX::XMFLOAT4 myExplostionPosition;
	DirectX::XMVECTOR myRotation;

	ComPtr<ID3D11Buffer> g_pcbCS = nullptr;

	ComPtr<ID3D11Buffer> myParticleBuffer = nullptr;

	StructuredBuffer myStructuredBufferOld;
	StructuredBuffer myStructuredBufferNext;

	StructuredBuffer myTotalParticlesPerFrame;

	StructuredBuffer myGlobalParticleSeed;

	ComPtr<ID3D11Buffer> myVertexBuffer;
	ComPtr<ID3D11Buffer> myIndexBuffer;

	ComPtr<ID3D11VertexShader> myVertexShader = nullptr;
	ComPtr<ID3D11PixelShader> myPixelShader = nullptr;
	ComPtr<ID3D11ComputeShader> myComputeShader = nullptr;

	//ID3D11InputLayout* g_pParticleVertexLayout = nullptr;

	Texture myTexture_c;
	Texture myTexture_m;
	Texture myTexture_n;
	Texture myTexture_fx;
};