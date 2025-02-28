
#include "stdafx.h"

#include "InputMapper.h"
#include "InputObserver.h"
#include "InputWrapper.h"
#include "../../component/CameraComponent.h"
#include "../../math/Matrix.h"
#include <assert.h>
#include "gameObject\GameObject.h"
#include <ApplicationWindow\ApplicationWindow.h>

InputMapper::InputMapper()
{
}

InputMapper::~InputMapper()
{
}

void InputMapper::RunEvent(const eInputEvent aOutputEvent, std::any aValue)
{
	for (int i = 0; i < myObservers[aOutputEvent].size(); ++i)
	{
		myObservers[aOutputEvent][i]->RecieveEvent(aOutputEvent, aValue);
	}
}


void InputMapper::TranslateActionToEvent(const eInputAction aAction, const float aValue)
{
	const auto eventIterator = myEvents.find(aAction);
	if (eventIterator != myEvents.end())
	{
		RunEvent(myEvents[aAction], aValue);
	}
}

void InputMapper::TranslateActionToEvent(const eInputAction aAction, POINT aValue)
{
	const auto eventIterator = myEvents.find(aAction);
	if (eventIterator != myEvents.end())
	{
		RunEvent(myEvents[aAction], aValue);
	}
}

void InputMapper::UpdateKeyboardInput()
{
	if (myInputWrapper.IsKeyPressed('0'))
	{
		TranslateActionToEvent(eInputAction::eKey_0, myInputWrapper.GetMousePosition());
	}
	if (myInputWrapper.IsKeyPressed('1'))
	{
		TranslateActionToEvent(eInputAction::eKey_1, myInputWrapper.GetMousePosition());
	}
	if (myInputWrapper.IsKeyPressed('2'))
	{
		TranslateActionToEvent(eInputAction::eKey_2, 1.f);
	}
	if (myInputWrapper.IsKeyPressed('3'))
	{
		TranslateActionToEvent(eInputAction::eKey_3, 1.f);
	}
	if (myInputWrapper.IsKeyPressed('4'))
	{
		TranslateActionToEvent(eInputAction::eKey_4, 1.f);
	}
	if (myInputWrapper.IsKeyPressed('5'))
	{
		TranslateActionToEvent(eInputAction::eKey_5, 1.f);
	}
	if (myInputWrapper.IsKeyPressed('6'))
	{
		TranslateActionToEvent(eInputAction::eKey_6, 1.f);
	}
	if (myInputWrapper.IsKeyPressed('7'))
	{
		TranslateActionToEvent(eInputAction::eKey_7, 1.f);
	}
	if (myInputWrapper.IsKeyPressed('8'))
	{
		TranslateActionToEvent(eInputAction::eKey_8, 1.f);
	}
	if (myInputWrapper.IsKeyPressed('9'))
	{
		TranslateActionToEvent(eInputAction::eKey_9, 1.f);
	}
	if (myInputWrapper.IsKeyPressed('0'))
	{
		TranslateActionToEvent(eInputAction::eKey_0, 1.f);
	}
	if (myInputWrapper.IsKeyPressed('W'))
	{
		TranslateActionToEvent(eInputAction::eKey_W, 1.f);
	}	
	if (myInputWrapper.IsKeyPressed('S'))
	{
		TranslateActionToEvent(eInputAction::eKey_S, 1.f);
	}
	if (myInputWrapper.IsKeyPressed('A'))
	{
		TranslateActionToEvent(eInputAction::eKey_A, 1.f);
	}
	if (myInputWrapper.IsKeyPressed('D'))
	{
		TranslateActionToEvent(eInputAction::eKey_D, 1.f);
	}

	if (myInputWrapper.IsKeyHeld('W'))
	{
		TranslateActionToEvent(eInputAction::eKey_W_Held, 1.f);
	}
	if (myInputWrapper.IsKeyHeld('S'))
	{
		TranslateActionToEvent(eInputAction::eKey_S_Held, 1.f);
	}
	if (myInputWrapper.IsKeyHeld('A'))
	{
		TranslateActionToEvent(eInputAction::eKey_A_Held, 1.f);
	}
	if (myInputWrapper.IsKeyHeld('D'))
	{
		TranslateActionToEvent(eInputAction::eKey_D_Held, 1.f);
	}

	if (myInputWrapper.IsKeyPressed('Q'))
	{
		TranslateActionToEvent(eInputAction::eKey_Q, 1.f);
	}
	if (myInputWrapper.IsKeyPressed('F'))
	{
		TranslateActionToEvent(eInputAction::eKey_F, 1.f);
	}


	if (myInputWrapper.IsKeyPressed(VK_ESCAPE))
	{
		TranslateActionToEvent(eInputAction::eKey_Escape, 1.f);
	}
	if (myInputWrapper.IsKeyPressed(VK_RETURN))
	{
		TranslateActionToEvent(eInputAction::eKey_Enter, 1.f);
	}
	
	if (myInputWrapper.GetBeginWindowFocus())
	{
		TranslateActionToEvent(eInputAction::eFocusBegin, 1.f);
	}
	if (myInputWrapper.GetEndWindowFocus())
	{
		TranslateActionToEvent(eInputAction::eFocusEnd, 1.f);
	}


	//if (myInputWrapper.IsKeyPressed(VK_UP) == true)
	//{
	//	TranslateActionToEvent(eInputAction::eArrowKeyUp, 1.f);
	//}
	//if (myInputWrapper.IsKeyPressed(VK_RIGHT) == true)
	//{
	//	TranslateActionToEvent(eInputAction::eArrowKeyRight, 1.f);
	//}
	//if (myInputWrapper.IsKeyPressed(VK_LEFT) == true)
	//{
	//	TranslateActionToEvent(eInputAction::eArrowKeyLeft, 1.f);
	//}
	//if (myInputWrapper.IsKeyPressed(VK_DOWN) == true)
	//{
	//	TranslateActionToEvent(eInputAction::eArrowKeyDown, 1.f);
	//}
	//
	//if (myInputWrapper.IsKeyPressed(VK_TAB) == true)
	//{
	//	TranslateActionToEvent(eInputAction::eKey_Tab, 1.f);
	//}
	
	if (myInputWrapper.IsKeyPressed(VK_SPACE) == true)
	{
		TranslateActionToEvent(eInputAction::eKey_Space, 1.f);
	}
	
	//if (myInputWrapper.IsKeyPressed('E') == true)
	//{
	//	TranslateActionToEvent(eInputAction::eKey_E, 1.f);
	//}
	//if (myInputWrapper.IsKeyPressed('Q') == true)
	//{
	//	TranslateActionToEvent(eInputAction::eKey_Q, 1.f);
	//}
	//

	if (myInputWrapper.IsKeyPressed(VK_LBUTTON) == true)
	{
		TranslateActionToEvent(eInputAction::eMouseLeft, myInputWrapper.GetMousePosition());
		//TranslateActionToEvent(eInputAction::eMouseLeftHold, 1.f);
	}
	if (myInputWrapper.IsKeyReleased(VK_LBUTTON) == true)
	{
		TranslateActionToEvent(eInputAction::eMouseLeftReleased, 1.f);
		//TranslateActionToEvent(eInputAction::eMouseLeftHold, 0.f);
	}
	if (myInputWrapper.IsKeyHeld(VK_LBUTTON) == true)
	{
		TranslateActionToEvent(eInputAction::eMouseLeftHold, 1.f);
	}
	
	if (myInputWrapper.IsKeyPressed(VK_RBUTTON) == true)
	{
		TranslateActionToEvent(eInputAction::eMouseRight, myInputWrapper.GetMousePosition());
		//TranslateActionToEvent(eInputAction::eMouseRightHold, 1.f);
	}
	if (myInputWrapper.IsKeyReleased(VK_RBUTTON) == true)
	{
		TranslateActionToEvent(eInputAction::eMouseRightReleased, 1.f);
		//TranslateActionToEvent(eInputAction::eMouseRightHold, 0.f);
	}
	if (myInputWrapper.IsKeyHeld(VK_RBUTTON) == true)
	{
		TranslateActionToEvent(eInputAction::eMouseRightHold, 1.f);
	}
	
	
	//if (myInputWrapper.IsKeyHeld(VK_MBUTTON)==true)
	//{
	//	TranslateActionToEvent(eInputAction::eMouseMiddleButtonHold);
	//}
	
}

