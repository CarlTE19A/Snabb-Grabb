//#pragma once
////#include "graphics\Shader.h"
////#include "graphics\Texture.h"
//#include <wrl/client.h>
//#include "VFXElement.h"
//
//
//class Mesh;
//
//class ScrollMesh : public VFXElement
//{
//public:
//ScrollMesh();
//	~ScrollMesh();
//	bool Init(VFXName aName, std::string aModelPath, float aTime, VFXType aType = VFXType::ScrollMeshType, bool aAnimated = false) override;
//	bool Init(VFXName aName, std::string aTexturePath, int aRow, int aCol, int aMissing = 0, VFXType aType = VFXType::FlipbookType) override;
//
//
//	void Update(float aDeltaTime) override;
//	//void Draw();
//	void Play() override;
//	//void ImguiUpdate() override
//	//{
//	//	//ImGui::Checkbox("IsPlaying", &myIsPlaying);
//
//	//	if (ImGui::Button("Play", ImVec2(50.f, 20.f)))
//	//	{
//	//		Play();
//	//	}
//
//	//	ImGui::Checkbox("Loop", &GetLoop());
//	//	ImGui::SliderFloat("PlayTime", &GetPlayTime(), 0, 10);
//	//	ImGui::SliderFloat("Intensity", &GetIntensity(), 0, 1);
//	//}
//private:
//
//	//ComPtr<ID3D11Buffer> myVFXBuffer;
//
//	//std::vector<Mesh*> myMesh;
//	//Mesh* myMesh2;
//	//float myTimer;
//	//float myPlayTime;
//	//bool myIsPlaying;
//	//float myIntensity;
//	//bool myLoop;
//};
