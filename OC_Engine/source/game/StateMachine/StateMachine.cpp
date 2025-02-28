#include "StateMachine.h"
#include "StateHandler.h"

Transition::Transition(StateMachine* aTargetState, Base_Condition* aCondition)
	:myTargetState(aTargetState)
	, myCondition(aCondition)
{
}

Transition::~Transition()
{
	delete myCondition;
}

StateMachine::StateMachine(StateHandler* const aStateHandler, const EnemysAction& aEnemyAction)
	:myStateHandler(aStateHandler)
	, myEnemysAction(aEnemyAction)
{
}

StateMachine::~StateMachine()
{
	for (Transition* transition : myTransitions)
	{
		delete transition;
	}
}

void StateMachine::Update(const float aDeltaTime)
{
	myEnemysAction(aDeltaTime);
	for (const Transition* transition : myTransitions)
	{
		if (transition->myCondition->Try())
		{
			myStateHandler->ChangeState(transition->myTargetState);
			return;
		}
	}
}

void StateMachine::AddTransition(Transition* aTransition)
{
	myTransitions.push_back(aTransition);
}
