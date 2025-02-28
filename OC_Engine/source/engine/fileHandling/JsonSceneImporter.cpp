#include "stdafx.h"
#include "Paths.h"

#include "scene/Scene.h"
#include "JsonSceneImporter.h"
#include <fstream>
//GameObjects
#include "gameObject/GameObject.h"

//Components
#include "component/mesh/MeshComponent.h"
#include "component/mesh/AnimatedMeshComponent.h"
#include "component/CameraComponent.h"
#include "component/DirectionalLightComponent.h"
#include "component/SpotLightComponent.h"
#include "component/PointLightComponent.h"
#include "component\PhysicsComponent.h"
#include "component/RotatorComponent.h"

#include "component\TriggerZone\triggerZoneComponent.h"

#include "graphics\light\DirectionalLight.h"
#include "graphics\light\SpotLight.h"
#include "graphics\light\PointLight.h"

#include "../game/Player/PlayerComponent.h"

//Actions
#include "component\TriggerZone\Actions\Action.h"
#include "component\TriggerZone\Actions\DelayAction.h"
#include "component\TriggerZone\Actions\UnlockTriggerZoneAction.h"
#include "component\TriggerZone\Actions\MoveTransformAction.h"
#include "component\TriggerZone\Actions\RotateTransformAction.h"
#include "component\TriggerZone\Actions\PlaySoundAction.h"
#include "component\TriggerZone\Actions\LevelTransitionAction.h"
#include "component\TriggerZone\Actions\ToggleTriggerZoneAction.h"

#include "mainSingelton\MainSingelton.h"

//Tools
#include "graphics/model/ModelFactory.h"

#include "../game/CardSystem/CardComponent.h"

#include "../game/EnemyComponent/EnemyInfo.h"
#include "../game/EnemyComponent/EnemyPool.h"
#include "../game/EnemyComponent/EnemyComponent.h"

#include "PhysXCode/PhysXManager.h"

#include "../game/Door/Door.h"
#include "../game/EnemyComponent/BalloonComponent.h"
#include "../game/EnemyComponent/TurretComponent.h"
using namespace nlohmann;

Scene* JsonSceneImporter::ImportScene(const char* filePath)
{
	//Prepare main file
	std::ifstream file(filePath);
	assert(file.is_open() && "Could not open file");
	json levelJson = json::parse(file);
	file.close();
	GraphicsEngine::GetInstance()->ClearShadowAtlas();
	Scene* scene = new Scene();

	std::string navMeshPath = filePath;
	navMeshPath = navMeshPath.substr(0, navMeshPath.size() - 5);
	navMeshPath += "_NavMesh.crackobj";
	scene->myScenePath = filePath;
	ImportSceneData(levelJson, scene);
	ImportGameObjects(levelJson["gameObjects"], scene);
	ImportTransform(levelJson["transforms"], scene);
	ImportMeshComponent(levelJson["staticMeshes"], scene);
	ImportSkinnedMeshComponent(levelJson["skinnedMeshes"], scene);
	ImportCameraComponent(levelJson["cameras"], scene);
	ImportLightComponent(levelJson["lights"], scene);
	ImportMeshColliderComponent(levelJson["meshColliders"], scene);
	//ImportBoxColliderComponent(levelJson["boxColliders"], scene);
	ImportPlayer(levelJson, scene);

	ImportCardComponent(levelJson["cardComponents"], scene);

	ImportEnemyComponent(levelJson["enemyComponent"], scene);
	ImportTriggerZoneComponent(levelJson["triggerZones"], scene);
	ImportDoorGameObject(levelJson["doorComponent"], scene);
	ImportRotatorComponent(levelJson["rotators"], scene);
	ImportActions(levelJson, scene);

	for (GameObject* go : scene->myGameObjects)
	{
		go->InitReset();
	}

	scene->AddMeshes();

	return scene;
}

void JsonSceneImporter::ImportSceneData(nlohmann::json& someSceneJson, Scene* aScene)
{
	aScene->mySceneName = someSceneJson["sceneName"];
	aScene->mySceneID = someSceneJson["sceneBuildIndex"];
	try
	{
		aScene->myPostProcessingIndex = someSceneJson["spawnPlayer"]["postprocessIndex"] -1;
	}
	catch (std::exception&)
	{
		aScene->myPostProcessingIndex = 0;
	}
}

