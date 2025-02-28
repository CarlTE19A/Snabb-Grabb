#include "stdafx.h"

#include "Scene.h"
#include "gameObject/GameObject.h"
#include "component/mesh/MeshComponent.h"
#include "component/mesh/AnimatedMeshComponent.h"
#include "fileHandling/JsonSceneImporter.h"
#include "graphics/GraphicsEngine.h"
#include <thread>
#include <future>
//#include <algorithm>

#include "graphics\ui\UIHandler.h"

//#include "graphics\vfx\Flipbook.h"
//#include "graphics\vfx\ScrollMesh.h"
//#include "graphics\vfx\VFXElement.h"

#include "graphics\model\ModelDrawer.h"
#include "graphics\model\ModelShader.h"

#include "mainSingelton\MainSingelton.h"
#include "../game/PollingStation.h"
#include "events\inputMapper\InputMapper.h"
#include "tools/lineDrawer/LineDrawer.h"

#include "graphics/FullscreenEffect.h"
#include "component/PointLightComponent.h"
#include "component/SpotLightComponent.h"
#include <graphics\light\SpotLight.h>
#include "component/CameraComponent.h"
#include "../game/Player/PlayerComponent.h"
#include "../game/CardSystem/CardComponent.h"

#include "component\Particle\ParticleComponent.h"
#include "../game/Player/PlayerBulletComponent.h"

#include "../game/Ammo/AmmoComponent.h"
#include "../game/Ammo/AmmoPool.h"

#include "../game/Explosion/ExplosionComponent.h"
#include "../game/Explosion/ExplosionPool.h"

#include "Script-Base/Script/ScriptRuntimeInstance.h"
#include "Script-Base/Script/ScriptManager.h"
#include "Script-Base/Game/GameUpdateContext.h"
#include "settings/settings.h"
#include <filesystem>
#include "../game/PollingStation.h"
#include "../game/EnemyComponent/EnemyPool.h"
#include "../game/EnemyComponent/EnemyComponent.h"
#include "../game/GameManager/GameManager.h"
#include "../engine/sound/SoundManager.h"
#include "Paths.h"

#include "component/PhysicsComponent.h"
//Should be a method to play safely while loading multiple scens
//Is now to load all scenes at the same time so may be faster if there are many large scens
#define USE_THREADING false

struct SpeedLineData
{
	Vector3f direction;
	float speed;
};


Scene::Scene()
	:mySceneCamera(nullptr)
	, myPostProcessingIndex(0)
{
	myShouldRenderPostProcessing = false;
	myActive = false;
	myScript = nullptr;
	graphics = nullptr;

	//Done here as it has to be done before adding enemies to a scene
}

