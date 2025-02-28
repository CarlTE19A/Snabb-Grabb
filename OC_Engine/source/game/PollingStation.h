#pragma once

class GameObject;
class AmmoPool;
class Scene;
class PollingStation
{
public:
	PollingStation();
	inline void AddPlayer(GameObject* aPlayer){ myPlayer = aPlayer;}
	inline void AddAmmoPool(AmmoPool* aAmmoPool){ myAmmoPool = aAmmoPool;}
	inline GameObject* GetPlayer(){ return myPlayer;}
	inline AmmoPool* GetAmmoPool(){ return myAmmoPool; }
	~PollingStation();


private:
	//They destoys in another class 
	GameObject* myPlayer{};
	AmmoPool* myAmmoPool{};
};