void JsonSceneImporter::ImportGameObjects(nlohmann::json& someGameObjectsJson, Scene* aScene)
{
	for (int i = 0; i < someGameObjectsJson.size(); i++)
	{
		int id = someGameObjectsJson[i]["instanceID"];
		aScene->myGameObjects.push_back(new GameObject(id, someGameObjectsJson[i]["active"]));
		GameObject* gameObject;
		gameObject = aScene->myGameObjects[i];

		aScene->myIdMap[gameObject->myID] = gameObject;

		gameObject->myName = someGameObjectsJson[i]["name"];
		gameObject->myTag = someGameObjectsJson[i]["tag"];
		gameObject->myLayer = someGameObjectsJson[i]["layer"];
		gameObject->parentID = someGameObjectsJson[i]["parentID"];
	}

	for (int i = 0; i < aScene->myGameObjects.size(); i++)
	{
		if (aScene->myGameObjects[i]->parentID != 0)
		{
			aScene->myGameObjects[i]->myParent = aScene->myIdMap[aScene->myGameObjects[i]->parentID];
			aScene->myGameObjects[i]->myParent->myChildren.push_back(aScene->myGameObjects[i]);
		}
	}
}

void JsonSceneImporter::ImportTransform(nlohmann::json& someTransformJson, Scene* aScene)
{
	for (int i = 0; i < someTransformJson.size(); i++)
	{
		Transform* transform = aScene->myIdMap[someTransformJson[i]["instanceID"]]->myTransform = new Transform();
		DirectX::XMMATRIX matrix;
		matrix.r[0].m128_f32[0] = (float)someTransformJson[i]["m00"];
		matrix.r[0].m128_f32[1] = (float)someTransformJson[i]["m01"];
		matrix.r[0].m128_f32[2] = (float)someTransformJson[i]["m02"];
		matrix.r[0].m128_f32[3] = (float)someTransformJson[i]["m03"];

		matrix.r[1].m128_f32[0] = (float)someTransformJson[i]["m10"];
		matrix.r[1].m128_f32[1] = (float)someTransformJson[i]["m11"];
		matrix.r[1].m128_f32[2] = (float)someTransformJson[i]["m12"];
		matrix.r[1].m128_f32[3] = (float)someTransformJson[i]["m13"];

		matrix.r[2].m128_f32[0] = (float)someTransformJson[i]["m20"];
		matrix.r[2].m128_f32[1] = (float)someTransformJson[i]["m21"];
		matrix.r[2].m128_f32[2] = (float)someTransformJson[i]["m22"];
		matrix.r[2].m128_f32[3] = (float)someTransformJson[i]["m23"];

		matrix.r[3].m128_f32[0] = (float)someTransformJson[i]["m30"];
		matrix.r[3].m128_f32[1] = (float)someTransformJson[i]["m31"];
		matrix.r[3].m128_f32[2] = (float)someTransformJson[i]["m32"];
		matrix.r[3].m128_f32[3] = (float)someTransformJson[i]["m33"];
		transform->SetMatrix(matrix);
	}
}

#pragma region Components
void JsonSceneImporter::ImportMeshComponent(nlohmann::json& someMeshComponentJson, Scene* aScene)
{
	for (int i = 0; i < someMeshComponentJson.size(); i++)
	{
		std::string path = someMeshComponentJson[i]["path"];
		std::string nodeName = someMeshComponentJson[i]["nodeName"];
		path = RELATIVE_ORGIN_PATH + path;

		std::vector<Mesh*> meshes = MainSingleton::GetInstance().GetModelFactory().Load(path, nodeName);
		MeshComponent* meshComponent = new MeshComponent(meshes);

		aScene->myIdMap[someMeshComponentJson[i]["instanceID"]]->AddComponent<MeshComponent>(meshComponent);
		meshComponent->SetActive(someMeshComponentJson[i]["active"]);
		meshComponent->SetPath(path);
	}
}

