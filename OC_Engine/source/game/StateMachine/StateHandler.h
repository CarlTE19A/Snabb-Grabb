#pragma once
#include "DecisionMaker.h"
#include <vector>

class StateMachine;

class StateHandler : public DecisionMaker
{
public:
	StateHandler() = default;

	~StateHandler();

	virtual void Update(const float aDeltaTime)override;
	
	void ChangeState(StateMachine* aState);

	void AddState(StateMachine* aState);

private:

	StateMachine* myCurrentState;
	std::vector<StateMachine*> myStates;
};
