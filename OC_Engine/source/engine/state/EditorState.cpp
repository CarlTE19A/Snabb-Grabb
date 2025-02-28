#include "stdafx.h"
#include "Paths.h"

//#include "graphics\Texture.h"
#include "EditorState.h"
#include "State.h"
#include "StateStack.h"
#include "events/inputMapper/InputMapper.h"
#include "events/EventSystem.h"
#include "mainSingelton/MainSingelton.h"
#include "scene/SceneManager.h"
#include "graphics/GraphicsEngine.h"
#include <iostream>
#include <filesystem>

//#include "d3d11.h"
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui\imgui_stdlib.h"

#include "scene/Scene.h"
#include "gameObject/GameObject.h"
#include "component/CameraComponent.h"

#include "tools/lineDrawer/LineDrawer.h"

#include "graphics\vfx\VFXElement.h"

#include "../game/Ammo/AmmoPool.h"
#include "../game/PollingStation.h"
#include "nlohmann\json.hpp"
#include <fstream>

#include "graphics\model\ModelFactory.h"

#include "component\DirectionalLightComponent.h"
#include "graphics\light\PointLight.h"
#include "component\PointLightComponent.h"
#include "graphics\light\SpotLight.h"
#include "component\SpotLightComponent.h"

#include "PhysXCode\PhysXManager.h"

//Player
#include "../game/Controller/ControllerComponent.h"
#include "../game/Player/PlayerComponent.h"

//ui
#include "graphics/sprite/Sprite.h"
#include "graphics\ui\UIComponent.h"
#include "graphics\ui\UIButton.h"
#include "graphics\ui\UISprite.h"
#include "graphics\ui\UIText.h"
#include "graphics\ui\UITextField.h"
#include "graphics\ui\UISpriteSheet.h"
#include "graphics\ui\UISpriteMorph.h"

//test playerui
#include "../game/Player/PlayerUI.h"

#include "../game/GameManager/GameManager.h"
#include "Script-Base/Script/Editor/ScriptEditor.h"

EditorState::EditorState(StateStack& aStateStack)
	:myScriptEditor(new ScriptEditor())
{
	myStateStack = &aStateStack;

	myDebugCamera = new GameObject();
	myDebugCamera->AddComponent(new CameraComponent());
	myDebugCamera->myName = "Debug Camera";
	myDebugCamera->GetTransform()->SetRotation(Vector3f(0.f, 180.f, 0.f));
	myDebugCamera->SetPosition(Vector3f(0.f, 50.f, -100.f));

	// init UILayouts prob move to somewhere else later plz dont hate it works
	myUIHandlers.reserve(10);
	const std::filesystem::path UILayouts{ STATIC_ASSET_PATH "/UILayouts" };
	if (!std::filesystem::exists(UILayouts))
	{
		std::filesystem::create_directory(UILayouts);
	}
	for (const auto& entry : std::filesystem::directory_iterator(UILayouts))
	{
		std::string name = entry.path().filename().string();
		name = name.substr(0, name.find_last_of("."));
		myUIHandlers.push_back(UIHandler());
		myUIHandlers.back().Init(name);
		myUIHandlers.back().ReadUILayout();
	}
	//myPlayerUI = new PlayerUI();
	//myPlayerUI->Init();
}

EditorState::~EditorState()
{
	delete myDebugCamera;
	delete myFileObserver;
	auto& inputmapper = MainSingleton::GetInstance().GetInputMapper();
	inputmapper.RemoveObserver(eInputEvent::ePause, this);
}

void EditorState::Init()
{
	MainSingleton::GetInstance().GetInputMapper().GetInputWrapper().SetFocus(true, false);
	graphicsEngine = GraphicsEngine::GetInstance();
	LoadEditorSettings();

	CameraComponent* debugCameraComponent = myDebugCamera->GetComponent<CameraComponent>();
	debugCameraComponent->SetPerspectiveProjection(90.f, (float)graphicsEngine->GetWindowSize().x, (float)graphicsEngine->GetWindowSize().y, 1.f, 200000.f);
	graphicsEngine->SetCamera(myDebugCamera->GetComponent<CameraComponent>());

	myDropTexture.Initialize(TextureInitializationType::DDS, STATIC_EDITOR_PATH "DropHere.dds", false);

	sceneManager = &MainSingleton::GetInstance().GetSceneManager();
	sceneManager->ResetScene();
	Log("Init Editor");
	myIsActive = true;
	//std::vector<Scene*> scenes = MainSingleton::GetInstance().GetSceneManager().GetScenes();
	//for (int i = 0; i < scenes.size(); i++)
	//{
	//	scenes[i]->Reset();
	//}
	//scenes.clear();
	myFileObserver = new Observer(Message::Type::FileDropped, std::bind(&EditorState::HandleFileDrop, this, std::placeholders::_1));
	myDeathObserver = new Observer(Message::Type::LostGame, std::bind(&EditorState::HandleLostGame, this, std::placeholders::_1));
	myWinObserver = new Observer(Message::Type::WonGame, std::bind(&EditorState::HandleWonGame, this, std::placeholders::_1));


	auto& inputmapper = MainSingleton::GetInstance().GetInputMapper();
	inputmapper.AddObserver(eInputEvent::ePause, this);

	//TestFunction();
	//myUIHandlers[0].SetVisible(true);

	myScriptEditor->Init();
}

void EditorState::UnInit()
{
	//SaveEditorSettings();
	delete myFileObserver;
	myFileObserver = nullptr;

	delete myDeathObserver;
	myDeathObserver = nullptr;

	delete myWinObserver;
	myWinObserver = nullptr;

	auto& inputmapper = MainSingleton::GetInstance().GetInputMapper();
	inputmapper.RemoveObserver(eInputEvent::ePause, this);
	myIsActive = false;
}

bool EditorState::Update(float aDeltaTime)
{
	////ONLY For Math
	//if (!pauseFPS)
	//{
	//	timeSinceUpdate += aDeltaTime;
	//	if (timeSinceUpdate > updateRate)
	//	{
	//		timeSinceUpdate = 0.0f;
	//		float sum = 0;
	//		for (int i = 0; i < dCount; ++i)
	//		{
	//			latestFPS[i] = latestFPS[i + 1];
	//			sum += latestFPS[i];
	//		}
	//		latestFPS[dCount - 1] = 1 / aDeltaTime;
	//		avgFPS = sum / dCount;
	//	}
	//}
	//latestFPS[0] = 1 / aDeltaTime;
	MainSingleton& mainSingleton = MainSingleton::GetInstance();

#ifdef _DEBUG
	ImGuiUpdate();
	mainSingleton.GetGameManager().ImGuiShowScenes();


	mainSingleton.GetSceneManager().SceneRestetSwitch();

	mainSingleton.GetLineDrawer().myActive = myShowDebugLines;
	if (myDebugCamera->GetComponent<CameraComponent>() == graphicsEngine->GetCamera())
		ControlCamera(myDebugCamera->GetComponent<CameraComponent>(), aDeltaTime);
#endif // _DEBUG


	Scene* scene = sceneManager->GetActiveScene();
	if (scene == nullptr)
		return myIsActive;
	mainSingleton.GetPhysXManager().Update(aDeltaTime * physicsTimeScale);
	scene->Update(aDeltaTime);

	for (int i = 0; i < myUIHandlers.size(); i++)
	{
		if (myUIHandlers[i].IsVisible())
		{
			myUIHandlers[i].Update(aDeltaTime);
		}
	}

	return myIsActive;
}

void EditorState::Render()
{
	Scene* scene = sceneManager->GetActiveScene();
	if (scene == nullptr)
		return;
	//scene->Render();
	scene->RenderDeferred();
	for (int i = 0; i < myUIHandlers.size(); i++)
	{
		if (myUIHandlers[i].IsVisible())
		{
			myUIHandlers[i].Draw();
		}
	}

	//myPlayerUI->Render();

#ifdef _DEBUG
	LineDrawer& lineDrawer = MainSingleton::GetInstance().GetLineDrawer();
	DebugLine line;
	line.color = { 1,0,0,1 };
	line.start = { 0,0,0 };
	line.end = { 100,0,0 };
	lineDrawer.Draw(line);
	line.start = { 0,0,0 };
	line.end = { 0,100,0 };
	line.color = { 0,0,1,1 };
	lineDrawer.Draw(line);
	line.start = { 0,0,0 };
	line.end = { 0,0,100 };
	line.color = { 0,1,0,1 };
	lineDrawer.Draw(line);
#endif
}

void EditorState::RecieveEvent(const eInputEvent aEvent, std::any aValue)
{
	aEvent;
	aValue;
}