void JsonSceneImporter::ImportSkinnedMeshComponent(nlohmann::json& someMeshComponentJson, Scene* aScene)
{
	for (int i = 0; i < someMeshComponentJson.size(); i++)
	{
		std::string path = someMeshComponentJson[i]["path"];
		path = RELATIVE_ORGIN_PATH + path;
		std::vector<AnimatedMesh*> meshes = MainSingleton::GetInstance().GetModelFactory().Load(path, true);
		AnimatedMeshComponent* meshComponent = new AnimatedMeshComponent(meshes);

		meshComponent->SetActive(someMeshComponentJson[i]["active"]);
		meshComponent->SetPath(path);
		GameObject* gameObject = aScene->myIdMap[someMeshComponentJson[i]["instanceID"]];
		gameObject->AddComponent<AnimatedMeshComponent>(meshComponent);
		gameObject->GetTransform()->RotateAroundY(180.f);

	}
}

void JsonSceneImporter::ImportCameraComponent(nlohmann::json& someCameraJson, Scene* aScene)
{
	for (int i = 0; i < someCameraJson.size(); i++)
	{
		GraphicsEngine* graphicsEngine = GraphicsEngine::GetInstance();

		//CameraComponent* camera = new Camera();
		CameraComponent* cameraComponent = new CameraComponent();
		cameraComponent->SetPerspectiveProjection(
			(float)someCameraJson[i]["fieldOfView"],
			(float)graphicsEngine->GetWindowSize().x,
			(float)graphicsEngine->GetWindowSize().y,
			(float)someCameraJson[i]["nearClipPlane"] * 100.f,
			(float)someCameraJson[i]["farClipPlane"] * 100.f);

		//Temp fix, because rotation for the camera is not working the same as for meshes
		aScene->myIdMap[someCameraJson[i]["instanceID"]]->GetTransform()->RotateAroundY(180.f);
		aScene->myIdMap[someCameraJson[i]["instanceID"]]->AddComponent<CameraComponent>(cameraComponent);
		cameraComponent->SetActive(someCameraJson[i]["active"]);
	}
}

void JsonSceneImporter::ImportLightComponent(nlohmann::json& someLightComponentJson, Scene* aScene)
{
	auto& shadowMap = GraphicsEngine::GetInstance()->GetShadowMap();
	//DirectionalLightComponent
	for (size_t i = 0; i < someLightComponentJson.size(); i++)
	{
		float intensity = (float)someLightComponentJson[i]["intensity"] * 100000.f;
		Vector4f color = { (float)someLightComponentJson[i]["color"]["x"],
							(float)someLightComponentJson[i]["color"]["y"],
							(float)someLightComponentJson[i]["color"]["z"],
							(float)someLightComponentJson[i]["color"]["w"] };

		if (someLightComponentJson[i]["type"] == "Directional")
		{
			DirectionalLight* dirLight = new DirectionalLight();
			dirLight->SetColor(color);
			dirLight->SetIntensity(intensity);
			dirLight->myViewPortIndex = shadowMap.RegisterForViewport(Vector2i(8192, 8192));
			if (dirLight->myViewPortIndex > -1)
			{
				dirLight->myTopLeftXY = Vector2f(shadowMap.myViewports[dirLight->myViewPortIndex].TopLeftX, shadowMap.myViewports[dirLight->myViewPortIndex].TopLeftY);
				dirLight->myViewPortSize = Vector2f(shadowMap.myViewports[dirLight->myViewPortIndex].Width, shadowMap.myViewports[dirLight->myViewPortIndex].Height);
			}
			DirectionalLightComponent* lightComponent = new DirectionalLightComponent(dirLight);
			aScene->myIdMap[someLightComponentJson[i]["instanceID"]]->AddComponent<DirectionalLightComponent>(lightComponent);
			lightComponent->SetActive(someLightComponentJson[i]["active"]);
		}

		float range = (float)someLightComponentJson[i]["range"] * 100.f;
		if (someLightComponentJson[i]["type"] == "Spot")
		{
			float innerAngle = (float)someLightComponentJson[i]["innerSpotAngle"];
			float outerAngle = (float)someLightComponentJson[i]["outerSpotAngle"];

			SpotLight* spotLight = new SpotLight();
			spotLight->SetColor(color);
			intensity *= 10.f;
			spotLight->SetIntensity(intensity);
			spotLight->SetRange(range);
			spotLight->SetInnerLimit(innerAngle);
			spotLight->SetOuterLimit(outerAngle);

			//spotLight->myViewPortIndex = shadowMap.RegisterForViewport(Vector2i(2048, 2048));
			//if (spotLight->myViewPortIndex > -1)
			//{
			//	spotLight->myTopLeftXY = Vector2f(shadowMap.myViewports[spotLight->myViewPortIndex].TopLeftX, shadowMap.myViewports[spotLight->myViewPortIndex].TopLeftY);
			//	spotLight->myViewPortSize = Vector2f(shadowMap.myViewports[spotLight->myViewPortIndex].Width, shadowMap.myViewports[spotLight->myViewPortIndex].Height);
			//}

			SpotLightComponent* lightComponent = new SpotLightComponent(spotLight);
			aScene->myIdMap[someLightComponentJson[i]["instanceID"]]->AddComponent<SpotLightComponent>(lightComponent);
			lightComponent->SetActive(someLightComponentJson[i]["active"]);
		}

		if (someLightComponentJson[i]["type"] == "Point")
		{
			PointLight* pointLight = new PointLight();
			pointLight->SetColor(color);
			pointLight->SetIntensity(intensity);
			pointLight->SetRange(range);

			PointLightComponent* lightComponent = new PointLightComponent(pointLight);
			aScene->myIdMap[someLightComponentJson[i]["instanceID"]]->AddComponent<PointLightComponent>(lightComponent);
			lightComponent->SetActive(someLightComponentJson[i]["active"]);
		}
	}
}