Scene::~Scene()
{
	//SaveScene(this);
	for (GameObject* gameObject : myGameObjects)
	{
		if (gameObject == nullptr)
		{
			LogWarning("Scene: " + mySceneName + " has a nullptr gameObject");
			continue;
		}
		else if (gameObject->GetTag() == "Enemy")
		{
			LogWarning("Scene: " + mySceneName + " is trying to delete enemy it is the EnemyPool job to do it");
			EnemyComponent* enemy = gameObject->GetComponent<EnemyComponent>();
			int enemyIndex = enemy->GetPoolIndex();
			MainSingleton::GetInstance().GetEnemyPool().ReturnEnemy(enemy->GetEnemyInfo(), enemyIndex);
		}
		else if (gameObject->GetTag() == "EnemyPart")
		{
			LogWarning("Scene: " + mySceneName + " is trying to delete enemy part it is the EnemyPool job to do it");
		}
		else if (gameObject->GetTag() == "Ammo")
		{
			LogWarning("Scene: " + mySceneName + " is trying to delete ammo it is the AmmoPool job to do it");
			/*AmmoComponent* ammo = gameObject->GetComponent<AmmoComponent>();
			if (ammo)
			{
				ammo->Reset
			}*/
		}
		else if (gameObject->GetTag() == "Explosion")
		{
			LogWarning("Scene: " + mySceneName + " is trying to delete explosion it is the ExplosionPool job to do it");
		}
		else
		{
			delete gameObject;
		}
	}
	myGameObjects.clear();
	myEnemies.clear();
	myAmmos.clear();
	myExplosions.clear();
	myIdMap.clear();

	//myParticle.Init(20);
	delete myScript;
	delete myShadowCamera;
	//delete myUIHandler;
	//myUIHandler = nullptr;
	graphics->SetDirectionalLight(nullptr);
}
#include "graphics\ui\UISprite.h"
void Scene::Init()
{
	graphics = GraphicsEngine::GetInstance();
	graphics->CreateBuffer(sizeof(SpeedLineData), mySpeedLinesBuffer);
	myShadowCamera = new GameObject();
	myShadowCamera->SetActive(true);
	myShadowCamera->AddComponent<CameraComponent>(new CameraComponent());
	mySoundManager = &MainSingleton::GetInstance().GetSoundManager();
	graphics->ReadPostprocessSettings(myPostProcessingIndex);
	mySoundManager->StopEvent(2);
	mySoundManager->PlayEvent(1);
	myShouldRenderPostProcessing = true;

	std::string tag = "";
	for (GameObject* gameObject : myGameObjects)
	{
		tag = gameObject->GetTag();
		//set player pointer
		if (tag == "MainCamera")
		{
			mySceneCamera = gameObject->GetComponent<CameraComponent>();
			MainSingleton::GetInstance().GetPollingStation().AddPlayer(gameObject->GetParent());
		}
		else if (tag == "Enemy")
		{
			myEnemies.push_back(gameObject);
		}
	}
	std::string scriptName = GetScriptName(mySceneName);
	std::shared_ptr<const Script> script = ScriptManager::GetScript(scriptName);

	myScript = new ScriptRuntimeInstance(script);
	myScript->Init();
	mySkyBox.Init(L"../assets/SkyboxBlankIdea.dds");

	myParticle.Init(10000);

}

void Scene::SetActive(bool isActive)
{
	myActive = isActive;
	if (isActive)
	{
		//GraphicsEngine::GetInstance()->SetCamera(mySceneCamera);
	}
}

void Scene::CreateCustomScene(std::string aSceneName, std::vector<GameObject*> someGameObjects)
{
	mySceneName = aSceneName;
	myGameObjects = someGameObjects;
}

void Scene::AddGameObject(GameObject* aGameObject)
{
	myGameObjects.push_back(aGameObject);
	myIdMap[aGameObject->GetID()] = aGameObject;

	if (aGameObject->GetComponent<MeshComponent>() != nullptr)
	{
		myMeshes.push_back(aGameObject->GetComponent<MeshComponent>());
	}
	else if (aGameObject->GetComponent<AnimatedMeshComponent>() != nullptr)
	{
		myAnimatedMeshes.push_back(aGameObject->GetComponent<AnimatedMeshComponent>());
	}

	for (GameObject* child : aGameObject->GetChildren())
	{
		AddGameObject(child);
	}
}

void Scene::AddAmmoObject(GameObject* aGameObject)
{
	myAmmos.push_back(aGameObject);
}

void Scene::AddExplosionObject(GameObject* aGameObject)
{
	myExplosions.push_back(aGameObject);
}

void Scene::AddNewCardObject(GameObject* aGameObject)
{
	myNewCards.push_back(aGameObject);
}

void Scene::AddMeshes()
{
	myMeshes.clear();
	myAnimatedMeshes.clear();
	for (GameObject* gameObject : myGameObjects)
	{
		if (gameObject->GetComponent<MeshComponent>() != nullptr)
		{
			std::string tag = gameObject->GetTag();
			if (tag == "Transparent" || tag == "DoorPart" || tag == "MainDoor")
			{
				myTransparentMeshes.push_back(gameObject->GetComponent<MeshComponent>());
			}
			else if (tag == "Scrollmesh")
			{
				myScrollMeshes.push_back(gameObject->GetComponent<MeshComponent>());
			}
			else if (tag == "Booster")
			{
				myBoosterMeshes.push_back(gameObject->GetComponent<MeshComponent>());
			}
			else
			{
				myMeshes.push_back(gameObject->GetComponent<MeshComponent>());
			}
		}
		else if (gameObject->GetComponent<AnimatedMeshComponent>() != nullptr)
		{
			myAnimatedMeshes.push_back(gameObject->GetComponent<AnimatedMeshComponent>());
		}
	}
}

