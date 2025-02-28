#pragma once
#include <functional>

class EnemysAction
{
public:

	EnemysAction(std::function<void(const float)>aFunction);
	void operator()(const float aDeltaTime);
	

private:
	const std::function<void(const float)> myActionFunction;
};