void InputMapper::Update()
{
	myInputWrapper.Update();
	UpdateKeyboardInput();
}

void InputMapper::Update(UINT message, WPARAM wParam, LPARAM lParam)
{
	myInputWrapper.UpdateEvents(message, wParam, lParam);
}

void InputMapper::MapEvent(const eInputAction aInputEvent, const eInputEvent aOutputEvent)
{
	if (myEvents.find(aInputEvent) == myEvents.end())
	{
		myEvents[aInputEvent] = aOutputEvent;
		return;

	}
	else
	{
		LogError("InputMapper::MapEvent: InputEvent already mapped!");
		return;
	}
}

void InputMapper::AddObserver(const eInputEvent aEventToListenFor, InputObserver* aObserver)
{
	assert(aObserver != nullptr && "InputObsever is nullptr!");
	myObservers[aEventToListenFor].push_back(aObserver);
}

void InputMapper::RemoveObserver(const eInputEvent aEventToListenFor, InputObserver* aObserver)
{
	assert(aObserver != nullptr && "InputObsever is nullptr!");
	auto it = std::find(myObservers[aEventToListenFor].begin(), myObservers[aEventToListenFor].end(), aObserver);
	if (it != myObservers[aEventToListenFor].end())
	{
		myObservers[aEventToListenFor].erase(it);
	}
}

