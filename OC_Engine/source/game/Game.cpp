#include "Game.h"

#define IMGUI_DEFINE_MATH_OPERATORS

#include "state/StateStack.h"
#include "state/State.h"
#include "state/GameState.h"
#include "state/MenuState.h"
#include "state/CreditsState.h"
#include "graphics\GraphicsEngine.h"
#include "graphics\Text\TextService.h"
#include <imgui/imgui.h>
#include "component\CameraComponent.h"
#include "gameObject\GameObject.h"
#include "mainSingelton/MainSingelton.h"
#include "scene/SceneManager.h"
#include "graphics\sprite\SpriteDrawer.h"
#include "component\mesh\MeshComponent.h"
#include "graphics\model\ModelFactory.h"
#include "graphics\model\ModelDrawer.h"
#include "graphics\model\ModelShader.h"
#include "Paths.h"
#include "Log.h"
#include "scene\Scene.h"
#include "PhysXCode\PhysXManager.h"

Game::~Game()
{
	delete myStateStack;
}

void Game::Init()
{
	myStateStack = new StateStack();
#ifdef _DEBUG
	myStateStack->PushState(eState::eEditor);
#else
	myStateStack->PushState(eState::eSplashScreen);
#endif // DEBUG

	AmbientLight ambLight;
	std::wstring cubeMap = L"../assets/dikhololo_night_8k_cube_specular.dds";
	ambLight.SetCubemap(cubeMap);
	myTexture.Initialize(TextureInitializationType::DDS, "../assets/m_perlin.dds", false);
	GraphicsEngine::GetInstance()->SetAmbientLight(ambLight);

	//myText = new Text();
	//myText->Init(FontSize_48);
	//myText->SetText("a A Hello World");
	//myText->SetOriginInMiddle();
}

bool Game::Update(const float& dt)
{
#ifndef _RETAIL
	//ONLY For Math
	if (!pauseFPS)
	{
		timeSinceUpdate += dt;
		if (timeSinceUpdate > updateRate)
		{
			timeSinceUpdate = 0.0f;
			float sumFPS = 0;
			float sumMS = 0;
			for (int i = 0; i < dCount; ++i)
			{
				latestFPS[i] = latestFPS[i + 1];
				sumFPS += latestFPS[i];

				latestMS[i] = latestMS[i + 1];
				sumMS += latestMS[i];
			}
			latestFPS[dCount - 1] = 1 / dt;
			avgFPS = sumFPS / dCount;
			latestMS[dCount - 1] = dt;
			avgMS = sumMS / dCount;
		}
	}
#ifdef _DEBUG
	if (ImGui::Begin("Debug", 0))
	{
		//ImGui::Text("FPS: %i", (int)latestFPS[dCount - 1]);
		ImGui::Text("FPS: %f", latestFPS[dCount - 1]);
		ImGui::SameLine();
		ImGui::Text("ms: %f", latestMS[dCount - 1]);
		ImGui::PlotLines("##FPS", latestFPS, dCount, 0, "FPS", 0, 3000, ImVec2(240, 120));
		ImGui::Checkbox("Pause FPS", &pauseFPS);
		ImGui::Text("Average FPS: %f", avgFPS);
		ImGui::Text("Avrage ms: %f", avgMS);
		//ImGui::BeginPlot
		//ImGui::Text(("FPS: " + std::to_string((int)(1 / latestFPS))).c_str());
	}
	ImGui::End();
#endif
#ifdef _RELEASE
	//LogInfo("FPS: " + std::to_string((int)latestFPS[dCount - 1]) + " | Avg FPS: " + std::to_string(avgFPS) + " | ms " + std::to_string(dt * 1000));
#endif
#endif

	return myStateStack->UpdateState(dt);
}

void Game::Render()
{
	//GraphicsEngine::GetInstance()->GetModelDrawer().Draw(*myGameObject->GetComponent<MeshComponent>(), GraphicsEngine::GetInstance()->GetModelDrawer().GetUnlitShader());
	myTexture.Bind(6, GraphicsEngine::GetInstance()->GetContext());
	myStateStack->RenderState();
	//myText->Draw();
	SceneManager* sceneManager = &MainSingleton::GetInstance().GetSceneManager();
	if (sceneManager->GetRequestedSceneChange())
	{
		sceneManager->SetActiveScene(sceneManager->GetToBeLoadedSceneName());
	}
}

