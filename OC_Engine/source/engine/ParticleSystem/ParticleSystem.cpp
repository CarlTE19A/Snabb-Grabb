#include "stdafx.h"
#include "ParticleSystem.h"
#include "graphics\GraphicsEngine.h"

#include <fstream>

#include "Engine.h"

#include "math\Timer.h"

#include"imgui/imgui.h"

#include "imgui\imgui_stdlib.h"

#include "nlohmann\json.hpp"

#include "graphics\GBuffer\GBuffer.h"
#include <random>

#include"mainSingelton\MainSingelton.h"

#include "ParticleCache.h"

#define MAX_PARTICLES 100000;

ParticleSystem::ParticleSystem()
{
	myNumberOfParticles = 0;
	mySize = 10;
	myColor = { 1,1,1,1 };
	myLifeTime = 10;
	//mySpeed = 250.f;
	myAngleOfPTrailParticle = 0.5f;
	timer = 0.f;
}

ParticleSystem::~ParticleSystem()
{}

bool ParticleSystem::SaveParticleSystemSettings(const std::string& aFileName)
{
	nlohmann::json tempJson;


	for (int i = 0; i < 4; ++i)
	{
		tempJson["Matrix"][("row" + std::to_string(i))] = myPos.r[i].m128_f32;
	}

	tempJson["Creation Limit"] = myMaxParticlesToCreatePerCreateCall;

	tempJson["Angle"] = myAngleOfPTrailParticle;

	tempJson["Particle spawn frequency"] = myParticleSpawnFrequency;

	tempJson["Particle TrailEffect"] = myTrailEffectToPlay;

	tempJson["Particle texture"] = myTexturePath;

	tempJson["Gravity Force"] = myGravity;

	tempJson["Particle velocity"]["x"] = myVelocity.x;
	tempJson["Particle velocity"]["y"] = myVelocity.y;
	tempJson["Particle velocity"]["z"] = myVelocity.z;

	tempJson["Particle Color"]["x"] = myColor.x;
	tempJson["Particle Color"]["y"] = myColor.y;
	tempJson["Particle Color"]["z"] = myColor.z;
	tempJson["Particle Color"]["w"] = myColor.w;

	tempJson["Particle size"] = mySize;

	tempJson["Explostion Position"] = { myExplostionPosition.x, myExplostionPosition.y, myExplostionPosition.z, myExplostionPosition.w };
	tempJson["Explostion Strengt"] = myExplosionStrength;
	tempJson["Explostion Fall Off Distance"] = myExplosionFalloffDistance;


	tempJson["Number Of Particles"] = myNumberOfParticles;
	tempJson["Particle System Type"] = myParticleSystemType;
	tempJson["Particle Lifetime"] = myLifeTime;

	tempJson["Particle Spread"] = myParticleSpread;

	std::ofstream file;
	file.open(STATIC_ASSET_PATH "json/ParticleSystems/" + aFileName + ".json");

	if (file.is_open())
	{
		file << tempJson;
	}
	file.close();

	return true;
}

bool ParticleSystem::LoadParticleSystemSettings(std::string_view aString)
{
	static std::unordered_map<std::string_view, nlohmann::json> jsonCache;

	if (jsonCache.count(aString) == 0)
	{
		nlohmann::json tempJson;
		std::ifstream file;
		file.open(STATIC_ASSET_PATH "json/ParticleSystems/" + std::string(aString) /*+ ".json"*/);

		if (!file.is_open())
		{
			return false;
		}
		file >> tempJson;
		jsonCache[aString] = tempJson;

		file.close();
	}

	nlohmann::json& tempJson = jsonCache[aString];

	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; j++)
		{
			//auto jeffSize = tempJson["Matrix"][("row" + std::to_string(i))].size(); jeffSize;

			myPos.r[i].m128_f32[j] = tempJson["Matrix"][("row" + std::to_string(i))][j].get<float>();
		}
	}

	myTrailEffectToPlay = tempJson["Particle TrailEffect"];

	myAngleOfPTrailParticle = tempJson["Angle"];

	myGravity = tempJson["Gravity Force"];

	myTexturePath = tempJson["Particle texture"];

	myTexture_c = MainSingleton::GetInstance().GetParticleCache().GetParticleTexture(myTexturePath);

	myTexture_m = MainSingleton::GetInstance().GetParticleCache().GetParticleTexture("Particles/spraksprite_round_m.dds");

	myTexture_fx = MainSingleton::GetInstance().GetParticleCache().GetParticleTexture("Particles/spraksprite_round_fx.dds");

	myMaxParticlesToCreatePerCreateCall = tempJson["Creation Limit"];

	myParticleSpawnFrequency = tempJson["Particle spawn frequency"];

	myVelocity = { tempJson["Particle velocity"]["x"],tempJson["Particle velocity"]["y"] ,tempJson["Particle velocity"]["z"], 0 };

	myColor = { tempJson["Particle Color"]["x"],tempJson["Particle Color"]["y"] ,tempJson["Particle Color"]["z"], tempJson["Particle Color"]["w"] };

	myParticleSpread = tempJson["Particle Spread"];

	myExplostionPosition = { tempJson["Explostion Position"][0],tempJson["Explostion Position"][1],tempJson["Explostion Position"][2],tempJson["Explostion Position"][3] };
	myExplosionStrength = tempJson["Explostion Strengt"];
	myExplosionFalloffDistance = tempJson["Explostion Fall Off Distance"];

	myLifeTime = tempJson["Particle Lifetime"];

	myParticleSystemType = tempJson["Particle System Type"];

	mySize = tempJson["Particle size"];

	myNumberOfParticles = tempJson["Number Of Particles"];

	UpdateBuffers();

	return true;
}

float RPercent()
{
	float ret = (float)((rand() % 10000) - 5000);
	return ret / 5000.0f;
}

float generateRandomFloat(float min, float max)
{
	return min + static_cast<float>(rand()) / RAND_MAX * (max - min);
}

bool ParticleSystem::Init(unsigned int aNbOfParticles)
{
	myNumberOfParticles = aNbOfParticles;

	UpdateBuffers();

	myTexturePath = "Particles/spraksprite_round_c.dds";

	myTexture_c = MainSingleton::GetInstance().GetParticleCache().GetParticleTexture(myTexturePath);

	myTexture_m = MainSingleton::GetInstance().GetParticleCache().GetParticleTexture("Particles/spraksprite_round_m.dds");

	myTexture_fx = MainSingleton::GetInstance().GetParticleCache().GetParticleTexture("Particles/spraksprite_round_fx.dds");

	return true;
}