void InputMapper::Init()
{
	myInputWrapper.Init(GetActiveWindow());
}

void InputMapper::UpdateHandler()
{
	myInputWrapper.Init(Engine::GetInstance()->GetApplicationWindow().GetWindowHandle());
}

Ray<float> InputMapper::RayCastFromMousePos(Vector2f aPosition)
{
	Ray<float> ray;
	aPosition;
	auto* camera = GraphicsEngine::GetInstance()->GetCamera();

	auto viewportDimensions = GraphicsEngine::GetInstance()->GetViewportDimensions();
	//std::cout << viewportDimensions.x << " " << viewportDimensions.y << std::endl;
	//std::cout << "Pos:" << aPosition.x << " " << aPosition.y << std::endl;
	auto nearPlaneDimensions = camera->GetNearPlaneDimensions();
	auto farPlaneDimensions = camera->GetFarPlaneDimensions();

	auto mousePosRatio = aPosition;
	mousePosRatio.x /= (float)viewportDimensions.x;
	mousePosRatio.y /= (float)viewportDimensions.y;

	float barYDiff = GraphicsEngine::GetInstance()->GetWindowSize().y / viewportDimensions.y;

	mousePosRatio -= { 0.5f, 0.5f * barYDiff * barYDiff };
	//std::cout << "Pos:" << mousePosRatio.x << " " << mousePosRatio.y << std::endl;
	Vector3f initialPos = camera->GetOwner()->GetTransform()->GetPosition();
	initialPos += camera->GetOwner()->GetTransform()->GetForward() * camera->GetNearPlane();

	//xNewDiff = viewportDimensions.y  / viewportDimensions.x;
	//yNewDiff = viewportDimensions.x / viewportDimensions.y;

	//std::cout << "barYDiff: " << barYDiff;
	//std::cout << ", barxDiff: " << barxDiff << std::endl;
	// magi???? vet inte varf?r just 0.65 och 0.56 fungerar men det g?r det och jag ska inte ifr?gas?tta det ?n :')
	Vector3f xDiff = camera->GetOwner()->GetTransform()->GetRight() * mousePosRatio.x * nearPlaneDimensions.x * 0.65f/* * barxDiff*/;
	Vector3f yDiff = camera->GetOwner()->GetTransform()->GetUp() * mousePosRatio.y * nearPlaneDimensions.y * 0.56f /** barYDiff*/;
	/*
	std::cout << "xDiff: " << xDiff.x << " " << xDiff.y << " " << xDiff.z << std::endl;
	std::cout << "yDiff: " << yDiff.x << " " << yDiff.y << " " << yDiff.z << std::endl;*/

	Vector3f startPos = initialPos + xDiff + yDiff;

	float xRatio = farPlaneDimensions.x / nearPlaneDimensions.x;
	float yRatio = farPlaneDimensions.y / nearPlaneDimensions.y;

	Vector3f endPos = initialPos;
	endPos += xDiff * xRatio;
	endPos += yDiff * yRatio;
	endPos += camera->GetOwner()->GetTransform()->GetForward() * camera->GetFarPlane();

	//Vector3f normalizedEndPos = endPos.GetNormalized();

	ray.InitWith2Points(startPos, endPos);

	return ray;
}