void EditorState::ImGuiUpdate()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Windows"))
		{
			ImGui::Checkbox("Hierarchy", &myShowHierarchy);
			ImGui::Checkbox("Inspector", &myShowInspector);
			ImGui::Checkbox("Deferred Rendering Window", &myShowDeferred);
			//ImGui::Checkbox("VFX Window", &myShowVFXWindow);
			ImGui::Checkbox("World Settings", &myShowWorldSettings);
			//ImGui::Checkbox("Debug Window", &myShowDebugWindow);
			ImGui::Checkbox("Demo Window", &myShowDemoWindow);
			ImGui::Checkbox("UI Editor Window", &myShowUIEditorWindow);
			ImGui::Checkbox("UI Inspector Window", &myShowUIInspectorWindow);
			ImGui::Checkbox("Particle Window", &myShowParticleWindow);
			ImGui::Checkbox("Script Editor", &myShowScript);
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Tools"))
		{
			ImGui::Checkbox("Line Drawer", &myShowDebugLines);
			if (ImGui::BeginMenu("Editor Layout"))
			{
				if (ImGui::MenuItem("Save Editor Layout"))
				{
					SaveEditorSettings();
				}
				if (ImGui::MenuItem("Load Editor Layout"))
				{
					LoadEditorSettings();
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}
	}
	ImGui::EndMainMenuBar();
	ImGuiWindowFlags window_flags = 0;
	//window_flags |= ImGuiWindowFlags_;
	//window_flags |= ImGuiWindowFlags_NoResize;
	//window_flags |= ImGuiWindowFlags_NoCollapse;
	//window_flags |= ImGuiWindowFlags_NoFocusOnAppearing;
	if (myShowHierarchy)
	{
		UpdateHierarchy(window_flags);
	}

	if (myShowInspector)
	{
		UpdateInspector(window_flags);
	}

	if (myShowUIEditorWindow)
	{
		UpdateUIEditorWindow(window_flags);
	}

	if (myShowUIInspectorWindow)
	{
		UpdateUIInspector(window_flags);
	}

	if (myShowDeferred)
	{
		UpdateDeferredWindow(window_flags);
	}

	if (myShowWorldSettings)
	{
		UpdateWorldSettings(window_flags);
	}

	//if (myShowDebugWindow)
	//{
	//	UpdateDebugWindow(window_flags);
	//}

	if (myShowParticleWindow)
	{
		UpdateParticleWindow(window_flags);
	}

	if (myShowScript)
	{
		UpdateScriptWindow(window_flags);
	}

	if (myShowDemoWindow)
	{
		ImGui::ShowDemoWindow();
	}

	layoutChanged = false;
}

void EditorState::ControlCamera(CameraComponent* aCamera, float aDeltaTime)
{
	Transform* transform = aCamera->GetOwner()->GetTransform();

	static Vector2<int> mouseLockPosition = { INT_MAX, INT_MAX };
	static float speedMultiplier = 1.0f;

	if (GetAsyncKeyState(VK_RBUTTON))
	{
		if (GetFocus())
		{
			POINT mousePosition;
			GetCursorPos(&mousePosition);
			if (mouseLockPosition.x == INT_MAX)
			{
				mouseLockPosition.x = mousePosition.x;
				mouseLockPosition.y = mousePosition.y;
			}

			Vector2<int> mouseDelta = { 0, 0 };
			mouseDelta.x = mousePosition.x - mouseLockPosition.x;
			mouseDelta.y = mousePosition.y - mouseLockPosition.y;


			//rotate based on mouse delta

			// Modify the rotation angles based on mouse delta
			float rotationSpeed = 0.5f;
			float yaw = mouseDelta.x * rotationSpeed;
			float pitch = mouseDelta.y * rotationSpeed;

			// Apply rotation to the camera's transform

			transform->RotateAroundAxis({ 0, 1, 0 }, yaw);
			transform->RotateAroundX(pitch);
			if (transform->GetUp().y < 0)
				transform->RotateAroundX(-pitch);

			//set mouse position back to mouseLockPosition
			//SetCursorPos(mouseLockPosition.x, mouseLockPosition.y);
			mouseLockPosition.x = mousePosition.x;
			mouseLockPosition.y = mousePosition.y;

			float mySpeed = 100.0f;

			if (GetAsyncKeyState(VK_SHIFT))
			{
				mySpeed *= 5.0f;
			}
			mySpeed *= speedMultiplier;
			float scrollDelta = MainSingleton::GetInstance().GetInputMapper().GetMouseWheelDelta();
			if (scrollDelta > -10 && scrollDelta < 10)
			{
				speedMultiplier += scrollDelta * 0.1f;
				if (speedMultiplier < 0.1f)
					speedMultiplier = 0.1f;
			}

			if (GetAsyncKeyState('E'))
			{
				transform->MoveUp(aDeltaTime * mySpeed);
				//transform.Move(transform.GetUp() * aDeltaTime * mySpeed);
			}
			if (GetAsyncKeyState('Q'))
			{
				transform->MoveUp(aDeltaTime * -mySpeed);
				//transform.Move(transform.GetUp() * aDeltaTime * -mySpeed);
			}

			if (GetAsyncKeyState('W'))
			{
				transform->MoveForward(aDeltaTime * mySpeed);
				//transform.Move(transform.GetForward() * aDeltaTime * mySpeed);
			}
			if (GetAsyncKeyState('S'))
			{
				transform->MoveForward(aDeltaTime * -mySpeed);
				//transform.Move(transform.GetForward() * aDeltaTime * -mySpeed);
			}
			if (GetAsyncKeyState('A'))
			{
				transform->MoveRight(aDeltaTime * -mySpeed);
				//transform.Move(transform.GetRight() * aDeltaTime * -mySpeed);
			}
			if (GetAsyncKeyState('D'))
			{
				transform->MoveRight(aDeltaTime * mySpeed);
				//transform.Move(transform.GetRight() * aDeltaTime * mySpeed);
			}
		}
	}
	else
	{
		mouseLockPosition.x = INT_MAX;
		mouseLockPosition.y = INT_MAX;
	}

}

void EditorState::UpdateHierarchy(ImGuiWindowFlags window_flags)
{
	Scene* scene = sceneManager->GetActiveScene();

	if (ImGui::Begin("Hierarchy", 0, window_flags))
	{
		if (!layoutChanged)
		{
			myHierarchyPos.x = ImGui::GetWindowPos().x;
			myHierarchyPos.y = ImGui::GetWindowPos().y;
			myHierarchySize.x = ImGui::GetWindowSize().x;
			myHierarchySize.y = ImGui::GetWindowSize().y;
		}
		ImGui::SetWindowPos(ImVec2(myHierarchyPos.x, myHierarchyPos.y));
		ImGui::SetWindowSize(ImVec2(myHierarchySize.x, myHierarchySize.y));

		if (ImGui::Button("Activate Debug Camera"))
		{
			graphicsEngine->SetCamera(myDebugCamera->GetComponent<CameraComponent>());
		}
		ImGui::SameLine();
		if (ImGui::TreeNode("Lights"))
		{
			//TODO Lights can probably cause memory leaks as they are not deleted by component but by engine (dir light not active will then not be activated)
			//ImGui::Tree
			//if (ImGui::Button("Add Directional Light"))
			//{
			//	GameObject* newObject = new GameObject();
			//	DirectionalLight* light = new DirectionalLight();
			//	light->SetColor({ 1.f, 1.f, 1.f, 1.f });
			//	light->SetIntensity(5000.f);
			//
			//	DirectionalLightComponent* lightComponent = new DirectionalLightComponent(light);
			//
			//	newObject->AddComponent<DirectionalLightComponent>(lightComponent);
			//	Scene* scene = MainSingleton::GetInstance().GetSceneManager().GetActiveScene();
			//	newObject->myName = "Directional Light";
			//	scene->AddGameObject(newObject);
			//
			//	mySelectedGameObject = newObject;
			//	myInspectorActive = InspectorActive::eGameObject;
			//}
			if (ImGui::Button("Add Point Light"))
			{
				GameObject* newObject = new GameObject();
				PointLight* light = new PointLight();
				light->SetColor({ 1.f, 1.f, 1.f, 1.f });
				light->SetIntensity(5000.f);
				light->SetRange(1000.f);

				PointLightComponent* lightComponent = new PointLightComponent(light);
				graphicsEngine->AddPointLight(*light);

				newObject->AddComponent<PointLightComponent>(lightComponent);
				newObject->myName = "Point Light";
				scene->AddGameObject(newObject);

				mySelectedGameObject = newObject;
				myInspectorActive = InspectorActive::eGameObject;
			}
			if (ImGui::Button("Add Spot Light"))
			{
				GameObject* newObject = new GameObject();
				SpotLight* light = new SpotLight();
				light->SetColor({ 1.f, 1.f, 1.f, 1.f });
				light->SetIntensity(5000.f);
				light->SetRange(1000.f);
				light->SetInnerLimit(0.f);
				light->SetOuterLimit(3.1415f);

				SpotLightComponent* lightComponent = new SpotLightComponent(light);
				graphicsEngine->AddSpotLight(*light);

				newObject->AddComponent<SpotLightComponent>(lightComponent);
				newObject->myName = "Spot Light";
				scene->AddGameObject(newObject);

				mySelectedGameObject = newObject;
				myInspectorActive = InspectorActive::eGameObject;
			}
			ImGui::TreePop();
		}

		ImGui::SameLine();

		if (ImGui::TreeNode("Ammo"))
		{
			float activeTime = MainSingleton::GetInstance().GetPollingStation().GetAmmoPool()->GetActiveTime();
			float bulletSpeed = MainSingleton::GetInstance().GetPollingStation().GetAmmoPool()->GetBulletSpeed();
			float radius = MainSingleton::GetInstance().GetPollingStation().GetAmmoPool()->GetAmmoRadius();

			ImGui::DragFloat("Active Time", &activeTime, 1, 1, 50);

			ImGui::DragFloat("Bullet Speed", &bulletSpeed, 1, -500, 10000);

			ImGui::DragFloat("Ammo Radius", &radius, 1, 1, 1000);

			MainSingleton::GetInstance().GetPollingStation().GetAmmoPool()->SetActiveTime(activeTime);
			MainSingleton::GetInstance().GetPollingStation().GetAmmoPool()->SetBulletSpeed(bulletSpeed);
			MainSingleton::GetInstance().GetPollingStation().GetAmmoPool()->SetRadius(radius);

			if (ImGui::Button("Apply"))
			{
				MainSingleton::GetInstance().GetPollingStation().GetAmmoPool()->EditBullet();
			}
			ImGui::SeparatorText("Data");

			if (ImGui::Button("Save"))
			{
				MainSingleton::GetInstance().GetPollingStation().GetAmmoPool()->SaveData();
			}
			if (ImGui::Button("Load"))
			{
				MainSingleton::GetInstance().GetPollingStation().GetAmmoPool()->LoadData();
			}

			ImGui::TreePop();
		}


		//if (ImGui::Button("Add Directional Light"));
		//{
		//	//myDebugCamera->GetTransform()->SetPosition(Vector3f(0.f, 0.f, 0.f));
		//	//myDebugCamera->GetTransform()->SetRotation(Vector3f(0.f, 180.f, 0.f));
		//
		//}
		//std::vector<Scene*> scenes = sceneManager->GetScenes();
		ImVec2 pos = ImGui::GetCursorPos();
		ImGui::Dummy(ImGui::GetContentRegionAvail());
		/*if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("jsonFile"))
			{
				std::string path = *(std::string*)payload->Data;
				Scene* newScene = sceneManager->LoadSceneFromPath(path);
				bool exists = false;
				for (Scene* scenee : scenes)
				{
					if (newScene->myScenePath == scenee->myScenePath)
					{
						LogWarning("Scene already loaded");
						exists = true;
						delete newScene;
						break;
					}
				}
				if (!exists)
				{
					scenes.push_back(newScene);
				}
			}
			ImGui::EndDragDropTarget();
		}*/
		ImGui::SetCursorPos(pos);

		ImGui::InputText("Search", mySearchBuffer, maxBufferLen, ImGuiInputTextFlags_EscapeClearsAll);

		//for (int i = 0; i < scenes.size(); i++)
		//{
			//ImGui::Checkbox(("##" + scene->mySceneName).c_str(), &scene->myActive);
			//ImGui::SameLine();
			//if (ImGui::Button(("Restart ##" + scenes[i]->mySceneName).c_str(), ImVec2(50, 20)))
			//{
			//	//Scene* resetScene = scenes[i]->GetResetScene();
			//	//Scene* resetScene = MainSingleton::GetInstance().GetSceneManager().GetResetScene(scenes[i]->myScenePath);
			//	MainSingleton::GetInstance().GetSceneManager().ResetScene();
			//	//if (resetScene != nullptr)
			//	//{
			//	//	delete scenes[i];
			//	//	scenes[i] = resetScene;
			//	//	scenes[i]->Init();
			//	//	LogInfo("Reset Scene");
			//	//}
			//	//else
			//	//{
			//	//	LogWarning("No reset scene found");
			//	//}
			//}
			//ImGui::SameLine();
		if (ImGui::CollapsingHeader(scene->mySceneName.c_str()))
		{
			DragDropTargetScene(scene);
			float width = ImGui::GetContentRegionAvail().x;

			for (int j = 0; j < scene->myGameObjects.size(); j++)
			{
				if (mySearchBuffer[0] != '\0')	//No ordering if searching
				{
					UpdateHierarchyGameObject(scene->myGameObjects[j], scene);
				}
				else if (scene->myGameObjects[j]->parentID == 0)	//Let the root objects be first and call its children
				{
					UpdateHierarchyGameObject(scene->myGameObjects[j], scene);
				}
				else
				{
					continue;
				}

				//Drop target between objects
				pos = ImGui::GetCursorPos();
				pos.y -= 4.0f;
				ImGui::SetCursorPos(pos);
				pos.y += 4.0f;
				ImGui::Dummy(ImVec2(width, 5.0f));
				ImGui::SetCursorPos(pos);
				DragDropTargetScene(scene);
			}
		}
		//}
	}
	ImGui::End();
}

void EditorState::UpdateHierarchyGameObject(GameObject* aGameObject, Scene* aScene)
{
	if (mySearchBuffer[0] != '\0')	//If search is not empty
	{
		std::string lowerName = aGameObject->myName + " | " + std::to_string(aGameObject->GetID());
		std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);

		std::string lowerSearch = mySearchBuffer;
		std::transform(lowerSearch.begin(), lowerSearch.end(), lowerSearch.begin(), ::tolower);

		if (!strstr(lowerName.c_str(), lowerSearch.c_str()))
		{
			return;
		}
	}
	std::string label = aGameObject->myName + " | " + std::to_string(aGameObject->GetID());

	//If this is selected
	bool selected = false;
	if (aGameObject == mySelectedGameObject && myInspectorActive == InspectorActive::eGameObject)
	{
		selected = true;
	}

	if (aGameObject->IsActive())
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
	else
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
	ImGuiTreeNodeFlags tree_flags = 0;
	tree_flags |= ImGuiTreeNodeFlags_OpenOnArrow;
	tree_flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
	tree_flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
	if (aGameObject->myChildren.size() == 0)
	{
		tree_flags |= ImGuiTreeNodeFlags_Leaf;
	}

	bool openTreeNode = ImGui::TreeNodeEx(label.c_str(), tree_flags);
	ImGui::PopStyleColor();

	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
	{
		ImGui::SetDragDropPayload("GameObject", &aGameObject, sizeof(GameObject*));
		ImGui::Text(label.c_str());
		ImGui::EndDragDropSource();
	}
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GameObject"))
		{
			GameObject* gameObject = *(GameObject**)payload->Data;
			if (!aGameObject->IsInParents(gameObject))
			{
				Scene* scene = sceneManager->GetActiveScene();
				scene->RemoveGameObject(gameObject);
				gameObject->SetParent(aGameObject);
				aScene->AddGameObject(gameObject);
			}
		}
		ImGui::EndDragDropTarget();
	}


	if (ImGui::IsItemClicked())
	{
		mySelectedGameObject = aGameObject;
		myInspectorActive = InspectorActive::eGameObject;
	}
	if (openTreeNode)
	{
		//mySelectedGameObject = aGameObject;
		//myInspectorActive = InspectorActive::eGameObject;
		for (GameObject* child : aGameObject->myChildren)
		{
			UpdateHierarchyGameObject(child, aScene);
		}
		ImGui::TreePop();
	}

	return;
}