void Scene::RemoveGameObject(GameObject* aGameObject)
{
	for (GameObject* child : aGameObject->GetChildren())
	{
		RemoveGameObject(child);
	}

	//TODO SOLVE GIVES ERRORS, should not be deleted here right?

	//May still give errors?
	MeshComponent* mesh = aGameObject->GetComponent<MeshComponent>();
	AnimatedMeshComponent* animMesh = aGameObject->GetComponent<AnimatedMeshComponent>();
	if (mesh != nullptr)
	{
		auto it = std::find(myMeshes.begin(), myMeshes.end(), mesh);
		if (it != myMeshes.end())
		{
			myMeshes.erase(it);
		}
	}
	else if (animMesh != nullptr)
	{
		auto it = std::find(myAnimatedMeshes.begin(), myAnimatedMeshes.end(), animMesh);
		if (it != myAnimatedMeshes.end())
		{
			myAnimatedMeshes.erase(it);
		}
	}

	for (int i = 0; i < myGameObjects.size(); i++)
	{
		myIdMap.erase(myGameObjects[i]->GetID());
		if (myGameObjects[i] == aGameObject)
		{
			myGameObjects.erase(myGameObjects.begin() + i);
			return;
		}
	}
}

void Scene::Update(float aDeltaTime)
{

	if (!myActive)
	{
		return;
	}

	int index = 0;
	myListAmmoRemove.clear();
	myListExplosionRemove.clear();
	for (GameObject* gameObject : myGameObjects)
	{

		if (gameObject == nullptr)
		{
			LogError("gameObject is NullPtr");
			continue;
		}
		gameObject->Update(aDeltaTime, *this);
		if (gameObject->GetComponent<PointLightComponent>() != nullptr)
		{
			pointLights.push_back(gameObject->GetComponent<PointLightComponent>());
		}
		else if (gameObject->GetComponent<SpotLightComponent>() != nullptr)
		{
			spotLights.push_back(gameObject->GetComponent<SpotLightComponent>());
		}
		else if (gameObject->GetComponent<PlayerComponent>() != nullptr)
		{
			//update SpeedLines buffer
			PlayerComponent* player = gameObject->GetComponent<PlayerComponent>();
			auto* graphicsEngine = GraphicsEngine::GetInstance();
			auto context = graphicsEngine->GetContext();

			SpeedLineData speedLinesData = {};
			speedLinesData.direction = /*player->ConvertToWorldDirection*/(player->GetAppliedVelocity());
			speedLinesData.speed = player->GetAppliedVelocity().Length();

			D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
			context->Map(mySpeedLinesBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
			memcpy(mappedBuffer.pData, &speedLinesData, sizeof(SpeedLineData));

			context->Unmap(mySpeedLinesBuffer.Get(), 0);

			context->VSSetConstantBuffers(9, 1, mySpeedLinesBuffer.GetAddressOf());
			context->PSSetConstantBuffers(9, 1, mySpeedLinesBuffer.GetAddressOf());


		}
		else if (gameObject->GetComponent<AmmoComponent>() != nullptr)
		{
			if (gameObject->GetComponent<AmmoComponent>()->IsItExpired())
			{
				SafeRemoveAmmo removeAmmo{};
				removeAmmo.myAmmo = gameObject;
				//removeAmmo.myIndexOfGameObject = index;
				myListAmmoRemove.push_back(removeAmmo);
			}
		}
		else if (gameObject->GetComponent<ExplosionComponent>() != nullptr)
		{
			if (gameObject->GetComponent<ExplosionComponent>()->IsItExpired())
			{
				SafeRemoveExpolision removeExplosion;
				removeExplosion.myExplosion = gameObject;
				//removeExplosion.myIndexOfGameObject = index;
				myListExplosionRemove.push_back(removeExplosion);

			}
		}
		++index;
	}


	RemoveOrAddAmmoInTheGameObject();
	RemoveOrAddExplosionInTheGameObject();

	/*for (int i = 0; i < myGameObjects.size(); i++)
	{
		myGameObjects[i]->Update(aDeltaTime, *this);
		if (myGameObjects[i]->GetComponent<PointLightComponent>() != nullptr)
		{
			pointLights.push_back(myGameObjects[i]->GetComponent<PointLightComponent>());
		}
		else if (myGameObjects[i]->GetComponent<SpotLightComponent>() != nullptr)
		{
			spotLights.push_back(myGameObjects[i]->GetComponent<SpotLightComponent>());
		}
	}*/

	ScriptUpdateContext info{};
	info.deltaTime = aDeltaTime;
	info.myScene = this;
	myScript->Update(info);
	graphics->UpdateClouds();
	graphics->UpdateFog();

	//myParticle1.Update();
//#ifdef _DEBUG
//
//	if (ImGui::Begin("ScriptEditor"))
//	{
//
//		if (ImGui::Button("Play", ImVec2(120, 0)))
//		{
//			std::string scriptName = GetScriptName(mySceneName);
//			std::shared_ptr<const Script> script = ScriptManager::GetScript(scriptName);
//			myScript->ReadAgain(script);
//		}
//	}
//	ImGui::End();
//
//#endif // DEBUG

}

void Scene::Render()
{
	if (!myActive)
	{
		return;
	}
	auto& modelDrawer = graphics->GetModelDrawer();
	modelDrawer.DrawBatch(myMeshes, modelDrawer.GetPbrShader());

	for (AnimatedMeshComponent* animMesh : myAnimatedMeshes)
	{
		if (!animMesh->IsActive() || !animMesh->GetOwner()->IsActive())
		{
			continue;
		}
		modelDrawer.Draw(*animMesh, modelDrawer.GetPbrAnimatedShader());
	}

}

void Scene::RenderDeferred()
{
	if (!myActive)
	{
		return;
	}
	ID3D11ShaderResourceView* nullSRV = nullptr;
	graphics->GetContext()->PSSetShaderResources(1, 1, &nullSRV);
	graphics->GetContext()->PSSetShaderResources(8, 1, &nullSRV);

	ShadowMapDirectionalLight();
	//ShadowMapSpotLight();
	graphics->GetGBuffer().ClearAllAsResources(10);
	graphics->GetGBuffer().ClearTextures();
	graphics->GetGBuffer().SetAsActiveTarget(graphics->GetDepthBuffer());
	graphics->SetBlendState(BlendState::Disabled);
	graphics->UpdateGPUStates();
	auto& modelDrawer = graphics->GetModelDrawer();
	//CameraComponent* activeCamera = graphics->GetCamera();

	modelDrawer.DrawBatch(myMeshes, modelDrawer.GetGBufferShader());

	for (AnimatedMeshComponent* animMesh : myAnimatedMeshes)
	{
		if (!animMesh->IsActive() || !animMesh->GetOwner()->IsActive())
		{
			continue;
		}

		modelDrawer.Draw(*animMesh, modelDrawer.GetGBufferAnimShader());
	}

	mySkyBox.Render();

	if (myShouldRenderPostProcessing)
	{
		graphics->GetBloomRenderTargets()[0].SetAsActiveTarget();
	}
	else
	{
		graphics->SetEngineBackBufferAsActiveTarget(false);
	}

	graphics->GetShadowMap().SetAsResourceOnSlot(8);
	graphics->GetGBuffer().SetAllAsResources(10);
	graphics->GetFullscreenDeferred().Render();

	graphics->SetBlendState(BlendState::AdditiveBlend);
	graphics->SetRasterizerState(RasterizerState::FrontfaceCulling);
	graphics->SetDepthStencilState(DepthStencilState::ReadOnlyGreater);
	graphics->UpdateGPUStates();

	for (PointLightComponent* point : pointLights)
	{
		point->Render();
	}

	for (SpotLightComponent* spot : spotLights)
	{
		spot->Render();
	}

	pointLights.clear();
	spotLights.clear();

	graphics->SetBlendState(BlendState::Disabled);
	graphics->SetDepthStencilState(DepthStencilState::Write);
	graphics->SetRasterizerState(RasterizerState::BackfaceCulling);
	graphics->UpdateGPUStates();

	if (myShouldRenderPostProcessing)
	{
		//graphics->GetBloomRenderTargets()[0].SetAsActiveTarget(nullptr);
		graphics->GetBloomRenderTargets()[0].SetAsResourceOnSlot(1);
		//graphics->GetFullScreenEffectCopy().Render();
		RunBloomPass();
	}

	graphics->RenderVolumetricFog();
	graphics->RenderVolumetricClouds();

	graphics->UseDepth(true);
	graphics->SetBlendState(BlendState::AlphaBlend);
	graphics->UpdateGPUStates();
	modelDrawer.DrawBatch(myTransparentMeshes, modelDrawer.GetPbrShader());

	graphics->SetRasterizerState(RasterizerState::NoFaceCulling);
	graphics->UpdateGPUStates();
	modelDrawer.DrawBatch(myScrollMeshes, modelDrawer.GetScrollMeshShader());
	graphics->SetRasterizerState(RasterizerState::BackfaceCulling);
	graphics->UpdateGPUStates();
	modelDrawer.DrawBatch(myBoosterMeshes, modelDrawer.GetBoosterMeshShader());

	graphics->SetBlendState(BlendState::Disabled);
	graphics->UpdateGPUStates();

	for (int i = 0; i < MainSingleton::GetInstance().GetPollingStation().GetAmmoPool()->GetAmmoPool().size(); i++)
	{
		MainSingleton::GetInstance().GetPollingStation().GetAmmoPool()->GetAmmoPool()[i].myTrailEffect->RenderParticles();
	}

	for (int i = 0; i < myGameObjects.size(); i++)
	{
		if (myGameObjects[i] != nullptr && myGameObjects[i]->GetComponent<ParticleComponent>() != nullptr)
		{
			myGameObjects[i]->GetComponent<ParticleComponent>()->RenderParticles();
		}
	}

	myParticle.Update();
	myParticle.EmitParticles();

#ifdef _DEBUG
	graphics->SetEngineBackBufferAsActiveTarget(false);
	MainSingleton::GetInstance().GetLineDrawer().DrawAllLines();
#endif

}

void Scene::Reset()
{
	int index = 0;
	std::vector<GameObject*> objcetsToDelete;
	for (GameObject* gameObject : myGameObjects)
	{
		gameObject->Reset();

		if (gameObject->GetComponent<AmmoComponent>() != nullptr)
		{
			SafeRemoveAmmo removeAmmo{};
			removeAmmo.myAmmo = gameObject;
			//removeAmmo.myIndexOfGameObject = index;
			myListAmmoRemove.push_back(removeAmmo);
		}
		if (gameObject->GetComponent<ExplosionComponent>() != nullptr)
		{
			SafeRemoveExpolision removeExplosion;
			removeExplosion.myExplosion = gameObject;
			//removeExplosion.myIndexOfGameObject = index;
			myListExplosionRemove.push_back(removeExplosion);
		}


		CardComponent* cardComponent = gameObject->GetComponent<CardComponent>();
		if (cardComponent != nullptr)
		{
			if (cardComponent->IAmNewCard())
			{
				objcetsToDelete.push_back(gameObject);
			}

		}

		++index;
	}
	RemoveOrAddAmmoInTheGameObject();
	RemoveOrAddExplosionInTheGameObject();
	for (size_t i = 0; i < objcetsToDelete.size(); i++)
	{
		if (objcetsToDelete[i]->GetComponent<MeshComponent>() != nullptr)
		{
			std::vector<MeshComponent*>::iterator it = std::find(myMeshes.begin(), myMeshes.end(), objcetsToDelete[i]->GetComponent<MeshComponent>());
			if (it != myMeshes.end())
			{
				myMeshes.erase(it);
			}
		}

		if (objcetsToDelete[i]->GetComponent<PhysicsComponent>() != nullptr)
		{
			PhysXManager& physXManager = MainSingleton::GetInstance().GetPhysXManager();
			physXManager.RemoveActor(*objcetsToDelete[i]->GetComponent<PhysicsComponent>()->GetActor());
		}

		RemoveGameObject(objcetsToDelete[i]);
		delete objcetsToDelete[i];
		objcetsToDelete[i] = nullptr;
	}



	myListAmmoRemove.clear();
	myListExplosionRemove.clear();
}

void Scene::RecieveEvent(const eInputEvent aEvent, std::any aValue)
{
	if (!myActive) return;
	aEvent;
	aValue;
}

const std::vector<GameObject*>& Scene::GetGameObjectList() const
{
	return myGameObjects;
}

const int Scene::GetEnemyCount() const
{
	int enemiesAlive = 0;
	for (int i = 0; i < myEnemies.size(); i++)
	{
		if (myEnemies[i]->GetComponent<EnemyComponent>()->IsAlive())
			++enemiesAlive;
	}
	return enemiesAlive;
}

void Scene::ReadNewScript()
{
	std::string scriptName = GetScriptName(mySceneName);
	std::shared_ptr<const Script> script = ScriptManager::GetScript(scriptName);
	myScript->ReadAgain(script);
}

SoundManager* Scene::GetSoundManager()
{
	return mySoundManager;
}

void Scene::ShadowMapDirectionalLight()
{
	DirectionalLight* dirLight = graphics->GetDirectionalLight();
	if (dirLight != nullptr)
	{
		float shadowMapScale = 5000;
		myShadowCamera->SetTransform(dirLight->GetTransform());
		if (graphics->GetCamera() != nullptr)
		{
			Vector3f playerPos = graphics->GetCamera()->GetOwner()->GetPosition();
			playerPos.y += 500.f;
			playerPos.z -= 500.f;
			dirLight->SetPosition(playerPos);
			myShadowCamera->SetTransform(dirLight->GetTransform());
			Vector3f shadowPos;
			{
				// round shadow map camera position to texel size
				// this makes it jitter less when moving the camera
				Transform shadowTransform = *myShadowCamera->GetTransform();
				Vector3f forward = shadowTransform.GetForward();
				Vector3f right = shadowTransform.GetRight();
				Vector3f up = shadowTransform.GetUp();
				float f = playerPos.Dot(forward);
				float r = playerPos.Dot(right);
				float u = playerPos.Dot(up);

				float texelWorldSize = (2.f * shadowMapScale / 8192); //TODO: double check values
				r = texelWorldSize * std::round(r / texelWorldSize);
				u = texelWorldSize * std::round(u / texelWorldSize);

				shadowPos = f * forward + r * right + u * up;
			}
			myShadowCamera->SetPosition(shadowPos);
		}
		myShadowCamera->GetComponent<CameraComponent>()->SetOrtographicProjection(-shadowMapScale, shadowMapScale, -shadowMapScale, shadowMapScale, 5000, -5000); //TODO: double check values
		dirLight->SetLightProjectionMatrix(myShadowCamera->GetComponent<CameraComponent>()->GetProjectionMatrix());
		CameraComponent* prevCamera = graphics->GetCamera();
		graphics->SetCamera(myShadowCamera->GetComponent<CameraComponent>());
		graphics->SetBlendState(BlendState::Disabled);
		graphics->UpdateGPUStates();

		//graphics->GetShadowMap().SetAsActiveTarget(0);
		graphics->GetShadowMap().SetAsActiveTarget(graphics->GetDirectionalLight()->myViewPortIndex);
		auto& modelDrawer = graphics->GetModelDrawer();
		modelDrawer.DrawBatch(myMeshes, modelDrawer.GetShadowShader());

		for (AnimatedMeshComponent* animMesh : myAnimatedMeshes)
		{
			if (!animMesh->IsActive() || !animMesh->GetOwner()->IsActive())
			{
				continue;
			}
			modelDrawer.Draw(*animMesh, modelDrawer.GetAnimatedShadowShader());
		}

		graphics->SetEngineBackBufferAsActiveTarget(true);
		graphics->SetCamera(prevCamera);
		graphics->UpdateGPUStates();
	}
}

void Scene::ShadowMapSpotLight()
{
	CameraComponent* prevCamera = graphics->GetCamera();
	auto& modelDrawer = graphics->GetModelDrawer();
	float fov = 0;
	float resolution = 512;
	float nearplane = 1.f;
	//graphics->SetDepthStencilState(DepthStencilState::ReadOnlyGreater);
	//graphics->SetRasterizerState(RasterizerState::FrontfaceCulling);
	//graphics->UpdateGPUStates();
	for (int i = 0; i < spotLights.size(); ++i)
	{
		if (spotLights[i]->GetSpotLight().myViewPortIndex < 0) continue;
		myShadowCamera->SetTransform(spotLights[i]->GetOwner()->GetTransform());
		//myShadowCamera->GetTransform()->RotateAroundAxis(myShadowCamera->GetTransform()->GetUp(), 180.f);
		fov = spotLights[i]->GetSpotLight().GetOuterLimit();
		myShadowCamera->GetComponent<CameraComponent>()->SetPerspectiveProjection(fov, resolution, resolution, nearplane, spotLights[i]->GetSpotLight().GetRange() * 2.f);
		spotLights[i]->GetSpotLight().SetLightProjectionMatrix(myShadowCamera->GetComponent<CameraComponent>()->GetProjectionMatrix());
		graphics->SetCamera(myShadowCamera->GetComponent<CameraComponent>());
		graphics->GetShadowMap().SetAsActiveTarget(spotLights[i]->GetSpotLight().myViewPortIndex);

		modelDrawer.DrawBatch(myMeshes, modelDrawer.GetShadowShader());

		for (AnimatedMeshComponent* animMesh : myAnimatedMeshes)
		{
			if (!animMesh->IsActive() || !animMesh->GetOwner()->IsActive())
			{
				continue;
			}
			modelDrawer.Draw(*animMesh, modelDrawer.GetAnimatedShadowShader());
		}
	}
	//graphics->SetDepthStencilState(DepthStencilState::Write);
	//graphics->SetRasterizerState(RasterizerState::BackfaceCulling);
	//graphics->SetEngineBackBufferAsActiveTarget(true);
	graphics->SetCamera(prevCamera);
	graphics->UpdateGPUStates();
}

void Scene::RemoveOrAddAmmoInTheGameObject()
{
	//add object to the vector
	if (!myAmmos.empty())
	{
		for (int i = 0; i < myAmmos.size(); i++)
		{
			AddGameObject(myAmmos[i]);
		}
		myAmmos.clear();
	}
	if (!myNewCards.empty())
	{
		for (int i = 0; i < myNewCards.size(); i++)
		{
			AddGameObject(myNewCards[i]);
		}
		myNewCards.clear();
	}

	if (!myListAmmoRemove.empty())
	{
		//std::sort(myListAmmoRemove.begin(), myListAmmoRemove.end(), std::greater{});

		for (int i = (int)myListAmmoRemove.size() - 1; i >= 0; --i)
		{
			//AmmoComponent& ammo = *myGameObjects[myListAmmoRemove[i].myIndexOfGameObject]->GetComponent<AmmoComponent>();
			AmmoComponent& ammo = *myListAmmoRemove[i].myAmmo->GetComponent<AmmoComponent>();
			ammo.GetOwnerPool()->ReturnAmmo(ammo.GetPoolIndex());
			RemoveGameObject(myListAmmoRemove[i].myAmmo);
			//myGameObjects.erase(myGameObjects.begin() + myListAmmoRemove[i].myIndexOfGameObject);

			for (int j = (int)myMeshes.size() - 1; j >= 0; --j)
			{
				if (myMeshes[j]->GetOwner() == myListAmmoRemove[i].myAmmo)
				{
					myMeshes.erase(myMeshes.begin() + j);
					break;
				}
			}
		}
		myListAmmoRemove.clear();
	}
}

void Scene::RemoveOrAddExplosionInTheGameObject()
{
	//add object to the vector
	if (!myExplosions.empty())
	{
		for (int i = 0; i < myExplosions.size(); i++)
		{
			AddGameObject(myExplosions[i]);
		}
		myExplosions.clear();
	}

	if (!myListExplosionRemove.empty())
	{
		for (int i = (int)myListExplosionRemove.size() - 1; i >= 0; --i)
		{
			//ExplosionComponent& explosion = *myGameObjects[myListExplosionRemove[i].myIndexOfGameObject]->GetComponent<ExplosionComponent>();
			ExplosionComponent& explosion = *myListExplosionRemove[i].myExplosion->GetComponent<ExplosionComponent>();
			explosion.GetOwnerPool()->ReturnExplosion(explosion.GetPoolIndex());
			//myGameObjects.erase(myGameObjects.begin() + myListExplosionRemove[i].myIndexOfGameObject);
			RemoveGameObject(myListExplosionRemove[i].myExplosion);

			for (int j = (int)myMeshes.size() - 1; j >= 0; --j)
			{
				if (myMeshes[j]->GetOwner() == myListExplosionRemove[i].myExplosion)
				{
					myMeshes.erase(myMeshes.begin() + j);
					break;
				}
			}
			//for (int j = (int)myAnimatedMeshes.size() - 1; j >= 0; --j)
			//{
			//	if (myAnimatedMeshes[j]->GetOwner() == myListExplosionRemove[i].myExplosion)
			//	{
			//		myAnimatedMeshes.erase(myAnimatedMeshes.begin() + j);
			//		break;
			//	}
			//}
		}
		myListExplosionRemove.clear();
	}
}

std::string Scene::GetScriptName(const std::string& /*aSceneName*/)
{
	std::filesystem::path ScriptPath = STATIC_ASSET_JSON_SCRIPT_PATH;
	for (const auto& entry : std::filesystem::directory_iterator(ScriptPath))
	{
		std::string filename = entry.path().filename().string();
		if (filename.find(".json") != std::string::npos)
		{
			filename = filename.substr(0, filename.find_last_of("."));
			return filename;
		}
	}
	return "";
}

void Scene::RunBloomPass()
{
	graphics->SetBlendState(BlendState::Disabled);
	graphics->SetDepthStencilState(DepthStencilState::Write);
	graphics->SetRasterizerState(RasterizerState::BackfaceCulling);
	graphics->UpdateGPUStates();
	ID3D11ShaderResourceView* const nullSRV = nullptr;
	for (int i = 1; i < graphics->GetBloomRenderTargets().size(); i++)
	{
		graphics->GetBloomRenderTargets()[i].SetAsActiveTarget(nullptr);
		graphics->GetBloomRenderTargets()[i - 1].SetAsResourceOnSlot(1);
		graphics->GetFullScreenEffectDownSample().Render();
	}

	graphics->SetBlendState(BlendState::AlphaBlend);
	graphics->UpdateGPUStates();

	for (int i = (int)graphics->GetBloomRenderTargets().size() - 2; i >= 0; i--)
	{
		graphics->GetContext()->PSSetShaderResources(1, 1, &nullSRV);
		graphics->GetBloomRenderTargets()[i].SetAsActiveTarget(nullptr);
		graphics->GetBloomRenderTargets()[i + 1].SetAsResourceOnSlot(1);
		graphics->GetFullScreenEffectUpSampleAndScaleBloom().Render();
	}

	graphics->SetBlendState(BlendState::Disabled);
	graphics->UpdateGPUStates();
	//graphics->SetEngineBackBufferAsActiveTarget(false);
	graphics->GetComicShaderRenderTarget().SetAsActiveTarget();
	graphics->GetBloomRenderTargets()[0].SetAsResourceOnSlot(1);
	graphics->GetFullScreenPostprocess().Render();

	graphics->SetEngineBackBufferAsActiveTarget(false);
	graphics->GetComicShaderRenderTarget().SetAsResourceOnSlot(1);
	graphics->GetFullscreenComic().Render();
}