void JsonSceneImporter::ImportMeshColliderComponent(nlohmann::json& someColliderComponentJson, Scene* aScene)
{
	for (size_t i = 0; i < someColliderComponentJson.size(); i++)
	{
		PhysicsComponent* collisionComponent = new PhysicsComponent();
		std::string path = someColliderComponentJson[i]["pathName"];
		std::string node = someColliderComponentJson[i]["nodeName"];
		std::string casheName = path + "-" + node;
		bool isStatic = someColliderComponentJson[i]["isStatic"];
		bool isConvex = someColliderComponentJson[i]["isConvex"];
		casheName = RELATIVE_ORGIN_PATH + casheName;
		path = RELATIVE_ORGIN_PATH + path;

		if (!MainSingleton::GetInstance().GetModelFactory().LoadTriangleMeshData(path, isStatic, isConvex, node))
		{
			LogError("Can't load FBX and information to TRIANGLE MESH CACHE. AKA it can't be loaded ");
		}

		GameObject* gameObject = aScene->myIdMap[someColliderComponentJson[i]["instanceID"]];

		if (isConvex)
		{
			std::vector<physx::PxConvexMesh*> pxMeshes = MainSingleton::GetInstance().GetModelFactory().GetConvexPhysxMesh(casheName);
			if (pxMeshes.size() > 0)
			{
				physx::PxConvexMeshGeometry pxConvexMeshGeometry = physx::PxConvexMeshGeometry(pxMeshes[0]);
				collisionComponent->AddGeometry(pxConvexMeshGeometry, isStatic);
				//if(isStatic)
				//	collisionComponent->SetFilter(PhysXManager::FilterGroup::eGROUND, 0);
			}
			else
			{
				LogError("(ConvexMesh) No physics mesh found for " + casheName);
			}
		}
		else
		{
			std::vector<physx::PxTriangleMesh*> pxMeshes = MainSingleton::GetInstance().GetModelFactory().GetTrianglePhysxMesh(casheName);
			if (pxMeshes.size() > 0)
			{
				//TODO: Add support for multiple meshes, should not be needed though

				physx::PxTriangleMeshGeometry pxTriangleMeshGeometry = physx::PxTriangleMeshGeometry(pxMeshes[0]);
				collisionComponent->AddGeometry(pxTriangleMeshGeometry, isStatic);
				//if (isStatic)
				//	collisionComponent->SetFilter(PhysXManager::FilterGroup::eGROUND, 0);
			}
			else
			{
				LogError("(Triangle) No physics mesh found for " + casheName);
				delete collisionComponent;
				continue;
			}
		}
		std::string tag = gameObject->GetTag();
		if (tag == "Booster")
			collisionComponent->SetFilter(PhysXManager::FilterGroup::eBOOST | PhysXManager::FilterGroup::eGROUND, 0);
		else if (tag == "Goal")
			collisionComponent->SetFilter(PhysXManager::FilterGroup::eGOAL, 0);
		else
			collisionComponent->SetFilter(PhysXManager::FilterGroup::eGROUND, 0);

		gameObject->AddComponent<PhysicsComponent>(collisionComponent);
		collisionComponent->SetObjectsPosition();
	}
}