void EditorState::DragDropTargetScene(Scene* aScene)
{
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GameObject"))
		{
			Log("GameObject dropped in scene: " + aScene->mySceneName);
			GameObject* gameObject = *(GameObject**)payload->Data;
			std::vector<Scene*> scenes = sceneManager->GetScenes();
			for (int j = 0; j < scenes.size(); j++)
			{
				scenes[j]->RemoveGameObject(gameObject);
			}
			gameObject->SetParent(nullptr);
			aScene->AddGameObject(gameObject);
		}
		ImGui::EndDragDropTarget();
	}
}

void EditorState::UpdateInspector(ImGuiWindowFlags window_flags)
{
	if (ImGui::Begin("Inspector", 0, window_flags))
	{
		if (!layoutChanged)
		{
			myInspectorPos.x = ImGui::GetWindowPos().x;
			myInspectorPos.y = ImGui::GetWindowPos().y;
			myInspectorSize.x = ImGui::GetWindowSize().x;
			myInspectorSize.y = ImGui::GetWindowSize().y;
		}
		ImGui::SetWindowPos(ImVec2(myInspectorPos.x, myInspectorPos.y));
		ImGui::SetWindowSize(ImVec2(myInspectorSize.x, myInspectorSize.y));
		switch (myInspectorActive)
		{
		case InspectorActive::eNone:
			//Empty
			break;
		case InspectorActive::eGameObject:
			if (mySelectedGameObject == nullptr)
			{
				myInspectorActive = InspectorActive::eNone;
				break;
			}
			UpdateInspectorGameObject();
			break;
		default:
			break;
		}
	}
	ImGui::End();
}

