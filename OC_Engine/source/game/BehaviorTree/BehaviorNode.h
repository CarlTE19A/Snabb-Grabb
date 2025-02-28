#pragma once

class EnemyComponent;

class Scene;
class BehaviorNode
{
public:

	BehaviorNode(EnemyComponent* aActor);
	virtual~BehaviorNode() = 0;


	virtual bool Execute(const float aDeltaTime, Scene& aScene) = 0;

	
protected:
	//Don't delete
	EnemyComponent* myActor;

};
