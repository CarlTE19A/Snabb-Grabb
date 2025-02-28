#include "EnemysAction.h"

EnemysAction::EnemysAction(std::function<void(const float)> aFunction)
	:myActionFunction(aFunction)
{
}

void EnemysAction::operator()(const float aDeltaTime)
{
	myActionFunction(aDeltaTime);
}