void ParticleSystem::Update()
{
	ParticlesPerFrame zeroValue;
	zeroValue.totaltParticlesCreated = 0;

	ID3D11DeviceContext* context = GraphicsEngine::GetInstance()->GetContext();

	int dimx = int(ceil(myNumberOfParticles / 128.f));

	context->UpdateSubresource(myTotalParticlesPerFrame.buffer.Get(), 0, nullptr, &zeroValue, 0, 0);

	D3D11_MAPPED_SUBRESOURCE MappedResource;
	context->Map(g_pcbCS.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
	auto pcbCS = reinterpret_cast<CB_CS*>(MappedResource.pData);
	pcbCS->param[0] = myNumberOfParticles;
	pcbCS->param[1] = dimx;
	pcbCS->paramf[0] = Engine::GetInstance()->GetTimer()->GetDeltaTime();
	pcbCS->paramf[1] = 1;
	pcbCS->explosionStrength = (float)myExplosionStrength;
	pcbCS->falloffDistance = (float)myExplosionFalloffDistance;
	context->Unmap(g_pcbCS.Get(), 0);

	timer -= Engine::GetInstance()->GetTimer()->GetDeltaTime();
	D3D11_MAPPED_SUBRESOURCE MappedResourceCone;
	context->Map(myParticleBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResourceCone);
	ParticleBuffer* tempParticleBuffer = (ParticleBuffer*)(MappedResourceCone.pData);

	tempParticleBuffer->maxParticlesToCreatePerCreateCall = myMaxParticlesToCreatePerCreateCall;

	tempParticleBuffer->angelOfParticleTrail = myAngleOfPTrailParticle;

	tempParticleBuffer->maxNumberOfparticles = myNumberOfParticles;

	tempParticleBuffer->explostionPosition = myExplostionPosition;

	tempParticleBuffer->trailEffectId = myTrailEffectToPlay;

	tempParticleBuffer->particlefrequency = timer;

	tempParticleBuffer->maxLifeTime = myLifeTime;

	tempParticleBuffer->gravityForce = myGravity;

	tempParticleBuffer->maxRadius = myMaxRadius;

	tempParticleBuffer->maxAngel = myMaxAngel;

	tempParticleBuffer->centerPos = myPos;

	tempParticleBuffer->color = myColor;

	tempParticleBuffer->size = mySize;

	context->Unmap(myParticleBuffer.Get(), 0);

	if (timer < 0)
	{
		timer = myParticleSpawnFrequency;
	}
}

void ParticleSystem::EmitParticles()
{
	GraphicsEngine& graphics = *GraphicsEngine::GetInstance();

	graphics.SetBlendState(BlendState::AlphaBlend);
	////GraphicsEngine::GetInstance()->SetRasterizerState(RasterizerState::FrontfaceCulling);
	////GraphicsEngine::GetInstance()->SetSamplerState(SamplerFilter::Trilinear, SamplerAddressMode::Wrap);
	GraphicsEngine::GetInstance()->UseDepth(true);
	graphics.UpdateGPUStates();
	ID3D11DeviceContext* context = graphics.GetContext();

	int dimx = int(ceil(myNumberOfParticles / 128.f));

	{
		context->CSSetShader(myComputeShader.Get(), nullptr, 0);

		graphics.GetGBuffer().SetAsResourceOnSlot(GBuffer::GBufferTexture::WorldPosition, 10);
		graphics.GetGBuffer().SetAsResourceOnSlot(GBuffer::GBufferTexture::Normal, 12);

		// For CS input            
		ID3D11ShaderResourceView* aRViews[1] = { myStructuredBufferOld.srv.Get() };
		context->CSSetShaderResources(15, 1, aRViews);

		// For CS output
		ID3D11UnorderedAccessView* aUAViews[1] = { myStructuredBufferNext.uav.Get() };
		context->CSSetUnorderedAccessViews(1, 1, aUAViews, NULL);

		ID3D11UnorderedAccessView* aUAViews2[1] = { myTotalParticlesPerFrame.uav.Get() };
		context->CSSetUnorderedAccessViews(2, 1, aUAViews2, NULL);


		ID3D11UnorderedAccessView* aUAViews3[1] = { myGlobalParticleSeed.uav.Get() };
		context->CSSetUnorderedAccessViews(3, 1, aUAViews3, NULL);

		context->CSSetConstantBuffers(11, 1, g_pcbCS.GetAddressOf());

		context->CSSetConstantBuffers(12, 1, myParticleBuffer.GetAddressOf());

		// Run the CS
		context->Dispatch(dimx, 1, 1);
		ID3D11ShaderResourceView* null = nullptr;
		GraphicsEngine::GetInstance()->GetContext()->CSSetShaderResources(5, 1, &null);

		GraphicsEngine::GetInstance()->SetEngineBackBufferAsActiveTarget(true);

		// Unbind resources for CS
		ID3D11ShaderResourceView* ppSRVNULL[1] = { nullptr };
		context->CSSetShaderResources(15, 1, ppSRVNULL);

		ID3D11UnorderedAccessView* ppUAViewNULL[1] = { nullptr };
		context->CSSetUnorderedAccessViews(1, 1, ppUAViewNULL, (UINT*)(&aUAViews));
		context->CSSetUnorderedAccessViews(2, 1, ppUAViewNULL, (UINT*)(&aUAViews));
		context->CSSetUnorderedAccessViews(3, 1, ppUAViewNULL, (UINT*)(&aUAViews));
		graphics.GetGBuffer().ClearAllAsResources(10);
	}

	context->VSSetShader(myVertexShader.Get(), nullptr, 0);
	context->PSSetShader(myPixelShader.Get(), nullptr, 0);
	context->IASetIndexBuffer(NULL, DXGI_FORMAT_R32_UINT, 0);

	UINT stride[1] = { sizeof(PARTICLE_VERTEX) };
	UINT offset[1] = { 0 };
	ID3D11Buffer* null = nullptr;
	context->IASetInputLayout(NULL);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetVertexBuffers(0, 1, &null, stride, offset);

	ID3D11ShaderResourceView* aRViews[1] = { myStructuredBufferNext.srv.Get() };
	context->VSSetShaderResources(15, 1, aRViews);

	myTexture_c.Bind(16, context);
	myTexture_m.Bind(17, context);
	myTexture_fx.Bind(18, context);

	context->Draw(myNumberOfParticles * 6, 0);

	std::swap(myStructuredBufferOld, myStructuredBufferNext);
}

void ParticleSystem::SetPosition(Vector3f aPos)
{
	myPos.r[3].m128_f32[0] = aPos.x; myPos.r[3].m128_f32[1] = aPos.y;	myPos.r[3].m128_f32[2] = aPos.z;
}

void ParticleSystem::UpdateBuffers()
{
	auto& device = GraphicsEngine::GetInstance()->GetDevice();

	switch (myParticleSystemType)
	{
	case eExplosionEffect:
	{
		HRESULT hr = S_OK;
		std::string vsData;
		//{
			// Load shaders
		std::string shaderPath = STATIC_SHADER_PATH;
		std::ifstream vsFile;
		vsFile.open(shaderPath + "Particle_VS.cso", std::ios::binary);

		vsData = { std::istreambuf_iterator<char>(vsFile), std::istreambuf_iterator<char>() };
		hr = device->CreateVertexShader(vsData.data(), vsData.size(), nullptr, myVertexShader.GetAddressOf());

		vsFile.close();

		std::ifstream psFile;
		psFile.open(shaderPath + "Particle_PS.cso", std::ios::binary);
		std::string psData = { std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>() };

		hr = device->CreatePixelShader(psData.data(), psData.size(), nullptr, myPixelShader.GetAddressOf());
		psFile.close();

		std::ifstream csFile;
		csFile.open(shaderPath + "ParticleExplostion_CS.cso", std::ios::binary);
		std::string csData = { std::istreambuf_iterator<char>(csFile), std::istreambuf_iterator<char>() };

		hr = device->CreateComputeShader(csData.data(), csData.size(), nullptr, myComputeShader.GetAddressOf());
		csFile.close();

		PARTICLE* pData = new PARTICLE[myNumberOfParticles];
		//float fCenterSpread = g_fSpread * 0.50f;
		DirectX::XMVECTOR Center = myPos.r[3];
		DirectX::XMVECTOR vCenter = Center;

		std::random_device rd;
		std::mt19937 gen(rd());

		// Distribution for speed (adjust the range as needed)
		std::uniform_real_distribution<> speedDistrib(2, 75);

		// Distribution for the angle in the xy-plane (0 to 2pi)
		std::uniform_real_distribution<> angleDistrib(0, 2 * 3.145f);

		// Distribution for the angle from the vertical (0 to pi/2 to ensure upward direction)
		std::uniform_real_distribution<> verticalAngleDistrib(0, 3.145f / 2);

		std::uniform_real_distribution<> directionDistrib(-1.0f, 1.0f); // For generating direction components.

		DirectX::XMVECTOR direction = { 0,0,0,0 };

		float speed = 0.0f;

		//float terminalVelocityX = 50.0f; // Maximum speed in the x direction
		//float terminalVelocityY = 50.0f; // Maximum speed in the y direction
		//float terminalVelocityZ = 50.0f; // Maximum speed in the z direction


		for (int i = 0; i < myNumberOfParticles; i++)
		{
			DirectX::XMVECTOR vDelta = DirectX::XMVectorReplicate(myParticleSpread);

			while (DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(vDelta)) > myParticleSpread * myParticleSpread)
			{
				vDelta = DirectX::XMVectorSet(RPercent() * myParticleSpread, RPercent() * myParticleSpread, RPercent() * myParticleSpread, 0.f);
			}

			DirectX::XMVECTOR vPos = DirectX::XMVectorAdd(vCenter, vDelta);

			pData[i].color = myColor;
			pData[i].size = mySize;
			pData[i].time = 0;
			pData[i].maxLifetime = myLifeTime /** generateRandomFloat(0.1f, 1.f)*/;

			DirectX::XMStoreFloat3(reinterpret_cast<DirectX::XMFLOAT3*>(&pData[i].pos), vPos);
			pData[i].pos.w = 1;
			//pData[i].pos = vPos;

			if (true)
			{
				if (i % 25 == 0)
				{
					float x = (float)directionDistrib(gen);
					float y = (float)directionDistrib(gen);
					float z = (float)directionDistrib(gen);

					direction = DirectX::XMVectorSet(x, fabs(y), z, 0.0f); // Using fabs to ensure upward direction.
					direction = DirectX::XMVector3Normalize(direction); // Normalize the direction vector.

					//myVelocity = { x,y,z,1 };
				}
				//std::random_device rd;  // a seed source for the random number engine
				//std::mt19937 gen(rd()); // mersenne_twister_engine seeded with rd()
				//std::uniform_int_distribution<> distrib(1, 6);

				speed = (float)speedDistrib(gen);
				DirectX::XMVECTOR velocity = DirectX::XMVectorScale(direction, speed);

				//// Clamp velocities to their terminal velocities
				//pData[i].velo.x = std::clamp(velocity.x, -terminalVelocityX, terminalVelocityX);
				//pData[i].velo.y = std::clamp(velocity.y, -terminalVelocityY, terminalVelocityY);
				//pData[i].velo.z = std::clamp(velocity.z, -terminalVelocityZ, terminalVelocityZ);


				DirectX::XMStoreFloat4(&pData[i].velo, velocity); // Assuming velo is a DirectX::XMFLOAT4.
			}

			//pData[i].velo = myVelocity;

			pData[i].shouldBeStatic = 0;
		}

		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = pData;

		D3D11_BUFFER_DESC desc = {};
		desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
		desc.ByteWidth = myNumberOfParticles * sizeof(PARTICLE);
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		desc.StructureByteStride = sizeof(PARTICLE);
		desc.Usage = D3D11_USAGE_DEFAULT;

		hr = device->CreateBuffer(&desc, &InitData, myStructuredBufferOld.buffer.GetAddressOf());
		hr = device->CreateBuffer(&desc, &InitData, myStructuredBufferNext.buffer.GetAddressOf());

		ParticlesPerFrame* pData1 = new ParticlesPerFrame[1];
		pData1[0].totaltParticlesCreated = 0;
		//pData1[0].garbage[0] = 0;
		//pData1[0].garbage[1] = 0;
		//pData1[0].garbage[2] = 0;

		D3D11_SUBRESOURCE_DATA InitDat1a;
		InitDat1a.pSysMem = pData1;

		D3D11_BUFFER_DESC descTEST = {};
		descTEST.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
		descTEST.ByteWidth = sizeof(ParticlesPerFrame);
		descTEST.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		descTEST.StructureByteStride = sizeof(ParticlesPerFrame);
		descTEST.Usage = D3D11_USAGE_DEFAULT;

		hr = device->CreateBuffer(&descTEST, &InitDat1a, myTotalParticlesPerFrame.buffer.GetAddressOf());

		D3D11_BUFFER_DESC descForGlobalParticleSeed = {};
		descForGlobalParticleSeed.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
		descForGlobalParticleSeed.ByteWidth = sizeof(ParticlesPerFrame);
		descForGlobalParticleSeed.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		descForGlobalParticleSeed.StructureByteStride = sizeof(ParticlesPerFrame);
		descForGlobalParticleSeed.Usage = D3D11_USAGE_DEFAULT;

		ParticleGlobalSeed* pDataSeedParticle = new ParticleGlobalSeed[1];
		pDataSeedParticle[0].globalSeed = 0;

		D3D11_SUBRESOURCE_DATA InitDataForGlobalSeed;
		InitDataForGlobalSeed.pSysMem = pDataSeedParticle;

		hr = device->CreateBuffer(&descForGlobalParticleSeed, &InitDataForGlobalSeed, myGlobalParticleSeed.buffer.GetAddressOf());

		D3D11_SHADER_RESOURCE_VIEW_DESC DescRVForGlobalParticleSeed = {};
		DescRVForGlobalParticleSeed.Format = DXGI_FORMAT_UNKNOWN;
		DescRVForGlobalParticleSeed.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		DescRVForGlobalParticleSeed.Buffer.FirstElement = 0;
		DescRVForGlobalParticleSeed.Buffer.NumElements = descForGlobalParticleSeed.ByteWidth / descForGlobalParticleSeed.StructureByteStride;

		hr = device->CreateShaderResourceView(myGlobalParticleSeed.buffer.Get(), &DescRVForGlobalParticleSeed, myGlobalParticleSeed.srv.GetAddressOf());

		D3D11_UNORDERED_ACCESS_VIEW_DESC DescUAVParticleSeed = {};
		DescUAVParticleSeed.Format = DXGI_FORMAT_UNKNOWN;
		DescUAVParticleSeed.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		DescUAVParticleSeed.Buffer.FirstElement = 0;
		DescUAVParticleSeed.Buffer.NumElements = descForGlobalParticleSeed.ByteWidth / descForGlobalParticleSeed.StructureByteStride;

		hr = device->CreateUnorderedAccessView(myGlobalParticleSeed.buffer.Get(), &DescUAVParticleSeed, myGlobalParticleSeed.uav.GetAddressOf());

		D3D11_SHADER_RESOURCE_VIEW_DESC DescRV = {};
		DescRV.Format = DXGI_FORMAT_UNKNOWN;
		DescRV.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		DescRV.Buffer.FirstElement = 0;
		DescRV.Buffer.NumElements = desc.ByteWidth / desc.StructureByteStride;
		hr = device->CreateShaderResourceView(myStructuredBufferOld.buffer.Get(), &DescRV, myStructuredBufferOld.srv.GetAddressOf());
		hr = device->CreateShaderResourceView(myStructuredBufferNext.buffer.Get(), &DescRV, myStructuredBufferNext.srv.GetAddressOf());

		D3D11_SHADER_RESOURCE_VIEW_DESC DescRVTEST = {};
		DescRVTEST.Format = DXGI_FORMAT_UNKNOWN;
		DescRVTEST.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		DescRVTEST.Buffer.FirstElement = 0;
		DescRVTEST.Buffer.NumElements = descTEST.ByteWidth / descTEST.StructureByteStride;

		hr = device->CreateShaderResourceView(myTotalParticlesPerFrame.buffer.Get(), &DescRVTEST, myTotalParticlesPerFrame.srv.GetAddressOf());


		D3D11_UNORDERED_ACCESS_VIEW_DESC DescUAV = {};
		DescUAV.Format = DXGI_FORMAT_UNKNOWN;
		DescUAV.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		DescUAV.Buffer.FirstElement = 0;
		DescUAV.Buffer.NumElements = desc.ByteWidth / desc.StructureByteStride;
		hr = device->CreateUnorderedAccessView(myStructuredBufferOld.buffer.Get(), &DescUAV, myStructuredBufferOld.uav.GetAddressOf());
		hr = device->CreateUnorderedAccessView(myStructuredBufferNext.buffer.Get(), &DescUAV, myStructuredBufferNext.uav.GetAddressOf());

		D3D11_UNORDERED_ACCESS_VIEW_DESC DescUAVTEST = {};
		DescUAVTEST.Format = DXGI_FORMAT_UNKNOWN;
		DescUAVTEST.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		DescUAVTEST.Buffer.FirstElement = 0;
		DescUAVTEST.Buffer.NumElements = descTEST.ByteWidth / descTEST.StructureByteStride;

		hr = device->CreateUnorderedAccessView(myTotalParticlesPerFrame.buffer.Get(), &DescUAVTEST, myTotalParticlesPerFrame.uav.GetAddressOf());

		D3D11_BUFFER_DESC Desc;
		Desc.Usage = D3D11_USAGE_DYNAMIC;
		Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		Desc.MiscFlags = 0;

		Desc.ByteWidth = sizeof(CB_CS);
		hr = device->CreateBuffer(&Desc, nullptr, g_pcbCS.GetAddressOf());

		Desc.ByteWidth = sizeof(ParticleBuffer);
		hr = device->CreateBuffer(&Desc, nullptr, myParticleBuffer.GetAddressOf());

		break;
	}
	case eBurstEffect:
	{
		HRESULT hr = S_OK;
		std::string vsData;
		// Load shaders
		std::string shaderPath = STATIC_SHADER_PATH;
		std::ifstream vsFile;
		vsFile.open(shaderPath + "Particle_VS.cso", std::ios::binary);

		vsData = { std::istreambuf_iterator<char>(vsFile), std::istreambuf_iterator<char>() };
		hr = device->CreateVertexShader(vsData.data(), vsData.size(), nullptr, myVertexShader.GetAddressOf());

		vsFile.close();

		std::ifstream psFile;
		psFile.open(shaderPath + "Particle_PS.cso", std::ios::binary);
		std::string psData = { std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>() };

		hr = device->CreatePixelShader(psData.data(), psData.size(), nullptr, myPixelShader.GetAddressOf());
		psFile.close();
		std::ifstream csFile;
		csFile.open(shaderPath + "ParticleBurstExplostion_CS.cso", std::ios::binary);
		std::string csData = { std::istreambuf_iterator<char>(csFile), std::istreambuf_iterator<char>() };

		hr = device->CreateComputeShader(csData.data(), csData.size(), nullptr, myComputeShader.GetAddressOf());
		csFile.close();

		D3D11_BUFFER_DESC desc = {};
		desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
		desc.ByteWidth = myNumberOfParticles * sizeof(PARTICLE);
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		desc.StructureByteStride = sizeof(PARTICLE);
		desc.Usage = D3D11_USAGE_DEFAULT;

		PARTICLE* pData = new PARTICLE[myNumberOfParticles];
		//float fCenterSpread = g_fSpread * 0.50f;
		DirectX::XMVECTOR vec = myPos.r[3];
		//DirectX::XMFLOAT3 Center = { myPos, myPos.y, myPos.z };
		DirectX::XMVECTOR vCenter = vec;

		for (int i = 0; i < myNumberOfParticles; i++)
		{
			DirectX::XMVECTOR vDelta = DirectX::XMVectorReplicate(myParticleSpread);

			while (DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(vDelta)) > myParticleSpread * myParticleSpread)
			{
				vDelta = DirectX::XMVectorSet(RPercent() * myParticleSpread, RPercent() * myParticleSpread, RPercent() * myParticleSpread, 0.f);
			}

			DirectX::XMVECTOR vPos = DirectX::XMVectorAdd(vCenter, vDelta);

			DirectX::XMStoreFloat3(reinterpret_cast<DirectX::XMFLOAT3*>(&pData[i].pos), vPos);
			pData[i].pos.w = (float)myNumberOfParticles * (float)myNumberOfParticles;
			pData[i].color = myColor;
			pData[i].size = mySize;
			pData[i].time = 0;
			pData[i].maxLifetime = 0;// myLifeTime* generateRandomFloat(0.1f, 1.f);
			pData[i].velo = myVelocity;
		}

		D3D11_BUFFER_DESC descForGlobalParticleSeed = {};
		descForGlobalParticleSeed.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
		descForGlobalParticleSeed.ByteWidth = sizeof(ParticlesPerFrame);
		descForGlobalParticleSeed.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		descForGlobalParticleSeed.StructureByteStride = sizeof(ParticlesPerFrame);
		descForGlobalParticleSeed.Usage = D3D11_USAGE_DEFAULT;

		ParticleGlobalSeed* pDataSeedParticle = new ParticleGlobalSeed[1];
		pDataSeedParticle[0].globalSeed = 0;

		D3D11_SUBRESOURCE_DATA InitDataForGlobalSeed;
		InitDataForGlobalSeed.pSysMem = pDataSeedParticle;

		hr = device->CreateBuffer(&descForGlobalParticleSeed, &InitDataForGlobalSeed, myGlobalParticleSeed.buffer.GetAddressOf());

		D3D11_SHADER_RESOURCE_VIEW_DESC DescRVForGlobalParticleSeed = {};
		DescRVForGlobalParticleSeed.Format = DXGI_FORMAT_UNKNOWN;
		DescRVForGlobalParticleSeed.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		DescRVForGlobalParticleSeed.Buffer.FirstElement = 0;
		DescRVForGlobalParticleSeed.Buffer.NumElements = descForGlobalParticleSeed.ByteWidth / descForGlobalParticleSeed.StructureByteStride;

		hr = device->CreateShaderResourceView(myGlobalParticleSeed.buffer.Get(), &DescRVForGlobalParticleSeed, myGlobalParticleSeed.srv.GetAddressOf());

		D3D11_UNORDERED_ACCESS_VIEW_DESC DescUAVParticleSeed = {};
		DescUAVParticleSeed.Format = DXGI_FORMAT_UNKNOWN;
		DescUAVParticleSeed.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		DescUAVParticleSeed.Buffer.FirstElement = 0;
		DescUAVParticleSeed.Buffer.NumElements = descForGlobalParticleSeed.ByteWidth / descForGlobalParticleSeed.StructureByteStride;

		hr = device->CreateUnorderedAccessView(myGlobalParticleSeed.buffer.Get(), &DescUAVParticleSeed, myGlobalParticleSeed.uav.GetAddressOf());

		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = pData;

		hr = device->CreateBuffer(&desc, &InitData, &myStructuredBufferOld.buffer);
		hr = device->CreateBuffer(&desc, &InitData, &myStructuredBufferNext.buffer);

		D3D11_SHADER_RESOURCE_VIEW_DESC DescRV = {};
		DescRV.Format = DXGI_FORMAT_UNKNOWN;
		DescRV.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		DescRV.Buffer.FirstElement = 0;
		DescRV.Buffer.NumElements = desc.ByteWidth / desc.StructureByteStride;
		hr = device->CreateShaderResourceView(myStructuredBufferOld.buffer.Get(), &DescRV, myStructuredBufferOld.srv.GetAddressOf());
		hr = device->CreateShaderResourceView(myStructuredBufferNext.buffer.Get(), &DescRV, myStructuredBufferNext.srv.GetAddressOf());

		D3D11_UNORDERED_ACCESS_VIEW_DESC DescUAV = {};
		DescUAV.Format = DXGI_FORMAT_UNKNOWN;
		DescUAV.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		DescUAV.Buffer.FirstElement = 0;
		DescUAV.Buffer.NumElements = desc.ByteWidth / desc.StructureByteStride;
		hr = device->CreateUnorderedAccessView(myStructuredBufferOld.buffer.Get(), &DescUAV, myStructuredBufferOld.uav.GetAddressOf());
		hr = device->CreateUnorderedAccessView(myStructuredBufferNext.buffer.Get(), &DescUAV, myStructuredBufferNext.uav.GetAddressOf());

		D3D11_BUFFER_DESC Desc;
		Desc.Usage = D3D11_USAGE_DYNAMIC;
		Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		Desc.MiscFlags = 0;

		Desc.ByteWidth = sizeof(CB_CS);
		device->CreateBuffer(&Desc, nullptr, g_pcbCS.GetAddressOf());

		Desc.ByteWidth = sizeof(ParticleBuffer);
		hr = device->CreateBuffer(&Desc, nullptr, myParticleBuffer.GetAddressOf());
		break;
	}
	case eDustParticles:
	{
		HRESULT hr = S_OK;
		std::string vsData;
		//{
			// Load shaders
		std::string shaderPath = STATIC_SHADER_PATH;
		std::ifstream vsFile;
		vsFile.open(shaderPath + "Particle_VS.cso", std::ios::binary);

		vsData = { std::istreambuf_iterator<char>(vsFile), std::istreambuf_iterator<char>() };
		hr = device->CreateVertexShader(vsData.data(), vsData.size(), nullptr, myVertexShader.GetAddressOf());

		vsFile.close();

		std::ifstream psFile;
		psFile.open(shaderPath + "Particle_PS.cso", std::ios::binary);
		std::string psData = { std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>() };

		hr = device->CreatePixelShader(psData.data(), psData.size(), nullptr, myPixelShader.GetAddressOf());
		psFile.close();
		std::ifstream csFile;
		csFile.open(shaderPath + "ParticleDust_CS.cso", std::ios::binary);
		std::string csData = { std::istreambuf_iterator<char>(csFile), std::istreambuf_iterator<char>() };

		hr = device->CreateComputeShader(csData.data(), csData.size(), nullptr, myComputeShader.GetAddressOf());
		csFile.close();

		D3D11_BUFFER_DESC desc = {};
		desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
		desc.ByteWidth = myNumberOfParticles * sizeof(PARTICLE);
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		desc.StructureByteStride = sizeof(PARTICLE);
		desc.Usage = D3D11_USAGE_DEFAULT;

		PARTICLE* pData = new PARTICLE[myNumberOfParticles];
		//float fCenterSpread = g_fSpread * 0.50f;
		DirectX::XMVECTOR Center = myPos.r[3];
		DirectX::XMVECTOR vCenter = Center;

		for (int i = 0; i < myNumberOfParticles; i++)
		{
			DirectX::XMVECTOR vDelta = DirectX::XMVectorReplicate(myParticleSpread);

			while (DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(vDelta)) > myParticleSpread * myParticleSpread)
			{
				vDelta = DirectX::XMVectorSet(RPercent() * myParticleSpread, RPercent() * myParticleSpread, RPercent() * myParticleSpread, 0.f);
			}

			DirectX::XMVECTOR vPos = DirectX::XMVectorAdd(vCenter, vDelta);

			DirectX::XMStoreFloat3(reinterpret_cast<DirectX::XMFLOAT3*>(&pData[i].pos), vPos);
			pData[i].pos.w = (float)myNumberOfParticles * (float)myNumberOfParticles;
			pData[i].color = myColor;
			pData[i].size = mySize;
			pData[i].time = 0;
			pData[i].maxLifetime = 0;// myLifeTime* generateRandomFloat(0.1f, 1.f);
			pData[i].velo = myVelocity;
		}

		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = pData;

		//ParticlesPerFrame* pData1 = new ParticlesPerFrame;
		//pData1->totaltParticlesCreated = 0;

		hr = device->CreateBuffer(&desc, &InitData, myStructuredBufferOld.buffer.GetAddressOf());
		hr = device->CreateBuffer(&desc, &InitData, myStructuredBufferNext.buffer.GetAddressOf());

		//InitData.pSysMem = pData1;

		//desc.ByteWidth = sizeof(ParticlesPerFrame);
		//desc.StructureByteStride = sizeof(ParticlesPerFrame);

		//hr = device->CreateBuffer(&desc, &InitData, &myTotalParticlesPerFrame.buffer);

		D3D11_BUFFER_DESC descForGlobalParticleSeed = {};
		descForGlobalParticleSeed.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
		descForGlobalParticleSeed.ByteWidth = sizeof(ParticlesPerFrame);
		descForGlobalParticleSeed.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		descForGlobalParticleSeed.StructureByteStride = sizeof(ParticlesPerFrame);
		descForGlobalParticleSeed.Usage = D3D11_USAGE_DEFAULT;

		ParticleGlobalSeed* pDataSeedParticle = new ParticleGlobalSeed[1];
		pDataSeedParticle[0].globalSeed = 0;

		D3D11_SUBRESOURCE_DATA InitDataForGlobalSeed;
		InitDataForGlobalSeed.pSysMem = pDataSeedParticle;

		hr = device->CreateBuffer(&descForGlobalParticleSeed, &InitDataForGlobalSeed, myGlobalParticleSeed.buffer.GetAddressOf());

		D3D11_SHADER_RESOURCE_VIEW_DESC DescRVForGlobalParticleSeed = {};
		DescRVForGlobalParticleSeed.Format = DXGI_FORMAT_UNKNOWN;
		DescRVForGlobalParticleSeed.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		DescRVForGlobalParticleSeed.Buffer.FirstElement = 0;
		DescRVForGlobalParticleSeed.Buffer.NumElements = descForGlobalParticleSeed.ByteWidth / descForGlobalParticleSeed.StructureByteStride;

		hr = device->CreateShaderResourceView(myGlobalParticleSeed.buffer.Get(), &DescRVForGlobalParticleSeed, myGlobalParticleSeed.srv.GetAddressOf());

		D3D11_UNORDERED_ACCESS_VIEW_DESC DescUAVParticleSeed = {};
		DescUAVParticleSeed.Format = DXGI_FORMAT_UNKNOWN;
		DescUAVParticleSeed.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		DescUAVParticleSeed.Buffer.FirstElement = 0;
		DescUAVParticleSeed.Buffer.NumElements = descForGlobalParticleSeed.ByteWidth / descForGlobalParticleSeed.StructureByteStride;

		hr = device->CreateUnorderedAccessView(myGlobalParticleSeed.buffer.Get(), &DescUAVParticleSeed, myGlobalParticleSeed.uav.GetAddressOf());

		D3D11_SHADER_RESOURCE_VIEW_DESC DescRV = {};
		DescRV.Format = DXGI_FORMAT_UNKNOWN;
		DescRV.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		DescRV.Buffer.FirstElement = 0;
		DescRV.Buffer.NumElements = desc.ByteWidth / desc.StructureByteStride;
		hr = device->CreateShaderResourceView(myStructuredBufferOld.buffer.Get(), &DescRV, myStructuredBufferOld.srv.GetAddressOf());
		hr = device->CreateShaderResourceView(myStructuredBufferNext.buffer.Get(), &DescRV, myStructuredBufferNext.srv.GetAddressOf());

		//desc.ByteWidth = sizeof(ParticlesPerFrame);
		//desc.StructureByteStride = sizeof(ParticlesPerFrame);
		//DescRV.Buffer.NumElements = desc.ByteWidth / desc.StructureByteStride;

		//hr = device->CreateShaderResourceView(myTotalParticlesPerFrame.buffer, &DescRV, &myTotalParticlesPerFrame.srv);

		D3D11_UNORDERED_ACCESS_VIEW_DESC DescUAV = {};
		DescUAV.Format = DXGI_FORMAT_UNKNOWN;
		DescUAV.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		DescUAV.Buffer.FirstElement = 0;
		DescUAV.Buffer.NumElements = desc.ByteWidth / desc.StructureByteStride;
		hr = device->CreateUnorderedAccessView(myStructuredBufferOld.buffer.Get(), &DescUAV, myStructuredBufferOld.uav.GetAddressOf());
		hr = device->CreateUnorderedAccessView(myStructuredBufferNext.buffer.Get(), &DescUAV, myStructuredBufferNext.uav.GetAddressOf());

		//desc.ByteWidth = sizeof(ParticlesPerFrame);
		//desc.StructureByteStride = sizeof(ParticlesPerFrame);
		//DescUAV.Buffer.NumElements = desc.ByteWidth / desc.StructureByteStride;

		//hr = device->CreateUnorderedAccessView(myTotalParticlesPerFrame.buffer, &DescUAV, &myTotalParticlesPerFrame.uav);

		D3D11_BUFFER_DESC Desc;
		Desc.Usage = D3D11_USAGE_DYNAMIC;
		Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		Desc.MiscFlags = 0;

		Desc.ByteWidth = sizeof(CB_CS);
		hr = device->CreateBuffer(&Desc, nullptr, g_pcbCS.GetAddressOf());

		Desc.ByteWidth = sizeof(ParticleBuffer);
		hr = device->CreateBuffer(&Desc, nullptr, myParticleBuffer.GetAddressOf());
		break;
	}
	case eWaterFallEffect:
	{
		HRESULT hr = S_OK;
		std::string vsData;
		//{
			// Load shaders
		std::string shaderPath = STATIC_SHADER_PATH;
		std::ifstream vsFile;
		vsFile.open(shaderPath + "Particle_VS.cso", std::ios::binary);

		vsData = { std::istreambuf_iterator<char>(vsFile), std::istreambuf_iterator<char>() };
		hr = device->CreateVertexShader(vsData.data(), vsData.size(), nullptr, myVertexShader.GetAddressOf());

		vsFile.close();

		std::ifstream psFile;
		psFile.open(shaderPath + "Particle_PS.cso", std::ios::binary);
		std::string psData = { std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>() };

		hr = device->CreatePixelShader(psData.data(), psData.size(), nullptr, myPixelShader.GetAddressOf());
		psFile.close();
		std::ifstream csFile;
		csFile.open(shaderPath + "ParticleWaterFall_CS.cso", std::ios::binary);
		std::string csData = { std::istreambuf_iterator<char>(csFile), std::istreambuf_iterator<char>() };

		hr = device->CreateComputeShader(csData.data(), csData.size(), nullptr, myComputeShader.GetAddressOf());
		csFile.close();

		D3D11_BUFFER_DESC desc = {};
		desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
		desc.ByteWidth = myNumberOfParticles * sizeof(PARTICLE);
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		desc.StructureByteStride = sizeof(PARTICLE);
		desc.Usage = D3D11_USAGE_DEFAULT;

		PARTICLE* pData = new PARTICLE[myNumberOfParticles];
		//float fCenterSpread = g_fSpread * 0.50f;
		DirectX::XMVECTOR Center = myPos.r[3];
		DirectX::XMVECTOR vCenter = Center;

		for (int i = 0; i < myNumberOfParticles; i++)
		{
			DirectX::XMVECTOR vDelta = DirectX::XMVectorReplicate(myParticleSpread);

			while (DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(vDelta)) > myParticleSpread * myParticleSpread)
			{
				vDelta = DirectX::XMVectorSet(RPercent() * myParticleSpread, RPercent() * myParticleSpread, RPercent() * myParticleSpread, 0.f);
			}

			DirectX::XMVECTOR vPos = DirectX::XMVectorAdd(vCenter, vDelta);

			DirectX::XMStoreFloat3(reinterpret_cast<DirectX::XMFLOAT3*>(&pData[i].pos), vPos);
			pData[i].pos.w = (float)myNumberOfParticles * (float)myNumberOfParticles;
			pData[i].color = { 0,0,0,0 };
			pData[i].size = mySize;
			pData[i].time = 0;
			pData[i].maxLifetime = 0;// myLifeTime* generateRandomFloat(0.1f, 1.f);
			pData[i].velo = myVelocity;
		}

		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = pData;

		//ParticlesPerFrame* pData1 = new ParticlesPerFrame;
		//pData1->totaltParticlesCreated = 0;

		hr = device->CreateBuffer(&desc, &InitData, myStructuredBufferOld.buffer.GetAddressOf());
		hr = device->CreateBuffer(&desc, &InitData, myStructuredBufferNext.buffer.GetAddressOf());

		//InitData.pSysMem = pData1;

		//desc.ByteWidth = myNumberOfParticles * sizeof(ParticlesPerFrame);
		//desc.StructureByteStride = sizeof(ParticlesPerFrame);

		D3D11_BUFFER_DESC descForGlobalParticleSeed = {};
		descForGlobalParticleSeed.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
		descForGlobalParticleSeed.ByteWidth = sizeof(ParticlesPerFrame);
		descForGlobalParticleSeed.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		descForGlobalParticleSeed.StructureByteStride = sizeof(ParticlesPerFrame);
		descForGlobalParticleSeed.Usage = D3D11_USAGE_DEFAULT;

		ParticleGlobalSeed* pDataSeedParticle = new ParticleGlobalSeed[1];
		pDataSeedParticle[0].globalSeed = 0;

		D3D11_SUBRESOURCE_DATA InitDataForGlobalSeed;
		InitDataForGlobalSeed.pSysMem = pDataSeedParticle;

		hr = device->CreateBuffer(&descForGlobalParticleSeed, &InitDataForGlobalSeed, myGlobalParticleSeed.buffer.GetAddressOf());

		D3D11_SHADER_RESOURCE_VIEW_DESC DescRVForGlobalParticleSeed = {};
		DescRVForGlobalParticleSeed.Format = DXGI_FORMAT_UNKNOWN;
		DescRVForGlobalParticleSeed.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		DescRVForGlobalParticleSeed.Buffer.FirstElement = 0;
		DescRVForGlobalParticleSeed.Buffer.NumElements = descForGlobalParticleSeed.ByteWidth / descForGlobalParticleSeed.StructureByteStride;

		hr = device->CreateShaderResourceView(myGlobalParticleSeed.buffer.Get(), &DescRVForGlobalParticleSeed, myGlobalParticleSeed.srv.GetAddressOf());

		D3D11_UNORDERED_ACCESS_VIEW_DESC DescUAVParticleSeed = {};
		DescUAVParticleSeed.Format = DXGI_FORMAT_UNKNOWN;
		DescUAVParticleSeed.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		DescUAVParticleSeed.Buffer.FirstElement = 0;
		DescUAVParticleSeed.Buffer.NumElements = descForGlobalParticleSeed.ByteWidth / descForGlobalParticleSeed.StructureByteStride;

		hr = device->CreateUnorderedAccessView(myGlobalParticleSeed.buffer.Get(), &DescUAVParticleSeed, myGlobalParticleSeed.uav.GetAddressOf());

		//hr = device->CreateBuffer(&desc, &InitData, &myTotalParticlesPerFrame.buffer);

		D3D11_SHADER_RESOURCE_VIEW_DESC DescRV = {};
		DescRV.Format = DXGI_FORMAT_UNKNOWN;
		DescRV.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		DescRV.Buffer.FirstElement = 0;
		DescRV.Buffer.NumElements = desc.ByteWidth / desc.StructureByteStride;
		hr = device->CreateShaderResourceView(myStructuredBufferOld.buffer.Get(), &DescRV, myStructuredBufferOld.srv.GetAddressOf());
		hr = device->CreateShaderResourceView(myStructuredBufferNext.buffer.Get(), &DescRV, myStructuredBufferNext.srv.GetAddressOf());

		D3D11_UNORDERED_ACCESS_VIEW_DESC DescUAV = {};
		DescUAV.Format = DXGI_FORMAT_UNKNOWN;
		DescUAV.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		DescUAV.Buffer.FirstElement = 0;
		DescUAV.Buffer.NumElements = desc.ByteWidth / desc.StructureByteStride;
		hr = device->CreateUnorderedAccessView(myStructuredBufferOld.buffer.Get(), &DescUAV, myStructuredBufferOld.uav.GetAddressOf());
		hr = device->CreateUnorderedAccessView(myStructuredBufferNext.buffer.Get(), &DescUAV, myStructuredBufferNext.uav.GetAddressOf());

		D3D11_BUFFER_DESC Desc;
		Desc.Usage = D3D11_USAGE_DYNAMIC;
		Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		Desc.MiscFlags = 0;

		Desc.ByteWidth = sizeof(CB_CS);
		device->CreateBuffer(&Desc, nullptr, g_pcbCS.GetAddressOf());

		Desc.ByteWidth = sizeof(ParticleBuffer);
		hr = device->CreateBuffer(&Desc, nullptr, myParticleBuffer.GetAddressOf());
		break;
	}
	case eFireEffect:
	{
		HRESULT hr = S_OK;
		std::string vsData;
		//{
			// Load shaders
		std::string shaderPath = STATIC_SHADER_PATH;
		std::ifstream vsFile;
		vsFile.open(shaderPath + "Particle_VS.cso", std::ios::binary);

		vsData = { std::istreambuf_iterator<char>(vsFile), std::istreambuf_iterator<char>() };
		hr = device->CreateVertexShader(vsData.data(), vsData.size(), nullptr, myVertexShader.GetAddressOf());

		vsFile.close();

		std::ifstream psFile;
		psFile.open(shaderPath + "Particle_PS.cso", std::ios::binary);
		std::string psData = { std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>() };

		hr = device->CreatePixelShader(psData.data(), psData.size(), nullptr, myPixelShader.GetAddressOf());
		psFile.close();
		std::ifstream csFile;
		csFile.open(shaderPath + "ParticleWaterFall_CS.cso", std::ios::binary);
		std::string csData = { std::istreambuf_iterator<char>(csFile), std::istreambuf_iterator<char>() };

		hr = device->CreateComputeShader(csData.data(), csData.size(), nullptr, myComputeShader.GetAddressOf());
		csFile.close();

		D3D11_BUFFER_DESC desc = {};
		desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
		desc.ByteWidth = myNumberOfParticles * sizeof(PARTICLE);
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		desc.StructureByteStride = sizeof(PARTICLE);
		desc.Usage = D3D11_USAGE_DEFAULT;

		PARTICLE* pData = new PARTICLE[myNumberOfParticles];
		//float fCenterSpread = g_fSpread * 0.50f;
		DirectX::XMVECTOR Center = myPos.r[3];
		DirectX::XMVECTOR vCenter = Center;

		for (int i = 0; i < myNumberOfParticles; i++)
		{
			DirectX::XMVECTOR vDelta = DirectX::XMVectorReplicate(myParticleSpread);

			while (DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(vDelta)) > myParticleSpread * myParticleSpread)
			{
				vDelta = DirectX::XMVectorSet(RPercent() * myParticleSpread, RPercent() * myParticleSpread, RPercent() * myParticleSpread, 0.f);
			}

			DirectX::XMVECTOR vPos = DirectX::XMVectorAdd(vCenter, vDelta);

			DirectX::XMStoreFloat3(reinterpret_cast<DirectX::XMFLOAT3*>(&pData[i].pos), vPos);
			pData[i].pos.w = (float)myNumberOfParticles * (float)myNumberOfParticles;
			pData[i].color = myColor;
			pData[i].size = mySize;
			pData[i].time = 0;
			pData[i].maxLifetime = 0;// myLifeTime* generateRandomFloat(0.1f, 1.f);
			pData[i].velo = myVelocity;
		}

		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = pData;

		//ParticlesPerFrame* pData1 = new ParticlesPerFrame;
		//pData1->totaltParticlesCreated = 0;

		hr = device->CreateBuffer(&desc, &InitData, &myStructuredBufferOld.buffer);
		hr = device->CreateBuffer(&desc, &InitData, &myStructuredBufferNext.buffer);


		D3D11_BUFFER_DESC descForGlobalParticleSeed = {};
		descForGlobalParticleSeed.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
		descForGlobalParticleSeed.ByteWidth = sizeof(ParticlesPerFrame);
		descForGlobalParticleSeed.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		descForGlobalParticleSeed.StructureByteStride = sizeof(ParticlesPerFrame);
		descForGlobalParticleSeed.Usage = D3D11_USAGE_DEFAULT;

		ParticleGlobalSeed* pDataSeedParticle = new ParticleGlobalSeed[1];
		pDataSeedParticle[0].globalSeed = 0;

		D3D11_SUBRESOURCE_DATA InitDataForGlobalSeed;
		InitDataForGlobalSeed.pSysMem = pDataSeedParticle;

		hr = device->CreateBuffer(&descForGlobalParticleSeed, &InitDataForGlobalSeed, myGlobalParticleSeed.buffer.GetAddressOf());

		D3D11_SHADER_RESOURCE_VIEW_DESC DescRVForGlobalParticleSeed = {};
		DescRVForGlobalParticleSeed.Format = DXGI_FORMAT_UNKNOWN;
		DescRVForGlobalParticleSeed.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		DescRVForGlobalParticleSeed.Buffer.FirstElement = 0;
		DescRVForGlobalParticleSeed.Buffer.NumElements = descForGlobalParticleSeed.ByteWidth / descForGlobalParticleSeed.StructureByteStride;

		hr = device->CreateShaderResourceView(myGlobalParticleSeed.buffer.Get(), &DescRVForGlobalParticleSeed, myGlobalParticleSeed.srv.GetAddressOf());

		D3D11_UNORDERED_ACCESS_VIEW_DESC DescUAVParticleSeed = {};
		DescUAVParticleSeed.Format = DXGI_FORMAT_UNKNOWN;
		DescUAVParticleSeed.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		DescUAVParticleSeed.Buffer.FirstElement = 0;
		DescUAVParticleSeed.Buffer.NumElements = descForGlobalParticleSeed.ByteWidth / descForGlobalParticleSeed.StructureByteStride;

		hr = device->CreateUnorderedAccessView(myGlobalParticleSeed.buffer.Get(), &DescUAVParticleSeed, myGlobalParticleSeed.uav.GetAddressOf());
		//InitData.pSysMem = pData1;

		//desc.ByteWidth = sizeof(ParticlesPerFrame);
		//desc.StructureByteStride = sizeof(ParticlesPerFrame);

		//hr = device->CreateBuffer(&desc, &InitData, &myTotalParticlesPerFrame.buffer);

		D3D11_SHADER_RESOURCE_VIEW_DESC DescRV = {};
		DescRV.Format = DXGI_FORMAT_UNKNOWN;
		DescRV.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		DescRV.Buffer.FirstElement = 0;
		DescRV.Buffer.NumElements = desc.ByteWidth / desc.StructureByteStride;
		hr = device->CreateShaderResourceView(myStructuredBufferOld.buffer.Get(), &DescRV, myStructuredBufferOld.srv.GetAddressOf());
		hr = device->CreateShaderResourceView(myStructuredBufferNext.buffer.Get(), &DescRV, myStructuredBufferNext.srv.GetAddressOf());

		//desc.ByteWidth = sizeof(ParticlesPerFrame);
		//desc.StructureByteStride = sizeof(ParticlesPerFrame);
		//DescRV.Buffer.NumElements = desc.ByteWidth / desc.StructureByteStride;

		//hr = device->CreateShaderResourceView(myTotalParticlesPerFrame.buffer, &DescRV, &myTotalParticlesPerFrame.srv);

		D3D11_UNORDERED_ACCESS_VIEW_DESC DescUAV = {};
		DescUAV.Format = DXGI_FORMAT_UNKNOWN;
		DescUAV.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		DescUAV.Buffer.FirstElement = 0;
		DescUAV.Buffer.NumElements = desc.ByteWidth / desc.StructureByteStride;
		hr = device->CreateUnorderedAccessView(myStructuredBufferOld.buffer.Get(), &DescUAV, myStructuredBufferOld.uav.GetAddressOf());
		hr = device->CreateUnorderedAccessView(myStructuredBufferNext.buffer.Get(), &DescUAV, myStructuredBufferNext.uav.GetAddressOf());

		//desc.ByteWidth = sizeof(ParticlesPerFrame);
		//desc.StructureByteStride = sizeof(ParticlesPerFrame);
		//DescUAV.Buffer.NumElements = desc.ByteWidth / desc.StructureByteStride;

		//hr = device->CreateUnorderedAccessView(myTotalParticlesPerFrame.buffer, &DescUAV, &myTotalParticlesPerFrame.uav);
		D3D11_BUFFER_DESC Desc;
		Desc.Usage = D3D11_USAGE_DYNAMIC;
		Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		Desc.MiscFlags = 0;

		Desc.ByteWidth = sizeof(CB_CS);
		device->CreateBuffer(&Desc, nullptr, g_pcbCS.GetAddressOf());

		Desc.ByteWidth = sizeof(ParticleBuffer);
		hr = device->CreateBuffer(&Desc, nullptr, myParticleBuffer.GetAddressOf());
		break;
	}
	case eTrailEffect:
	{
		HRESULT hr = S_OK;
		std::string vsData;
		//{
			// Load shaders
		std::string shaderPath = STATIC_SHADER_PATH;
		std::ifstream vsFile;
		vsFile.open(shaderPath + "Particle_VS.cso", std::ios::binary);

		vsData = { std::istreambuf_iterator<char>(vsFile), std::istreambuf_iterator<char>() };
		hr = device->CreateVertexShader(vsData.data(), vsData.size(), nullptr, myVertexShader.GetAddressOf());

		vsFile.close();

		std::ifstream psFile;
		psFile.open(shaderPath + "Particle_PS.cso", std::ios::binary);
		std::string psData = { std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>() };

		hr = device->CreatePixelShader(psData.data(), psData.size(), nullptr, myPixelShader.GetAddressOf());
		psFile.close();
		std::ifstream csFile;
		csFile.open(shaderPath + "TrailEffect_CS.cso", std::ios::binary);
		std::string csData = { std::istreambuf_iterator<char>(csFile), std::istreambuf_iterator<char>() };

		hr = device->CreateComputeShader(csData.data(), csData.size(), nullptr, myComputeShader.GetAddressOf());
		csFile.close();

		D3D11_BUFFER_DESC desc = {};
		desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
		desc.ByteWidth = myNumberOfParticles * sizeof(PARTICLE);
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		desc.StructureByteStride = sizeof(PARTICLE);
		desc.Usage = D3D11_USAGE_DEFAULT;

		PARTICLE* pData = new PARTICLE[myNumberOfParticles];
		//float fCenterSpread = g_fSpread * 0.50f;

		DirectX::XMVECTOR Center = myPos.r[3];
		DirectX::XMVECTOR vCenter = Center;

		for (int i = 0; i < myNumberOfParticles; i++)
		{
			DirectX::XMVECTOR vDelta = DirectX::XMVectorReplicate(myParticleSpread);

			/*	while (DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(vDelta)) > myParticleSpread * myParticleSpread)
			{*/
			//vDelta = DirectX::XMVectorSet(RPercent() * myParticleSpread, RPercent() * myParticleSpread, RPercent() * myParticleSpread, 0.f);
			vDelta = DirectX::XMVectorSet(DirectX::XMVectorGetX(Center), DirectX::XMVectorGetY(Center), DirectX::XMVectorGetZ(Center), 0.f);
			//}

			DirectX::XMVECTOR vPos = DirectX::XMVectorAdd(vCenter, vDelta);

			DirectX::XMStoreFloat3(reinterpret_cast<DirectX::XMFLOAT3*>(&pData[i].pos), vPos);
			pData[i].pos.w = (float)myNumberOfParticles * (float)myNumberOfParticles;
			pData[i].color = myColor;
			pData[i].size = mySize;
			pData[i].time = 0;
			pData[i].maxLifetime = 0;//myLifeTime * generateRandomFloat(0.1f, 1.f);
			pData[i].velo = myVelocity;
		}
		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = pData;


		hr = device->CreateBuffer(&desc, &InitData, myStructuredBufferOld.buffer.GetAddressOf());
		hr = device->CreateBuffer(&desc, &InitData, myStructuredBufferNext.buffer.GetAddressOf());

		ParticlesPerFrame* pData1 = new ParticlesPerFrame();
		pData1->totaltParticlesCreated = 0;

		InitData.pSysMem = pData1;

		// Define the buffer description
		//D3D11_BUFFER_DESC bufferDesc;
		//bufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
		//bufferDesc.ByteWidth = sizeof(ParticlesPerFrame);
		//bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		//bufferDesc.StructureByteStride = sizeof(ParticlesPerFrame);
		//bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		//bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		//bufferDesc.ByteWidth = sizeof(ParticlesPerFrame);
		//bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		//bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		//bufferDesc.MiscFlags = 0;
		//bufferDesc.StructureByteStride = sizeof(ParticlesPerFrame);

		D3D11_BUFFER_DESC descForGlobalParticleSeed = {};
		descForGlobalParticleSeed.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
		descForGlobalParticleSeed.ByteWidth = sizeof(ParticlesPerFrame);
		descForGlobalParticleSeed.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		descForGlobalParticleSeed.StructureByteStride = sizeof(ParticlesPerFrame);
		descForGlobalParticleSeed.Usage = D3D11_USAGE_DEFAULT;

		ParticleGlobalSeed* pDataSeedParticle = new ParticleGlobalSeed[1];
		pDataSeedParticle[0].globalSeed = 0;

		D3D11_SUBRESOURCE_DATA InitDataForGlobalSeed;
		InitDataForGlobalSeed.pSysMem = pDataSeedParticle;

		hr = device->CreateBuffer(&descForGlobalParticleSeed, &InitDataForGlobalSeed, myGlobalParticleSeed.buffer.GetAddressOf());

		D3D11_SHADER_RESOURCE_VIEW_DESC DescRVForGlobalParticleSeed = {};
		DescRVForGlobalParticleSeed.Format = DXGI_FORMAT_UNKNOWN;
		DescRVForGlobalParticleSeed.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		DescRVForGlobalParticleSeed.Buffer.FirstElement = 0;
		DescRVForGlobalParticleSeed.Buffer.NumElements = descForGlobalParticleSeed.ByteWidth / descForGlobalParticleSeed.StructureByteStride;

		hr = device->CreateShaderResourceView(myGlobalParticleSeed.buffer.Get(), &DescRVForGlobalParticleSeed, myGlobalParticleSeed.srv.GetAddressOf());

		D3D11_UNORDERED_ACCESS_VIEW_DESC DescUAVParticleSeed = {};
		DescUAVParticleSeed.Format = DXGI_FORMAT_UNKNOWN;
		DescUAVParticleSeed.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		DescUAVParticleSeed.Buffer.FirstElement = 0;
		DescUAVParticleSeed.Buffer.NumElements = descForGlobalParticleSeed.ByteWidth / descForGlobalParticleSeed.StructureByteStride;

		hr = device->CreateUnorderedAccessView(myGlobalParticleSeed.buffer.Get(), &DescUAVParticleSeed, myGlobalParticleSeed.uav.GetAddressOf());

		//hr = device->CreateBuffer(&bufferDesc, &InitData, &myTotalParticlesPerFrame.buffer);

		D3D11_SHADER_RESOURCE_VIEW_DESC DescRV = {};
		DescRV.Format = DXGI_FORMAT_UNKNOWN;
		DescRV.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		DescRV.Buffer.FirstElement = 0;
		DescRV.Buffer.NumElements = desc.ByteWidth / desc.StructureByteStride;
		hr = device->CreateShaderResourceView(myStructuredBufferOld.buffer.Get(), &DescRV, myStructuredBufferOld.srv.GetAddressOf());
		hr = device->CreateShaderResourceView(myStructuredBufferNext.buffer.Get(), &DescRV, myStructuredBufferNext.srv.GetAddressOf());

		//DescRV.Buffer.NumElements = bufferDesc.ByteWidth / bufferDesc.StructureByteStride;
		//hr = device->CreateShaderResourceView(myTotalParticlesPerFrame.buffer, &DescRV, &myTotalParticlesPerFrame.srv);

		D3D11_UNORDERED_ACCESS_VIEW_DESC DescUAV = {};
		DescUAV.Format = DXGI_FORMAT_UNKNOWN;
		DescUAV.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		DescUAV.Buffer.FirstElement = 0;
		DescUAV.Buffer.NumElements = desc.ByteWidth / desc.StructureByteStride;
		hr = device->CreateUnorderedAccessView(myStructuredBufferOld.buffer.Get(), &DescUAV, myStructuredBufferOld.uav.GetAddressOf());
		hr = device->CreateUnorderedAccessView(myStructuredBufferNext.buffer.Get(), &DescUAV, myStructuredBufferNext.uav.GetAddressOf());

		//DescUAV.Buffer.NumElements = bufferDesc.ByteWidth / bufferDesc.StructureByteStride;

		//hr = device->CreateUnorderedAccessView(myTotalParticlesPerFrame.buffer, &DescUAV, &myTotalParticlesPerFrame.uav);

		//D3D11_SUBRESOURCE_DATA InitData;
		//InitData.pSysMem = pData;

		D3D11_BUFFER_DESC Desc;
		Desc.Usage = D3D11_USAGE_DYNAMIC;
		Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		Desc.MiscFlags = 0;

		Desc.ByteWidth = sizeof(CB_CS);
		device->CreateBuffer(&Desc, nullptr, g_pcbCS.GetAddressOf());

		Desc.ByteWidth = sizeof(ParticleBuffer);
		hr = device->CreateBuffer(&Desc, nullptr, myParticleBuffer.GetAddressOf());
		break;
	}
	default:
		break;
	}
}
