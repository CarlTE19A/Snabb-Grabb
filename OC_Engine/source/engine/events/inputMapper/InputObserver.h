#pragma once
#include "InputMapperEvents.h"
#include <any>

struct Position;

class InputObserver
{
public:
	InputObserver() = default;
	virtual	~InputObserver() = default;
	virtual void RecieveEvent(const eInputEvent aEvent, std::any aValue) = 0;
};