void EditorState::UpdateInspectorGameObject()
{
	//Show active gameObject

	Transform* transform = mySelectedGameObject->GetTransform();

	LineDrawer& lineDrawer = MainSingleton::GetInstance().GetLineDrawer();
	DebugLine line;
	Vector3f pos = transform->GetPosition();
	line.start = pos;
	line.end = pos + transform->GetForward() * 100.f;
	line.color = { 0,0,1,1 };
	lineDrawer.Draw(line);
	line.end = pos + transform->GetUp() * 100.f;
	line.color = { 0,1,0,1 };
	lineDrawer.Draw(line);
	line.end = pos + transform->GetRight() * 100.f;
	line.color = { 1,0,0,1 };
	lineDrawer.Draw(line);

	//mySelectedGameObject->ImguiUpdate();
	ImGui::SeparatorText(mySelectedGameObject->myName.c_str());
	ImGui::Text("ID: %d", mySelectedGameObject->myID);
	ImGui::SameLine();
	ImGui::Checkbox("Active", &mySelectedGameObject->myActive);
	ImGui::Text("ParentID: %d", mySelectedGameObject->parentID);

	//Transform
	{
		//Position
		{
			Vector3f positionVec = transform->GetPosition();

			float position[3] = {
				positionVec.x,
				positionVec.y,
				positionVec.z
			};
			ImGui::DragFloat3("Position", position, 1.0f);
			if (position[0] != positionVec.x || position[1] != positionVec.y || position[2] != positionVec.z)
			{
				transform->SetPosition(Vector3f(position[0], position[1], position[2]));
			}
		}

		//Rotation
		{
			//ImGui::BeginDisabled();
			Vector3f rotationVec = transform->GetRotation();

			float rotation[3] = {
				rotationVec.x,
				rotationVec.y,
				rotationVec.z
			};
			ImGui::DragFloat3("Rotation", rotation, 1.0f);

			if (rotation[0] != rotationVec.x)
			{
				transform->RotateAroundX(rotation[0] - rotationVec.x);
			}
			if (rotation[1] != rotationVec.y)
			{
				transform->RotateAroundY(rotation[1] - rotationVec.y);
			}
			if (rotation[2] != rotationVec.z)
			{
				transform->RotateAroundZ(rotation[2] - rotationVec.z);
			}

			//if (rotation[0] != rotationVec.x || rotation[1] != rotationVec.y || rotation[2] != rotationVec.z)
			//{
			//	transform->SetRotation(Vector3f(rotation[0], rotation[1], rotation[2]));
			//}

			//ImGui::EndDisabled();
		}

		//Scale
		{
			Vector3f scaleVec = transform->GetScale();

			float scale[3] = {
				scaleVec.x,
				scaleVec.y,
				scaleVec.z
			};
			ImGui::DragFloat3("Scale", scale, 0.01f, 0.0001f, 10000.0f);
			if (scale[0] != scaleVec.x || scale[1] != scaleVec.y || scale[2] != scaleVec.z)
			{
				transform->SetScale(Vector3f(scale[0], scale[1], scale[2]));
			}

		}

		ImGui::SameLine();
		if (ImGui::Button("Reset"))
		{
			mySelectedGameObject->Reset();
		}
	}
	//Components
	{
		ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_Reorderable;
		ImGui::BeginTabBar("Components", tab_bar_flags);
		for (auto& c : mySelectedGameObject->myComponents)
		{
			c->ImguiUpdate();
		}
		ImGui::EndTabBar();
	}
}

void EditorState::UpdateUIEditorWindow(ImGuiWindowFlags window_flags)
{
	window_flags;
	if (ImGui::Begin("UI Editor", 0, window_flags))
	{
		if (!layoutChanged)
		{
			myUIEditorPos.x = ImGui::GetWindowPos().x;
			myUIEditorPos.y = ImGui::GetWindowPos().y;
			myUIEditorSize.x = ImGui::GetWindowSize().x;
			myUIEditorSize.y = ImGui::GetWindowSize().y;
		}
		ImGui::SetWindowPos(ImVec2(myUIEditorPos.x, myUIEditorPos.y));
		ImGui::SetWindowSize(ImVec2(myUIEditorSize.y, myUIEditorSize.y));

		if (ImGui::Button("Create New UI"))
		{
			myUIHandlers.push_back(UIHandler());
			myUIHandlers.back().Init("New UI Layout");
			myUIHandlers.back().ReadUILayout();
		}
		ImGui::SameLine();
		if (ImGui::Button("Save UI"))
		{
			for (size_t i = 0; i < myUIHandlers.size(); i++)
			{
				if (myUIHandlers[i].IsVisible())
				{
					myUIHandlers[i].SaveUILayout();
				}
			}
		}

		Vector2f sizepos = Vector2f(10.f, 10.f);
		Vector2f sizepos1 = Vector2f(1.f, 1.f);


		//show the ui files in a collapsing header
		for (int i = 0; i < myUIHandlers.size(); i++)
		{
			ImGui::PushID(i);
			ImGui::Checkbox(("##" + myUIHandlers[i].GetName()).c_str(), &myUIHandlers[i].IsVisible());
			ImGui::SameLine();
			if (ImGui::CollapsingHeader(myUIHandlers[i].GetName().c_str()))
			{
				//add sprite to the ui file
				if (ImGui::Button("Add Element"))
				{
					myUIHandlers[i].CreateUIElement();
				}

				ImGuiTreeNodeFlags tree_flags = 0;
				tree_flags |= ImGuiTreeNodeFlags_OpenOnArrow;
				tree_flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
				tree_flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
				if (myUIHandlers[i].GetElements().size() == 0)
				{
					tree_flags |= ImGuiTreeNodeFlags_Leaf;
				}

				//show all the elements in the file
				std::vector<UIElement*> elements = myUIHandlers[i].GetElements();
				for (int j = 0; j < elements.size(); j++)
				{
					if (ImGui::Selectable((elements[j]->GetName() + "##" + std::to_string(j)).c_str()))
					{
						mySelectedElement = elements[j];
						mySelectedUIHandler = i;
						myUIInspectorActive = UIInspectorActive::eUIElement;
					}
				}
			}
			ImGui::PopID();
		}
	}
	ImGui::End();
}

void EditorState::UpdateUIInspector(ImGuiWindowFlags window_flags)
{
	if (ImGui::Begin("UI Inspector", 0, window_flags))
	{
		if (!layoutChanged)
		{
			myUIInspectorPos.x = ImGui::GetWindowPos().x;
			myUIInspectorPos.y = ImGui::GetWindowPos().y;
			myUIInspectorSize.x = ImGui::GetWindowSize().x;
			myUIInspectorSize.y = ImGui::GetWindowSize().y;
		}
		ImGui::SetWindowPos(ImVec2(myUIInspectorPos.x, myUIInspectorPos.y));
		ImGui::SetWindowSize(ImVec2(myUIInspectorSize.x, myUIInspectorSize.y));
		switch (myUIInspectorActive)
		{
		case UIInspectorActive::eNone:
			//Empty
			break;
		case UIInspectorActive::eUIElement:
			if (mySelectedElement == nullptr)
			{
				myUIInspectorActive = UIInspectorActive::eNone;
				break;
			}
			UpdateUIInspectorElement();
			break;
		default:
			break;
		}
	}
	ImGui::End();

}

