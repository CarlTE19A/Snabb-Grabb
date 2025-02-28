#pragma once
#include "VFXElement.h"

struct ObjectBufferData;

class Flipbook : public VFXElement
{
public:
	~Flipbook();
	bool Init(VFXName aName, std::string aTexturePath, int aRow, int aCol, int aMissing = 0, VFXType aType = VFXType::FlipbookType) override;
	bool Init(VFXName aName, std::string aModelPath, float aTime, VFXType aType = VFXType::ScrollMeshType, bool aAnimated = false) override;
	void Update(float aDeltaTime) override;
	//void Draw();
	void Play() override;
private:
	std::vector<UV> myUvs;
	int myRow;
	int myCol;
	int myMissing;
	//float myTimer;
	unsigned short myIndex;
	//bool myIsPlaying;
	//Sprite mySprite;
	TextureRect myTextureRect;
};