void JsonSceneImporter::ImportTriggerZoneComponent(nlohmann::json& someTriggerZoneComponentJson, Scene* aScene)
{
	for (size_t i = 0; i < someTriggerZoneComponentJson.size(); i++)
	{
		TriggerZoneComponent* triggerZoneComponent = new TriggerZoneComponent();
		triggerZoneComponent->SetActivator(static_cast<TriggerActivator>(someTriggerZoneComponentJson[i]["triggerActivator"]));
		triggerZoneComponent->SetActorType(static_cast<TriggerActor>(someTriggerZoneComponentJson[i]["triggerZoneActors"]));

		aScene->myIdMap[someTriggerZoneComponentJson[i]["instanceID"]]->AddComponent<TriggerZoneComponent>(triggerZoneComponent);
		triggerZoneComponent->SetActive(someTriggerZoneComponentJson[i]["active"]);
		triggerZoneComponent->SetRepeatable(someTriggerZoneComponentJson[i]["isRepeatable"]);
	}
}

void JsonSceneImporter::ImportCardComponent(nlohmann::json& someCardComponentJson, Scene* aScene)
{
	physx::PxSphereGeometry sphereGeo(125);
	for (size_t i = 0; i < someCardComponentJson.size(); i++)
	{
		CardComponent* cardComponent = new CardComponent(CardTypeEnum::COUNT);
		PhysicsComponent* physicsComponent = new PhysicsComponent();
		physicsComponent->AddGeometry(sphereGeo, true);
		physicsComponent->SetFilter(PhysXManager::FilterGroup::eCARD, PhysXManager::FilterGroup::ePLAYER);

		std::string cardType = someCardComponentJson[i]["cardType"];

		if (cardType == "DoubleJump")
		{
			cardComponent->SetCardType(CardTypeEnum::DoubleJump);
		}
		else if (cardType == "Dash")
		{
			cardComponent->SetCardType(CardTypeEnum::Dash);
		}
		else if (cardType == "Stomp")
		{
			cardComponent->SetCardType(CardTypeEnum::Stomp);
		}
		else if (cardType == "DirectionalDash")
		{
			cardComponent->SetCardType(CardTypeEnum::DirectionalDash);
		}
		else if (cardType == "Grapple")
		{
			cardComponent->SetCardType(CardTypeEnum::Grappling);
		}
		else if (cardType == "Teleport")
		{
			cardComponent->SetCardType(CardTypeEnum::Teleporting);
		}

		else
		{
			LogError("CardType not implemented");
		}
		aScene->myIdMap[someCardComponentJson[i]["instanceID"]]->AddComponent<CardComponent>(cardComponent);
		cardComponent->SetActive(someCardComponentJson[i]["active"]);

		aScene->myIdMap[someCardComponentJson[i]["instanceID"]]->AddComponent<PhysicsComponent>(physicsComponent);
		physicsComponent->SetObjectsPosition();
		physicsComponent->SetActive(someCardComponentJson[i]["active"]);
		physicsComponent->SetCollisionFunction(std::bind(&CardComponent::OnCollision, cardComponent, std::placeholders::_1));
		//cardComponent->Init();
	}
}