void EditorState::UpdateUIInspectorElement()
{
	//Show active gameObject
	//mySelectedElement->ImguiUpdate();
	ImGui::SeparatorText(mySelectedElement->GetName().c_str());
	//change name on the element
	char text[128];
	strcpy_s(text, mySelectedElement->GetName().c_str());
	ImGui::InputText("Name", text, 256);
	mySelectedElement->SetName(text);

	ImGui::Checkbox("Active", &mySelectedElement->IsActive());

	ImGui::Text("ID: %d", mySelectedElement->GetID());

	//move to front /back / forward/backward
	//in a dropdown
	if (ImGui::Button("Move To"))
	{
		ImGui::OpenPopup("MoveTO");
	}
	if (ImGui::BeginPopup("MoveTO"))
	{
		if (ImGui::Selectable("Front"))
		{
			myUIHandlers[mySelectedUIHandler].MoveElementToFront(mySelectedElement->GetID());
		}
		if (ImGui::Selectable("Back"))
		{
			myUIHandlers[mySelectedUIHandler].MoveElementToBack(mySelectedElement->GetID());
		}
		if (ImGui::Selectable("Forward"))
		{
			myUIHandlers[mySelectedUIHandler].MoveElementForward(mySelectedElement->GetID());
		}
		if (ImGui::Selectable("Backward"))
		{
			myUIHandlers[mySelectedUIHandler].MoveElementBackward(mySelectedElement->GetID());
		}
		ImGui::EndPopup();
	}
	ImGui::SameLine();

	if (ImGui::Button("Delete"))
	{
		ImGui::OpenPopup("Del");
	}
	if (ImGui::BeginPopup("Del"))
	{
		if (ImGui::Selectable("Yes"))
		{
			myUIHandlers[mySelectedUIHandler].RemoveElement(mySelectedElement->GetID());
		}
		if (ImGui::Selectable("No"))
		{
			//Do nothing
		}
		ImGui::EndPopup();
	}

	//Transform
	{
		//Position
		{
			Vector2f positionVec = mySelectedElement->GetPosition();

			float position[2] = {
				positionVec.x,
				positionVec.y
			};
			ImGui::DragFloat2("Position", position, 1.0f);
			if (position[0] != positionVec.x || position[1] != positionVec.y)
			{
				mySelectedElement->SetPosition(Vector2f(position[0], position[1]));
			}
		}

		//Scale
		{
			Vector2f scaleVec = mySelectedElement->GetSize();

			float scale[2] = {
				scaleVec.x,
				scaleVec.y
			};
			ImGui::DragFloat2("Scale", scale, 0.01f, 0.0001f, 10000.0f);
			if (scale[0] != scaleVec.x || scale[1] != scaleVec.y)
			{
				mySelectedElement->SetSize(Vector2f(scale[0], scale[1]));
			}
		}

		//rotation
		{
			float rotation = mySelectedElement->GetRotation();
			ImGui::DragFloat("Rotation", &rotation, 0.1f, 0.0f, 360.0f);
			if (rotation != mySelectedElement->GetRotation())
			{
				mySelectedElement->SetRotation(rotation);
			}
		}

		//ImGui::Checkbox("Override", &mySelectedElement->myOverride);
		//ImGui::SameLine();
		//if (ImGui::Button("Reset"))
		//{
		//	mySelectedElement->Reset();
		//}
	}
	//add componets
	{
		if (ImGui::Button("Add Component"))
		{
			ImGui::OpenPopup("AddComponent");
		}
		if (ImGui::BeginPopup("AddComponent"))
		{
			if (ImGui::Selectable("sprite"))
			{
				mySelectedElement->AddComponent(new UISprite("../assets/SplashScreen/engine_logo.dds", mySelectedElement));
			}

			if (ImGui::Selectable("spriteMorph"))
			{
				mySelectedElement->AddComponent(new UISpriteMorph("../assets/SplashScreen/engine_logo.dds", mySelectedElement));
			}

			if (ImGui::Selectable("button"))
			{
				std::vector<std::string> buttonTextures;
				buttonTextures.push_back("../assets/SplashScreen/rotate_around_mittens.dds");
				mySelectedElement->AddComponent(new UIButton(buttonTextures, mySelectedElement));
			}
			if (ImGui::Selectable("text"))
			{
				mySelectedElement->AddComponent(new UIText("test", mySelectedElement));
			}

			if (ImGui::Selectable("textField"))
			{
				mySelectedElement->AddComponent(new UITextField("test", mySelectedElement));
			}

			if (ImGui::Selectable("spriteSheet"))
			{
				mySelectedElement->AddComponent(new UISpriteSheet("../assets/SplashScreen/engine_logo.dds", mySelectedElement));
			}

			ImGui::EndPopup();
		}
	}
	ImGui::SameLine();
	//remove component
	{
		if (ImGui::Button("Remove Component"))
		{
			ImGui::OpenPopup("RemoveComponent");
		}
		if (ImGui::BeginPopup("RemoveComponent"))
		{
			for (auto& c : mySelectedElement->GetComponents())
			{
				if (ImGui::Selectable(c->GetName().c_str()))
				{
					mySelectedElement->RemoveComponent(c);
					break;
				}
			}
			ImGui::EndPopup();
		}
	}

	//Components
	{
		ImGui::PushID(mySelectedElement->GetID());
		ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_Reorderable;
		ImGui::BeginTabBar("Components", tab_bar_flags);
		for (auto& c : mySelectedElement->GetComponents())
		{
			c->ImguiUpdate();
			ImGui::Separator();
		}
		ImGui::EndTabBar();
		ImGui::PopID();
	}

}

void EditorState::UpdateDeferredWindow(ImGuiWindowFlags window_flags)
{
	auto& gBuffer = graphicsEngine->GetGBuffer();

	if (ImGui::Begin("Deferred View", 0, window_flags))
	{
		if (!layoutChanged)
		{
			myDeferredPos.x = ImGui::GetWindowPos().x;
			myDeferredPos.y = ImGui::GetWindowPos().y;
			myDeferredSize.x = ImGui::GetWindowSize().x;
			myDeferredSize.y = ImGui::GetWindowSize().y;
		}
		ImGui::SetWindowPos(ImVec2(myDeferredPos.x, myDeferredPos.y));
		ImGui::SetWindowSize(ImVec2(myDeferredSize.y, myDeferredSize.y));
		ImGui::Columns(2);
		ImGui::Text("#0 World Position");
		ImGui::Image(gBuffer.mySRVs[0].Get(), ImVec2(256, 144));
		ImGui::Text("#1 Albedo");
		ImGui::Image(gBuffer.mySRVs[1].Get(), ImVec2(256, 144));
		ImGui::Text("#2 Normal");
		ImGui::Image(gBuffer.mySRVs[2].Get(), ImVec2(256, 144));
		ImGui::NextColumn();
		ImGui::Text("#3 Material");
		ImGui::Image(gBuffer.mySRVs[3].Get(), ImVec2(256, 144));
		ImGui::Text("#4 Emissive");
		ImGui::Image(gBuffer.mySRVs[4].Get(), ImVec2(256, 144));
		ImGui::Text("#5 ShadowAtlas");
		ImGui::Image(graphicsEngine->GetShadowMap().mySRV.Get(), ImVec2(256, 144));
	}
	ImGui::End();
}

//void EditorState::UpdateDebugWindow(ImGuiWindowFlags)
//{
//	//if (ImGui::Begin("Debug", 0, window_flags))
//	//{
//	//	if (!layoutChanged)
//	//	{
//	//		myDebugPos.x = ImGui::GetWindowPos().x;
//	//		myDebugPos.y = ImGui::GetWindowPos().y;
//	//		myDebugSize.x = ImGui::GetWindowSize().x;
//	//		myDebugSize.y = ImGui::GetWindowSize().y;
//	//	}
//	//	ImGui::SetWindowPos(ImVec2(myDebugPos.x, myDebugPos.y));
//	//	ImGui::SetWindowSize(ImVec2(myDebugSize.x, myDebugSize.y));
//	//	ImGui::Text("FPS: %i", (int)latestFPS[dCount - 1]);
//	//	ImGui::PlotLines("##FPS", latestFPS, dCount, 0, "FPS", 0, 3000, ImVec2(240, 120));
//	//	ImGui::Checkbox("Pause FPS", &pauseFPS);
//	//	ImGui::Text("Average FPS: %i", (int)avgFPS);
//	//	//ImGui::BeginPlot
//	//	//ImGui::Text(("FPS: " + std::to_string((int)(1 / latestFPS))).c_str());
//	//}
//	//ImGui::End();
//}

