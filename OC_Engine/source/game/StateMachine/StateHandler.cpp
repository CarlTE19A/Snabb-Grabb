#include "StateHandler.h"
#include "StateMachine.h"

StateHandler::~StateHandler()
{
	for (StateMachine* state : myStates)
	{
		delete state;
	}
}

void StateHandler::Update(const float aDeltaTime)
{
	myCurrentState->Update(aDeltaTime);
}

void StateHandler::ChangeState(StateMachine* aState)
{
	myCurrentState = aState;
}

void StateHandler::AddState(StateMachine* aState)
{
	myStates.push_back(aState);
}