void JsonSceneImporter::ImportEnemyComponent(nlohmann::json& someEnemyComponentJson, Scene* aScene)
{
	//TODO export Enemy
	for (size_t i = 0; i < someEnemyComponentJson.size(); i++)
	{
		EnemyInfo enemyInfo{};
		GameObject* gameObject;
		enemyInfo.myEnemyType = someEnemyComponentJson[i]["enemyType"];
		enemyInfo.myDropCardType = someEnemyComponentJson[i]["cardDropType"];
		gameObject = MainSingleton::GetInstance().GetEnemyPool().GetEnemyType(enemyInfo);
		float angle = someEnemyComponentJson[i]["rotation"];
		Vector3f position = ImportVector3fX100(someEnemyComponentJson[i]["position"]);
		gameObject->GetComponent<EnemyComponent>()->SetPositionAndRotationToScene(position, angle);
		gameObject->SetActive(someEnemyComponentJson[i]["active"]);


		if (!gameObject->GetChildren().empty())
		{
			GameObject* childBaseTurret = gameObject->GetChildren()[0];
			PhysicsComponent* physicsComponent = childBaseTurret->GetComponent<PhysicsComponent>();
			if (physicsComponent)
			{
				physicsComponent->SetObjectsPosition();
				physicsComponent->SetActive(someEnemyComponentJson[i]["active"]);
				physicsComponent->SetCollisionFunction(std::bind(&TurretComponent::OnCollision, gameObject->GetComponent<TurretComponent>(), std::placeholders::_1));
				physicsComponent->SetFilter(PhysXManager::FilterGroup::eGROUNDENEMY, PhysXManager::FilterGroup::ePLAYERATTACK);
			}
		}
		else
		{
			PhysicsComponent* physicsComponent = gameObject->GetComponent<PhysicsComponent>();
			if (physicsComponent)
			{
				physicsComponent->SetObjectsPosition();
				physicsComponent->SetActive(someEnemyComponentJson[i]["active"]);
				physicsComponent->SetCollisionFunction(std::bind(&BalloonComponent::OnCollision, gameObject->GetComponent<BalloonComponent>(), std::placeholders::_1));
				physicsComponent->SetFilter(PhysXManager::FilterGroup::eBALLONENEMY, PhysXManager::FilterGroup::ePLAYER | PhysXManager::FilterGroup::ePLAYERATTACK);
			}
		}

		aScene->AddGameObject(gameObject);
	}
}

void JsonSceneImporter::ImportDoorGameObject(nlohmann::json& someDoorJson, Scene* aScene)
{
	aScene;
	Door door;

	std::vector<GameObject*> DoorPart;
	DoorPart.reserve(someDoorJson.size());

	for (size_t i = 0; i < someDoorJson.size(); ++i)
	{
		int iD = someDoorJson[i]["instanceID"];
		bool activated = someDoorJson[i]["active"];
		std::string tag = someDoorJson[i]["myTag"];
		GameObject* ObjectDoorPart = aScene->myIdMap[iD];
		ObjectDoorPart->myTag = tag;
		ObjectDoorPart->SetActive(activated);
		DoorPart.push_back(ObjectDoorPart);
	}

	door.FixDoor(DoorPart);

	for (size_t i = 0; i < DoorPart.size(); i++)
	{
		GameObject* gameObject = DoorPart[i];
		PhysicsComponent* physicsComponent = gameObject->GetComponent<PhysicsComponent>();
		if (physicsComponent)
		{
			physicsComponent->SetObjectsPosition();
			physicsComponent->SetActive(gameObject->IsActive());
		}
	}
}

void JsonSceneImporter::ImportRotatorComponent(nlohmann::json& someRotatorComponentJson, Scene* aScene)
{
	for (size_t i = 0; i < someRotatorComponentJson.size(); i++)
	{
		RotatorComponent* rotatorComponent = new RotatorComponent();
		Vector3f rotation = ImportVector3fX100(someRotatorComponentJson[i]["rotation"]);
		rotation /= 100.f;
		rotatorComponent->SetRotator(rotation);
		aScene->myIdMap[someRotatorComponentJson[i]["instanceID"]]->AddComponent<RotatorComponent>(rotatorComponent);
		rotatorComponent->SetActive(someRotatorComponentJson[i]["active"]);
	}
}


