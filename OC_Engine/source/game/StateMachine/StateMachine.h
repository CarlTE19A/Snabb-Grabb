#pragma once
#include "Condition.h"
#include "EnemysAction.h"
class StateMachine;

class Transition
{
public:
	Transition(StateMachine* aTargetState, Base_Condition* aCondition);
	~Transition();
	StateMachine* myTargetState;
	Base_Condition* myCondition;
};

class StateHandler;

class StateMachine
{
public:
	StateMachine(StateHandler* const aStateHandler, const EnemysAction& aEnemyAction);

	~StateMachine();

	void Update(const float aDeltaTime);

	void AddTransition(Transition* aTransition);

private:

	StateHandler* const myStateHandler;
	EnemysAction myEnemysAction;
	std::vector<Transition*>myTransitions;
};
