#pragma once

#include "events/inputMapper/InputObserver.h"
#include <graphics\RenderTarget.h>
#include "graphics/DepthBuffer.h"
#include <map>
#include <component\mesh\AnimatedMeshComponent.h>
#include <component\mesh\MeshComponent.h>
#include <graphics\SkyBox.h>
#include "ParticleSystem\ParticleSystem.h"

#include "graphics\Text\Text.h"

class UIHandler;

class NavMesh;
class CameraComponent;
class GameObject;
//class Flipbook;
//class ScrollMesh;
//class VFXElement;
class PointLightComponent;
class SpotLightComponent;
class ScriptRuntimeInstance;
class GraphicsEngine;
class AmmoPool;
class SoundManager;


struct SafeRemoveAmmo
{
	//int myIndexOfGameObject;
	GameObject* myAmmo;
};
struct SafeRemoveExpolision
{
	//int myIndexOfGameObject;
	GameObject* myExplosion;
};
class Scene : public InputObserver
{
	friend class SceneManager;
public:
	Scene();
	~Scene();

	void Init();
	void SetActive(bool isActive);
	void CreateCustomScene(std::string aSceneName, std::vector<GameObject*> someGameObjects = {});
	void AddGameObject(GameObject* aGameObject);
	void AddAmmoObject(GameObject* aGameObject);
	void AddExplosionObject(GameObject* aGameObject);
	void AddNewCardObject(GameObject* aGameObject);
	void AddMeshes();
	void RemoveGameObject(GameObject* aGameObject);

	void Update(float aDeltaTime);
	void Render();
	void RenderDeferred();
	void Reset();

	void RecieveEvent(const eInputEvent aEvent, std::any aValue) override;

	bool GetActive() { return myActive; }

	std::string GetSceneName() { return mySceneName; }
	const std::vector<GameObject*>& GetGameObjectList()const;
	const std::vector<GameObject*>& GetEnemies() const { return myEnemies; }
	const int GetEnemyCount() const;

	ParticleSystem& GetParticle() { return myParticle; }

	void ReadNewScript();

	int GetSceneID() { return mySceneID; }

	SoundManager* GetSoundManager();

private:
	void ShadowMapDirectionalLight();
	void ShadowMapSpotLight();
	void RemoveOrAddAmmoInTheGameObject();
	void RemoveOrAddExplosionInTheGameObject();
	std::string GetScriptName(const std::string& aSceneName);
	void RunBloomPass();
	
	GraphicsEngine* graphics;
	std::vector<GameObject*> myGameObjects;
	std::vector<GameObject*> myEnemies;
	std::vector<GameObject*> myAmmos;
	std::vector<GameObject*> myNewCards;
	std::vector<GameObject*> myExplosions;
	std::vector<SafeRemoveAmmo>myListAmmoRemove;
	std::vector<SafeRemoveExpolision>myListExplosionRemove;
	std::map<int, GameObject*> myIdMap;
	std::string mySceneName;
	std::string myScenePath;
	int mySceneID;

	std::vector<AnimatedMeshComponent*> myAnimatedMeshes;
	std::vector<MeshComponent*> myMeshes;
	std::vector<MeshComponent*> myScrollMeshes;
	std::vector<MeshComponent*> myBoosterMeshes;
	std::vector<MeshComponent*> myTransparentMeshes;
	std::vector<PointLightComponent*> pointLights;
	std::vector<SpotLightComponent*> spotLights;

	ParticleSystem myParticle;
	//ParticleSystem myParticle1;

	//UIHandler* myUIHandler = nullptr;

	CameraComponent* mySceneCamera;

	bool myActive;
	bool myShouldRenderPostProcessing;

	char* myPostProcessingPath;
	int myPostProcessingIndex;

	friend class EditorState;
	friend class JsonSceneImporter;
	//Will do nothing if not in debug
	friend class JsonSceneExporter;
	ScriptRuntimeInstance* myScript;
	ComPtr<ID3D11Buffer> mySpeedLinesBuffer;

	SkyBox mySkyBox;

	SoundManager* mySoundManager;

	GameObject* myShadowCamera;
};