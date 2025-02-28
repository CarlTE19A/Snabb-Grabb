#pragma once
#include "InputMapperEvents.h"
#include "InputWrapper.h"
#include <unordered_map>
#include "../../graphics/GraphicsEngine.h"
#include "../../Engine.h"
#include "math/Ray.h"

#include "../../MainSingelton/MainSingelton.h"
#include <any>

class InputObserver;

class InputMapper
{
public:
	InputMapper();
	~InputMapper();

	void Update();
	void Update(UINT message, WPARAM wParam, LPARAM lParam);
	void MapEvent(const eInputAction aInputEvent, const eInputEvent aOutputEvent);
	void AddObserver(const eInputEvent aEventToListenFor, InputObserver* aObserver);
	void RemoveObserver(const eInputEvent aEventToListenFor, InputObserver* aObserver);
	void Init();
	void UpdateHandler();

	inline Vector2<float> GetMousePos()
	{
		Vector2<float> c = { (float)myInputWrapper.GetMousePosition().x, (float)myInputWrapper.GetMousePosition().y };
		c.y = GraphicsEngine::GetInstance()->GetWindowSize().y - c.y;
		return c;
	}
	inline Vector2<float> GetMouseDelta()
	{
		POINT p = myInputWrapper.GetMouseDelta();
		Vector2<float> c = { (float)p.x, (float)p.y };
		c.y = - c.y;
		return c;
	}

	inline float GetMouseWheelDelta()
	{
		return myInputWrapper.GetMouseWheelDelta();
	}

	Ray<float> RayCastFromMousePos(Vector2f aPosition);

	//Vector2<float> WorldToScreenSpace(const Vector3<float>& aPosition);
	//Vector3<float> ScreenToWorldSpace(const Vector2<float> aPosition);
	InputWrapper& GetInputWrapper() { return myInputWrapper; }
private:

	void RunEvent(const eInputEvent aOutputEvent, std::any aValue);
	void TranslateActionToEvent(const eInputAction aAction, const float aValue = 0);
	void TranslateActionToEvent(const eInputAction aAction, POINT aValue);

	void UpdateKeyboardInput();


	std::unordered_map<eInputEvent, std::vector<InputObserver*>>
		myObservers;

	std::unordered_map<eInputAction, eInputEvent>
		myEvents;

	InputWrapper myInputWrapper;

};