void EditorState::UpdateWorldSettings(ImGuiWindowFlags window_flags)
{
	if (ImGui::Begin("World Settings", 0, window_flags))
	{
		Scene* scene = sceneManager->GetActiveScene();
		//if (ImGui::Button("DELETE"))
		//{
		//	MainSingleton::GetInstance().GetPhysXManager().ResetPhysXScene();
		//}
		//Clear Color
		ImGui::ColorEdit3("Clear Color", graphicsEngine->GetClearColor());
		if (ImGui::BeginItemTooltip())
		{
			ImGui::Text("Clear color will be saved with editor settings");
			ImGui::EndTooltip();
		}

		ImGui::Checkbox("Postprocess", &scene->myShouldRenderPostProcessing);

		if (ImGui::TreeNode("Postprocess setting files"))
		{
			static int selected = 0;
			for (int i = 0; i < 3; ++i)
			{
				char buff[32];
				sprintf(buff, "Postprocess file %d", i);
				if (ImGui::Selectable(buff, selected == i))
				{
					selected = i;
					if (graphicsEngine->myCurrentPostProcessingPath != graphicsEngine->myPostProccessArray[selected])
					{
						graphicsEngine->myCurrentPostProcessingPath = graphicsEngine->myPostProccessArray[selected];
						graphicsEngine->ReadPostprocessSettings(selected);
					}
				}
			}
			ImGui::TreePop();
		}

		ImGui::SliderFloat("BloomBlending", &graphicsEngine->GetBloomBlending(), 0, 1);
		ImGui::SliderFloat("Exposure", &graphicsEngine->GetExposure(), 0, 1);
		ImGui::SliderFloat("Saturation", &graphicsEngine->GetSaturation(), 0, 1);
		float contrast[3] = { graphicsEngine->GetContrast().x, graphicsEngine->GetContrast().y, graphicsEngine->GetContrast().z };
		if (ImGui::SliderFloat3("Contrast", contrast, 0, 1))
		{
			graphicsEngine->GetContrast().x = contrast[0];
			graphicsEngine->GetContrast().y = contrast[1];
			graphicsEngine->GetContrast().z = contrast[2];
		}
		float tint[3] = { graphicsEngine->GetTint().x, graphicsEngine->GetTint().y, graphicsEngine->GetTint().z };
		if (ImGui::SliderFloat3("Tint", tint, 0, 1))
		{
			graphicsEngine->GetTint().x = tint[0];
			graphicsEngine->GetTint().y = tint[1];
			graphicsEngine->GetTint().z = tint[2];
		}
		float blackpoint[3] = { graphicsEngine->GetBlackPoint().x, graphicsEngine->GetBlackPoint().y, graphicsEngine->GetBlackPoint().z };
		if (ImGui::SliderFloat3("Blackpoint", blackpoint, 0, 1))
		{
			graphicsEngine->GetBlackPoint().x = blackpoint[0];
			graphicsEngine->GetBlackPoint().y = blackpoint[1];
			graphicsEngine->GetBlackPoint().z = blackpoint[2];
		}
		if (ImGui::Button("Save postprocessing"))
		{
			graphicsEngine->SavePostprocessSettings(graphicsEngine->myCurrentPostProcessingPath);
		}
		//Cubemap
		ImGui::Text("CubeMap");
		if (ImGui::BeginItemTooltip())
		{
			ImGui::Text("Cubemap will not be saved with editor settings");
			ImGui::EndTooltip();
		}
		ImGui::Image(myDropTexture.GetShaderResourceView(), ImVec2(150, 150), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 1));
		//std::vector<Texture&> textures(myMeshes[i]->GetTextureCount());
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(".dds"))
			{
				std::string* payLoadPath = static_cast<std::string*>(payload->Data);
				std::string payLoadString = *payLoadPath;
				if (payLoadString == "")
				{
					try
					{
						payLoadString = std::any_cast<std::string>(MainSingleton::GetInstance().GetEventSystem().aTempData);
					}
					catch (const std::exception&)
					{
						LogWarning("Failed to cast any to string, you stupid");
					}

				}
				AmbientLight newAmbientLight;

				newAmbientLight.SetCubemap(StringHelper::s2ws(payLoadString));
				graphicsEngine->SetAmbientLight(newAmbientLight);
			}
			ImGui::EndDragDropTarget();
		}
		ImGui::Text("Sorry for saying stupid, love you <3");
		ImGui::SliderFloat("Physics Time Scale", &physicsTimeScale, 0.01f, 2.f);

	}
	ImGui::End();
}