//Vector2<float> InputMapper::WorldToScreenSpace(const Vector3<float>& aPosition)
//{
//	auto& levelManager = MainSingleton::GetInstance().GetLevelManager();
//	Camera& aCamera = levelManager.GetLevel(levelManager.GetCurrentSceneIndex()).GetCamera();
//	auto pos = Vector4<float>(aPosition.x, aPosition.y, aPosition.z , 1.f);

//	Matrix4x4<float> view = aCamera.GetTransform().GetMatrix();
//	view = view.GetFastInverse(view);
//	//pos = view * pos;
//	pos = aCamera.GetProjection() * pos;

//	if (pos.w == 0.f) return Vector2<float>(0.f, 0.f);

//	Vector2<float> finalPos = Vector2<float>(pos.x / pos.w, pos.y / pos.w);

//	
//	auto windowSize = Vector2<float>(GraphicsEngine::GetInstance()->GetWindowWidth(), GraphicsEngine::GetInstance()->GetWindowHeight());
//	finalPos.x *= windowSize.x * 0.5f;
//	finalPos.y *= windowSize.y * 0.5f;
//	finalPos += { windowSize.x * 0.5f, windowSize.y * 0.5f };

//	return finalPos;
//}

//Vector3<float> InputMapper::ScreenToWorldSpace(const Vector2<float> aPosition)
//{
//	auto& levelManager = MainSingleton::GetInstance().GetLevelManager();
//	auto& cam = levelManager.GetLevel(levelManager.GetCurrentSceneIndex()).GetCamera();

//	float fov = cam.GetFOVRad();

//	Vector3<float> camPos = { levelManager.GetLevel(levelManager.GetCurrentSceneIndex()).GetCamera().GetTransform().GetPosition() };
//	Vector3<float> rayFrom = camPos;
//	Vector3<float> rayForward = { cam.GetCameraDirection() };

//	rayForward.Normalize();
//	float farPlane = 10000.0f;
//	rayForward *= farPlane;

//	Vector3<float> rightOffset;
//	Vector3<float> cameraUp = { 0,1,0 };
//		 
//	Vector3<float> vertical = cameraUp;
//	
//	Vector3<float> hor = rayForward.Cross(vertical);
//	hor.Normalize();
//	vertical = hor.Cross(rayForward);
//	vertical.Normalize();

//	float tanfov = tanf(0.5f * fov);

//	hor *= 2.f * farPlane * tanfov;
//	vertical *= 2.f * farPlane * tanfov;

//	float aspect;
//	float width = float(GraphicsEngine::GetInstance()->GetWindowWidth());
//	float height = float(GraphicsEngine::GetInstance()->GetWindowHeight());

//	aspect = width / height;

//	hor *= aspect;

//	Vector3<float> rayToCenter = rayFrom + rayForward;
//	Vector3<float> dHor = hor * 1.f / width;
//	Vector3<float> dVert = vertical * 1.f / height;


//	Vector3<float> rayTo = rayToCenter - 0.5f * hor + 0.5f * vertical;
//	rayTo += dHor * aPosition.x;
//	rayTo -= dVert * aPosition.y;
//	return rayTo;
//}