#pragma endregion
#pragma region Actions
void JsonSceneImporter::ImportActions(nlohmann::json& someActionsJson, Scene* aScene)
{
	ImportUnlockTriggerZoneAction(someActionsJson["unlockTriggerZoneActions"], aScene);
	ImportMoveTransformAction(someActionsJson["moveTransformActions"], aScene);
	ImportRotateTransformAction(someActionsJson["rotateTransformActions"], aScene);
	ImportPlaySoundAction(someActionsJson["playSoundActions"], aScene);
	ImportLevelTransitionAction(someActionsJson["levelTransitionActions"], aScene);
	ImportDelayAction(someActionsJson["delayActions"], aScene);
	ImportToggleComponentAction(someActionsJson["toggleTriggerZoneActions"], aScene);
}

int JsonSceneImporter::ImportAction(nlohmann::json& someActionJson, Scene* aScene, Action* aAction)
{
	aAction->order = someActionJson["actionsRequired"]["order"];
	ImportActionKeys(someActionJson["actionsRequired"]["key"], aScene, aAction);
	return						someActionJson["actionsRequired"]["instanceID"];
}
void JsonSceneImporter::ImportActionKeys(nlohmann::json& someActionKeysJson, Scene* aScene, Action* aAction)
{
	for (size_t i = 0; i < someActionKeysJson["targetID"].size(); i++)
	{
		int targetID = someActionKeysJson["targetID"][i];
		GameObject* target = aScene->myIdMap[targetID];
		//TriggerZoneComponent* triggerZoneComponent = target->GetComponent<TriggerZoneComponent>();
		aAction->AddKeyTarget(target->GetComponent<TriggerZoneComponent>());
	}
}

void JsonSceneImporter::ImportUnlockTriggerZoneAction(nlohmann::json& someUnlockTriggerZoneActionJson, Scene* aScene)
{
	for (size_t i = 0; i < someUnlockTriggerZoneActionJson.size(); i++)
	{
		UnlockTriggerZoneAction* unlockTriggerZoneAction = new UnlockTriggerZoneAction();
		int instanceID = ImportAction(someUnlockTriggerZoneActionJson[i], aScene, unlockTriggerZoneAction);

		GameObject* owner = aScene->myIdMap[instanceID];
		owner->GetComponent<TriggerZoneComponent>()->AddAction(unlockTriggerZoneAction);


	}
}

void JsonSceneImporter::ImportMoveTransformAction(nlohmann::json& someMoveTransformActionJson, Scene* aScene)
{
	for (size_t i = 0; i < someMoveTransformActionJson.size(); i++)
	{
		MoveTransformAction* moveTransformAction = new MoveTransformAction();
		int instanceID = ImportAction(someMoveTransformActionJson[i], aScene, moveTransformAction);
		int targetID = someMoveTransformActionJson[i]["targetID"];
		if (targetID == -1)
		{
			LogWarning("MoveTransformAction in " + std::to_string(instanceID) + " has no targetID");
			delete moveTransformAction;
			continue;
		}

		GameObject* owner = aScene->myIdMap[instanceID];

		owner->GetComponent<TriggerZoneComponent>()->AddAction(moveTransformAction);
		moveTransformAction->SetTarget(aScene->myIdMap[targetID]);
		moveTransformAction->SetOffset(ImportVector3fX100(someMoveTransformActionJson[i]["relativeTarget"]));

		moveTransformAction->SetTime((float)someMoveTransformActionJson[i]["time"]);
	}
}
void JsonSceneImporter::ImportRotateTransformAction(nlohmann::json& someRotateTransformActionJson, Scene* aScene)
{
	for (size_t i = 0; i < someRotateTransformActionJson.size(); i++)
	{
		RotateTransformAction* rotateTransformAction = new RotateTransformAction();
		int instanceID = ImportAction(someRotateTransformActionJson[i], aScene, rotateTransformAction);
		int targetID = someRotateTransformActionJson[i]["targetID"];
		if (targetID == -1)
		{
			LogWarning("RotateTransformAction in " + std::to_string(instanceID) + " has no targetID");
			delete rotateTransformAction;
			continue;
		}
		GameObject* owner = aScene->myIdMap[instanceID];

		owner->GetComponent<TriggerZoneComponent>()->AddAction(rotateTransformAction);
		rotateTransformAction->SetTarget(aScene->myIdMap[targetID]);
		rotateTransformAction->SetRotation(ImportVector3fX100(someRotateTransformActionJson[i]["rotation"]));


		rotateTransformAction->SetTime((float)someRotateTransformActionJson[i]["time"]);
	}
}
void JsonSceneImporter::ImportPlaySoundAction(nlohmann::json& somePlaySoundActionJson, Scene* aScene)
{
	somePlaySoundActionJson;
	aScene;
	LogWarning("PlaySoundAction not implemented");
}
void JsonSceneImporter::ImportLevelTransitionAction(nlohmann::json& someLevelTransitionActionJson, Scene* aScene)
{
	for (size_t i = 0; i < someLevelTransitionActionJson.size(); i++)
	{
		LevelTransitionAction* levelTransformAction = new LevelTransitionAction();
		int instanceID = ImportAction(someLevelTransitionActionJson[i], aScene, levelTransformAction);
		std::string nextSceneName = someLevelTransitionActionJson[i]["nextSceneName"];

		GameObject* owner = aScene->myIdMap[instanceID];

		levelTransformAction->SetNextSceneName(nextSceneName);
		owner->GetComponent<TriggerZoneComponent>()->AddAction(levelTransformAction);
	}
}
void JsonSceneImporter::ImportDelayAction(nlohmann::json& someDelayActionJson, Scene* aScene)
{
	for (size_t i = 0; i < someDelayActionJson.size(); i++)
	{
		DelayAction* delayAction = new DelayAction();
		int instanceID = ImportAction(someDelayActionJson[i], aScene, delayAction);

		GameObject* owner = aScene->myIdMap[instanceID];
		owner->GetComponent<TriggerZoneComponent>()->AddAction(delayAction);
		delayAction->SetDelay((float)someDelayActionJson[i]["delay"]);
	}
}