void EditorState::UpdateParticleWindow(ImGuiWindowFlags window_flags)
{
	if (ImGui::Begin("Particle Window", 0, window_flags))
	{
		Scene* scene = sceneManager->GetActiveScene();
		ParticleSystem& tempParticleSystem = scene->GetParticle();

		bool hasChanged = false;

		{
			const ImGuiStyle defaultStyle = ImGui::GetStyle();
			ImGui::BeginGroup();
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { defaultStyle.ItemSpacing.x, defaultStyle.ItemSpacing.y * 2 });
			const char* enumNames[] = { "ExplosionEffect", "BurstEffect", "DustParticles","WaterFallIsh","FireEffect", "TrailEffect" };
			hasChanged = ImGui::Combo("Particle System Type", &tempParticleSystem.GetParticleSystemType(), enumNames, ParticleEffectType::eCount) || hasChanged;

			/*hasChanged =*/ ImGui::ColorEdit4("Color", (float*)&tempParticleSystem.GetColor()) || hasChanged;

			/*hasChanged =*/ ImGui::DragFloat3("Particle position", (float*)&tempParticleSystem.GetPosition(), 10.f) || hasChanged;

			if (tempParticleSystem.GetParticleSystemType() == ParticleEffectType::eExplosionEffect)
			{
				hasChanged = ImGui::DragFloat("Spread", tempParticleSystem.GetParticleSpread(), 10) || hasChanged;

				hasChanged = ImGui::DragFloat3("Velocity", (float*)&tempParticleSystem.GetVelocity(), 10.f) || hasChanged;

				hasChanged = ImGui::DragFloat3("Explostion Position", (float*)&tempParticleSystem.GetExplostionPosition(), 10.f) || hasChanged;

				hasChanged = ImGui::DragInt("Explosion Strength", (int*)&tempParticleSystem.GetExplosionStrength(), 10) || hasChanged;

				hasChanged = ImGui::DragInt("Explosion FalloffDistance", (int*)&tempParticleSystem.GetFalloffDistance(), 10) || hasChanged;
			}
			else if (tempParticleSystem.GetParticleSystemType() == ParticleEffectType::eBurstEffect)
			{

				hasChanged = ImGui::DragFloat("Spread", tempParticleSystem.GetParticleSpread(), 10) || hasChanged;

				hasChanged = ImGui::DragFloat3("Explostion direction", (float*)&tempParticleSystem.GetVelocity(), 10.f) || hasChanged;

				hasChanged = ImGui::DragFloat3("Explostion Position", (float*)&tempParticleSystem.GetExplostionPosition(), 10.f) || hasChanged;
			}
			else if (tempParticleSystem.GetParticleSystemType() == ParticleEffectType::eDustParticles)
			{

				hasChanged = ImGui::DragFloat("Spread", tempParticleSystem.GetParticleSpread(), 10) || hasChanged;

				hasChanged = ImGui::DragFloat3("Velocity", (float*)&tempParticleSystem.GetVelocity(), 10) || hasChanged;
			}
			else if (tempParticleSystem.GetParticleSystemType() == ParticleEffectType::eWaterFallEffect)
			{

				hasChanged = ImGui::DragFloat("Spread", tempParticleSystem.GetParticleSpread(), 10) || hasChanged;

				hasChanged = ImGui::DragFloat3("Velocity", (float*)&tempParticleSystem.GetVelocity(), 10) || hasChanged;

				ImGui::DragInt("Creation Limit / Create Call", (int*)&tempParticleSystem.GetMaxAmoutThatCanBeCreatedPerCreateCall(), 10);

				ImGui::DragFloat("Time between particle creation", (float*)&tempParticleSystem.GetParticleTime(), 10);
			}
			else if (tempParticleSystem.GetParticleSystemType() == ParticleEffectType::eFireEffect)
			{
				hasChanged = ImGui::DragFloat("Spread", tempParticleSystem.GetParticleSpread(), 10) || hasChanged;

				hasChanged = ImGui::DragFloat3("Velocity", (float*)&tempParticleSystem.GetVelocity(), 10) || hasChanged;
			}
			else if (tempParticleSystem.GetParticleSystemType() == ParticleEffectType::eTrailEffect)
			{
				const char* trailNames[] = { "Cone" , "Circle" };
				ImGui::Combo("Trail Effect Type", &tempParticleSystem.GetTrailEffectType(), trailNames, 2);

				ImGui::DragFloat3("Particle system rotation ", (float*)&tempParticleSystem.GetRotation(), 0.05f, -DirectX::XM_PI, DirectX::XM_PI) || hasChanged;

				ImGui::DragFloat("Angel Of Trail", (float*)&tempParticleSystem.GetTrailAngel(), 1.f, 0.0f, 360.f) || hasChanged;

				hasChanged = ImGui::DragFloat("Spread", tempParticleSystem.GetParticleSpread(), 10) || hasChanged;

				hasChanged = ImGui::DragFloat3("Velocity", (float*)&tempParticleSystem.GetVelocity(), 10) || hasChanged;

				ImGui::DragInt("Creation Limit / Create Call", (int*)&tempParticleSystem.GetMaxAmoutThatCanBeCreatedPerCreateCall(), 1);

				ImGui::DragFloat("Time between particle creation", (float*)&tempParticleSystem.GetParticleTime(), 1);

			}

			hasChanged = ImGui::DragFloat("Gravityforce of particles", &tempParticleSystem.GetGravityForce(), 10.f) || hasChanged;

			hasChanged = ImGui::DragFloat("Life time of particles", &tempParticleSystem.GetLifeTime(), 0.1f, 1000.f) || hasChanged;

			hasChanged = ImGui::DragInt("Number of particles", (int*)&tempParticleSystem.GetNumberOfParticles(), 50, 0, 250000) || hasChanged;

			hasChanged = ImGui::DragFloat("Size of Particles", &tempParticleSystem.GetSize(), 1, 2000) || hasChanged;
			ImGui::PopStyleVar();
			ImGui::EndGroup();
		}

		{
			ImGui::BeginGroup();
			hasChanged = ImGui::Button("Reset particles") || hasChanged;

			tempParticleSystem.SetRotation(tempParticleSystem.GetRotation());
			if (hasChanged)
			{
				tempParticleSystem.UpdateBuffers();
			}

			ImGui::SameLine();
			if (ImGui::Button("Save Particle System"))
			{
				ImGui::OpenPopup("Name Particle System File");
			}

			ImGui::SameLine();
			if (ImGui::Button("Load Particle System"))
			{
				ImGui::OpenPopup("Load Particle System File");
			}
			ImGui::EndGroup();
		}
		{ // Image
			float size = ImGui::GetContentRegionAvail().y;
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ((ImGui::GetContentRegionAvail().x / 2) - (size / 2)));
			ImGui::Image(tempParticleSystem.GetTextureC().GetShaderResourceView(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 1));

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(".dds"))
				{
					std::string* payLoadPath = static_cast<std::string*>(payload->Data);
					std::string payLoadString = *payLoadPath;
					if (payLoadString == "")
					{
						try
						{
							payLoadString = std::any_cast<std::string>(MainSingleton::GetInstance().GetEventSystem().aTempData);
							tempParticleSystem.SetTexturePath(payLoadString);
							tempParticleSystem.GetTextureC().Initialize(TextureInitializationType::DDS, tempParticleSystem.GetTexturePath(), false);
						}
						catch (const std::exception&)
						{
							LogWarning("Failed to cast any to string, you stupid");
						}

					}

				}
				ImGui::EndDragDropTarget();
			}
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(".dds"))
				{
					std::string* payLoadPath = static_cast<std::string*>(payload->Data);
					std::string payLoadString = *payLoadPath;
					if (payLoadString == "")
					{
						try
						{
							payLoadString = std::any_cast<std::string>(MainSingleton::GetInstance().GetEventSystem().aTempData);
							tempParticleSystem.SetTexturePath(payLoadString);
							tempParticleSystem.GetTextureC().Initialize(TextureInitializationType::DDS, tempParticleSystem.GetTexturePath(), false);
						}
						catch (const std::exception&)
						{
							LogWarning("Failed to cast any to string, you stupid");
						}

					}

				}
				ImGui::EndDragDropTarget();
			}
						if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(".dds"))
				{
					std::string* payLoadPath = static_cast<std::string*>(payload->Data);
					std::string payLoadString = *payLoadPath;
					if (payLoadString == "")
					{
						try
						{
							payLoadString = std::any_cast<std::string>(MainSingleton::GetInstance().GetEventSystem().aTempData);
							tempParticleSystem.SetTexturePath(payLoadString);
							tempParticleSystem.GetTextureC().Initialize(TextureInitializationType::DDS, tempParticleSystem.GetTexturePath(), false);
						}
						catch (const std::exception&)
						{
							LogWarning("Failed to cast any to string, you stupid");
						}

					}

				}
				ImGui::EndDragDropTarget();
			}
		}

		if (ImGui::BeginPopupModal("Load Particle System File"))
		{
			//std::vector<std::string> particleSystems;
			//std::vector<SceneData> sceneData;

			for (const auto& entry : std::filesystem::directory_iterator(STATIC_ASSET_PATH "json/ParticleSystems/"))
			{
				/*if (entry.is_directory())
					continue;*/

				if (entry.path().extension() != ".json")
					continue;

				{
					auto str = entry.path().filename().string();

					//printf_s("%s\n", str.c_str());

					if (ImGui::Button(entry.path().stem().string().c_str()))
					{
						tempParticleSystem.LoadParticleSystemSettings(entry.path().filename().string());
						ImGui::CloseCurrentPopup();
					}
				}
			}


			ImGui::SetCursorPosY(ImGui::GetWindowContentRegionMax().y - ImGui::GetCurrentWindow()->DC.PrevLineSize.y);
			if (ImGui::Button("Cancel", { ImGui::GetContentRegionAvail().x, 0 }))
				ImGui::CloseCurrentPopup();
			//if (ImGui::Button("Save particle system"))
			//{
			//	tempParticleSystem.LoadParticleSystemSettings();
			//	myFileName = "";
			//	ImGui::CloseCurrentPopup();
			//}

			ImGui::EndPopup();
		}

		if (ImGui::BeginPopupModal("Name Particle System File"))
		{

			ImGui::InputText("File Name", &myFileName);

			if (ImGui::Button("Save particle system"))
			{
				tempParticleSystem.SaveParticleSystemSettings(myFileName);
				myFileName = "";
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::Button("Cancel", { ImGui::GetContentRegionAvail().x, 0 }))
				ImGui::CloseCurrentPopup();

			ImGui::EndPopup();
		}

		//if ("You Stupid")
		//{
		//	LogInfo("You Stupid");
		//}

		//ImGui::Slider 
	}
	ImGui::End();
}

void EditorState::UpdateScriptWindow(ImGuiWindowFlags window_flags)
{
	if (ImGui::Begin("ScriptEditor", 0, window_flags))
	{
		myScriptEditor->Update();
		if (ImGui::Button("Play", ImVec2(120, 0)))
		{
			MainSingleton::GetInstance().GetSceneManager().GetActiveScene()->ReadNewScript();
		}
	}
	ImGui::End();
}

void EditorState::SaveEditorSettings()
{
	nlohmann::json json;
	json["hierarchy"]["show"] = myShowHierarchy;
	json["hierarchy"]["position"]["x"] = myHierarchyPos.x;
	json["hierarchy"]["position"]["y"] = myHierarchyPos.y;
	json["hierarchy"]["size"]["x"] = myHierarchySize.x;
	json["hierarchy"]["size"]["y"] = myHierarchySize.y;

	json["inspector"]["show"] = myShowInspector;
	json["inspector"]["position"]["x"] = myInspectorPos.x;
	json["inspector"]["position"]["y"] = myInspectorPos.y;
	json["inspector"]["size"]["x"] = myInspectorSize.x;
	json["inspector"]["size"]["y"] = myInspectorSize.y;

	json["deferred"]["show"] = myShowDeferred;
	json["deferred"]["position"]["x"] = myDeferredPos.x;
	json["deferred"]["position"]["y"] = myDeferredPos.y;
	json["deferred"]["size"]["x"] = myDeferredSize.x;
	json["deferred"]["size"]["y"] = myDeferredSize.y;

	json["uieditor"]["show"] = myShowUIEditorWindow;
	json["uieditor"]["position"]["x"] = myUIEditorPos.x;
	json["uieditor"]["position"]["y"] = myUIEditorPos.y;
	json["uieditor"]["size"]["x"] = myUIEditorSize.x;
	json["uieditor"]["size"]["y"] = myUIEditorSize.y;

	json["uiinspector"]["show"] = myShowUIInspectorWindow;
	json["uiinspector"]["position"]["x"] = myUIInspectorPos.x;
	json["uiinspector"]["position"]["y"] = myUIInspectorPos.y;
	json["uiinspector"]["size"]["x"] = myUIInspectorSize.x;
	json["uiinspector"]["size"]["y"] = myUIInspectorSize.y;

	json["particle"]["show"] = myShowParticleWindow;
	json["particle"]["position"]["x"] = myParticlePos.x;
	json["particle"]["position"]["y"] = myParticlePos.y;
	json["particle"]["size"]["x"] = myParticleSize.x;
	json["particle"]["size"]["y"] = myParticleSize.y;

	//json["debug"]["show"] = myShowDebugWindow;
	//json["debug"]["position"]["x"] = myDebugPos.x;
	//json["debug"]["position"]["y"] = myDebugPos.y;
	//json["debug"]["size"]["x"] = myDebugSize.x;
	//json["debug"]["size"]["y"] = myDebugSize.y;

	json["demo"]["show"] = myShowDemoWindow;

	json["lineDrawer"]["show"] = myShowDebugLines;

	json["worldSettings"]["show"] = myShowWorldSettings;
	float* color = graphicsEngine->GetClearColor();
	json["clearColor"]["r"] = color[0];
	json["clearColor"]["g"] = color[1];
	json["clearColor"]["b"] = color[2];

	std::ofstream file;
	file.open(STATIC_SETTINGS_PATH "EditorSettings.user");
	if (file.is_open())
	{
		file << json.dump(4);
	}
	file.close();
}

