#pragma once
#include "graphics/sprite/Sprite.h"
#include <graphics/Texture.h>

#include "graphics\Text\Text.h"

class StateStack;
class GameObject;
class Scene;
class Game
{
public:

	void Init();
	bool Update(const float& dt);
	void Render();

	void RecieveFileDrop(const std::string& aFilePath);

	~Game();

private:

	//Text* myText;

	Scene* myScene;
	StateStack* myStateStack;
	Texture myTexture;


#ifndef _RETAIL
	bool pauseFPS;
	static const int dCount = 240;
	float latestFPS[dCount];
	float latestMS[dCount];
	float updateRate = 1.f / 60.f;
	float timeSinceUpdate = 0.0f;
	float avgFPS = 0.0f;
	float avgMS = 0.0f;
#endif

};