void JsonSceneImporter::ImportToggleComponentAction(nlohmann::json& someToggleComponentAction, Scene* aScene)
{
	aScene;
	for (size_t i = 0; i < someToggleComponentAction.size(); i++)
	{
		ToggleTriggerZoneAction* toggleComponentAction = new ToggleTriggerZoneAction();
		int instanceID = ImportAction(someToggleComponentAction[i], aScene, toggleComponentAction);

		GameObject* owner = aScene->myIdMap[instanceID];
		owner->GetComponent<TriggerZoneComponent>()->AddAction(toggleComponentAction);
		int targetID = someToggleComponentAction[i]["targetID"];
		toggleComponentAction->SetTarget(aScene->myIdMap[targetID]->GetComponent<TriggerZoneComponent>());
		int toggleAction = someToggleComponentAction[i]["actionType"];
		toggleComponentAction->SetToggleAction(static_cast<eToggleAction>(toggleAction));
	}
}

Vector3f JsonSceneImporter::ImportVector3fX100(nlohmann::json& someVector3)
{
	Vector3f position;
	position.x = (float)someVector3["x"];
	position.y = (float)someVector3["y"];
	position.z = (float)someVector3["z"];
	position *= 100.0f;
	return position;
}

#pragma endregion
void JsonSceneImporter::ImportPlayer(nlohmann::json& somePlayerJson, Scene* aScene)
{
	GameObject* player = new GameObject();
	PlayerComponent* playerComponent = new PlayerComponent();
	player->myName = "Player";
	player->myTag = "Player";
	player->myLayer = "Player";
	player->AddComponent(playerComponent);
	try
	{
		playerComponent->SetStartRotation(somePlayerJson["spawnPlayer"]["rotation"]);
		playerComponent->SetStartPos(ImportVector3fX100(somePlayerJson["spawnPlayer"]["position"]));
	}
	catch (const std::exception&)
	{
		LogError("Player position not found in scene file");
	}
	//playerComponent->SetStartRotation(somePlayerJson["spawnPlayer"]["rotation"]);
	//playerComponent->SetStartPos(ImportVector3fX100(somePlayerJson["spawnPlayer"]["position"]));
	aScene->AddGameObject(player);
	playerComponent->Init(*aScene);
	GameObject* camera = playerComponent->GetCameraGameObject();
	camera->myName = "PlayerCamera";
	camera->myTag = "MainCamera";
	aScene->mySceneCamera = camera->GetComponent<CameraComponent>();

}