void EditorState::LoadEditorSettings()
{
	//GraphicsEngine* graphicsEngine = GraphicsEngine::GetInstance();
	std::ifstream file;
	file.open(STATIC_SETTINGS_PATH "EditorSettings.user");
	if (file.is_open())
	{
		nlohmann::json json;
		file >> json;
		try
		{
			myShowHierarchy = json["hierarchy"]["show"];
			myHierarchyPos.x = json["hierarchy"]["position"]["x"];
			myHierarchyPos.y = json["hierarchy"]["position"]["y"];
			myHierarchySize.x = json["hierarchy"]["size"]["x"];
			myHierarchySize.y = json["hierarchy"]["size"]["y"];
		}
		catch (const std::exception&)
		{
			LogError("Failed to load hierarchy editor settings");
		}

		try
		{
			myShowInspector = json["inspector"]["show"];
			myInspectorPos.x = json["inspector"]["position"]["x"];
			myInspectorPos.y = json["inspector"]["position"]["y"];
			myInspectorSize.x = json["inspector"]["size"]["x"];
			myInspectorSize.y = json["inspector"]["size"]["y"];
		}
		catch (const std::exception&)
		{
			LogError("Failed to load inspector editor settings");
		}

		try
		{
			myShowDeferred = json["deferred"]["show"];
			myDeferredPos.x = json["deferred"]["position"]["x"];
			myDeferredPos.y = json["deferred"]["position"]["y"];
			myDeferredSize.x = json["deferred"]["size"]["x"];
			myDeferredSize.y = json["deferred"]["size"]["y"];
		}
		catch (const std::exception&)
		{
			LogError("Failed to load deferred window editor settings");
		}
		try
		{
			myShowUIEditorWindow = json["uieditor"]["show"];
			myUIEditorPos.x = json["uieditor"]["position"]["x"];
			myUIEditorPos.y = json["uieditor"]["position"]["y"];
			myUIEditorSize.x = json["uieditor"]["size"]["x"];
			myUIEditorSize.y = json["uieditor"]["size"]["y"];
		}
		catch (const std::exception&)
		{
			LogError("Failed to load vfx window editor settings");
		}
		try
		{
			myShowUIInspectorWindow = json["uiinspector"]["show"];
			myUIInspectorPos.x = json["uiinspector"]["position"]["x"];
			myUIInspectorPos.y = json["uiinspector"]["position"]["y"];
			myUIInspectorSize.x = json["uiinspector"]["size"]["x"];
			myUIInspectorSize.y = json["uiinspector"]["size"]["y"];
		}
		catch (const std::exception&)
		{
			LogError("Failed to load vfx window editor settings");
		}


		try
		{
			myShowDemoWindow = json["demo"]["show"];
		}
		catch (const std::exception&)
		{
			LogError("Failed to load demo window editor settings");
		}

		try
		{
			myShowDebugLines = json["lineDrawer"]["show"];
		}
		catch (const std::exception&)
		{
			LogError("Failed to load line drawer editor settings");
		}

		try
		{
			myShowWorldSettings = json["worldSettings"]["show"];
			graphicsEngine->SetClearColor(json["clearColor"]["r"], json["clearColor"]["g"], json["clearColor"]["b"]);
		}
		catch (const std::exception&)
		{
			LogError("Failed to load world settings editor settings");
		}

		//try
		//{
		//	myShowDebugWindow = json["debug"]["show"];
		//	myDebugPos.x = json["debug"]["position"]["x"];
		//	myDebugPos.y = json["debug"]["position"]["y"];
		//	myDebugSize.x = json["debug"]["size"]["x"];
		//	myDebugSize.y = json["debug"]["size"]["y"];
		//}
		//catch (const std::exception&)
		//{
		//	LogError("Failed to load debug window editor settings");
		//}

		try
		{
			myShowParticleWindow = json["particle"]["show"];
			myParticlePos.x = json["particle"]["position"]["x"];
			myParticlePos.y = json["particle"]["position"]["y"];
			myParticleSize.x = json["particle"]["size"]["x"];
			myParticleSize.y = json["particle"]["size"]["y"];
		}
		catch (const std::exception&)
		{
			LogError("Failed to load particle window editor settings");
		}

		layoutChanged = true;
	}
	file.close();
}

void EditorState::HandleFileDrop(Message& aMessage)
{
	std::filesystem::path path = std::any_cast<std::filesystem::path>(aMessage.myData);
	std::string pathString = path.string();
	std::replace(pathString.begin(), pathString.end(), '\\', '/');
	if (path.extension() == ".fbx")
	{
		GameObject* newObject = new GameObject();
		newObject->myName = path.stem().string();

		std::vector<Mesh*> meshes = MainSingleton::GetInstance().GetModelFactory().Load(pathString, (std::string)("None"), true);
		MeshComponent* meshComponent = new MeshComponent(meshes);

		newObject->AddComponent<MeshComponent>(new MeshComponent(meshes));
		Scene* scene = sceneManager->GetActiveScene();
		scene->AddGameObject(newObject);

		meshComponent->SetActive(true);
		meshComponent->SetPath(pathString);

		LogSuccess("FBX file imported");
	}
	else if (path.extension() == ".json")
	{
		LogSuccess("JSON file imported");
	}
	else
	{
		LogWarning("File extension not recognized");
	}

}

void EditorState::HandleWonGame(Message&)
{
	//aMessage;
	myStateStack->PushState(eState::eStart);
	myStateStack->PushState(eState::eWin);
}

void EditorState::HandleLostGame(Message& aMessage)
{
	bool isFast = std::any_cast<bool>(aMessage.myData);
	myStateStack->PushState(eState::eStart);
	if (!isFast)
	{
		myStateStack->PushState(eState::eDeath);
	}
}

//void EditorState::TestFunction()
//{
//	// read a JSON file
//	std::ifstream file;
//	file.open(STATIC_SETTINGS_PATH "../test");
//	if (file.is_open())
//	{
//		nlohmann::json json;
//		file >> json;
//		try
//		{
//			//myShowHierarchy = json["hierarchy"]["show"];
//			//myHierarchyPos.x = json["hierarchy"]["position"]["x"];
//			//myHierarchyPos.y = json["hierarchy"]["position"]["y"];
//			//myHierarchySize.x = json["hierarchy"]["size"]["x"];
//			//myHierarchySize.y = json["hierarchy"]["size"]["y"];
//
//			std::vector<std::string> test;
//			test.resize(10);
//			for (size_t i = 0; i < json["score"].size(); i++)
//			{
//				test[i]	+= std::to_string(i);
//				test[i] += "        ";
//				test[i] += std::to_string((int)json["score"][i]["timems"]);
//				test[i] += "ms               ";
//				test[i] += json["score"][i]["usr"];
//			}
//			std::vector<UIElement*> elements = myUIHandlers[1].GetElements();
//			std::vector<UIElement*> numbElements;
//			numbElements.resize(8);
//			for (size_t i = 0; i < elements.size(); i++)
//			{
//				for (size_t j = 0; j < 9; j++)
//				{
//					if (elements[i]->GetName() == std::to_string(j+1))
//					{
//						numbElements[j] = elements[i];
//					}
//
//				}
//			}
//			for (size_t i = 0; i < numbElements.size(); i++)
//			{
//				numbElements[i]->GetComponent<UIText>()->SetText(test[i]);
//			}
//		}
//		catch (const std::exception&)
//		{
//			LogError("Failed to load hierarchy editor settings");
//		}
//	}
//	file.close();
//
//}
