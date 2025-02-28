#pragma once
#include "nlohmann/json.hpp"
#include "scene/Scene.h"

class GameObject;
class Action;

//Should maybe be a static class?
class JsonSceneImporter
{
public:
	JsonSceneImporter() {};
	~JsonSceneImporter() {};

	Scene* ImportScene(const char* filePath);
private:
	void ImportSceneData(			nlohmann::json& someSceneJson,				Scene* aScene);
	void ImportGameObjects(			nlohmann::json& someGameObjectsJson,		Scene* aScene);
	void ImportTransform(			nlohmann::json& someTransformJson,			Scene* aScene);
	//void ImportMatrices(			nlohmann::json& someMatriciesJson,			nlohmann::json& someOverrideMatriciesJson,			Scene* aScene);
	void ImportMeshComponent(		nlohmann::json& someMeshComponentJson,		Scene* aScene);
	void ImportSkinnedMeshComponent(		nlohmann::json& someMeshComponentJson,		Scene* aScene);
	void ImportCameraComponent(		nlohmann::json& someCameraComponentJson,	Scene* aScene);
	void ImportLightComponent(		nlohmann::json& someLightComponentJson,		Scene* aScene);
	//void ImportBoxColliderComponent(nlohmann::json& someBoxComponentJson,		Scene* aScene);
	void ImportMeshColliderComponent(nlohmann::json& someMeshComponentJson,		Scene* aScene);
	void ImportTriggerZoneComponent(nlohmann::json& someTriggerZoneComponentJson,	Scene* aScene);
	void ImportActions(				nlohmann::json& someActionsJson,				Scene* aScene);
	

	void ImportCardComponent(		nlohmann::json& someCardComponentJson,		Scene* aScene);
	void ImportEnemyComponent(nlohmann::json& someEnemyComponentJson, Scene* aScene);
	void ImportDoorGameObject(nlohmann::json& someDoorJson, Scene* aScene);
	void ImportRotatorComponent(nlohmann::json& someRotatorComponentJson, Scene* aScene);




	//Actions are not components, owned by trigger zones, all imported in importActions
	//Returns the instanceID of the action
	int ImportAction(nlohmann::json& someActionJson, Scene* aScene, Action* aAction);
	void ImportActionKeys(nlohmann::json& someActionKeysJson, Scene* aScene, Action* aAction);

	void ImportUnlockTriggerZoneAction(nlohmann::json& someUnlockTriggerZoneActionJson, Scene* aScene);
	void ImportMoveTransformAction(nlohmann::json& someMoveTransformActionJson, Scene* aScene);
	void ImportRotateTransformAction(nlohmann::json& someRotateTransformActionJson, Scene* aScene);
	void ImportPlaySoundAction(nlohmann::json& somePlaySoundActionJson, Scene* aScene);
	void ImportLevelTransitionAction(nlohmann::json& someLevelTransitionActionJson, Scene* aScene);
	void ImportDelayAction(nlohmann::json& someDelayActionJson, Scene* aScene);
	void ImportToggleComponentAction(nlohmann::json& someToggleComponentAction, Scene* aScene);
	Vector3f ImportVector3fX100(nlohmann::json& someVector3);
	void ImportPlayer(nlohmann::json& somePlayerJson, Scene* aScene);
	//void ImportGoal(nlohmann::json& someGoalJson, Scene* aScene);
};