#pragma once
#include "Action.h"

class PlaySoundAction : public Action
{
public:
	void Activate(Scene& aScene) override;
	bool Update(float deltaTime, Scene& aScene) override;
	~PlaySoundAction() override;
	
private:

};