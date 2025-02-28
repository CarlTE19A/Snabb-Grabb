#pragma once

class TriggerZoneComponent;
class Scene;

class Action
{
public:
	virtual void Activate(Scene& aScene) = 0;
	virtual void Reset() = 0;
	virtual bool Update(float deltaTime, Scene& aScene) = 0;
	virtual ~Action() { myKeyTargets.clear(); };
	void ActivateKeys();
	void AddKeyTarget(TriggerZoneComponent* atriggerZoneTarget);
	int order;
protected:
	bool myActive = true;
	std::vector<int> myKeyTargets;
private:
	friend class JsonSceneImporter;
};