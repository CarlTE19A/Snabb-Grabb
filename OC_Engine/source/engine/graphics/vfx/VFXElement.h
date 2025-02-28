//#pragma once
////#include "graphics\Shader.h"
////#include "graphics\Texture.h"
//#include <wrl/client.h>
//#include"../../external/imgui/imgui.h"
//#include "graphics\model\Mesh.h"
//#include "graphics\model\AnimatedMesh.h"
//
//using namespace Microsoft::WRL;
//
//struct ID3D11Buffer;
//
//struct ObjectBufferData;
//class Mesh;
//
//enum VFXType
//{
//	ScrollMeshType,
//	FlipbookType,
//};
//
//enum VFXName
//{
//	Bloodnova,
//	GroundCrack,
//	BloodSpike,
//	BloodSlash,
//	BloodAltar,
//};
//
//struct UV
//{
//	UV(Vector2f aStart, Vector2f aEnd) { myStart = aStart; myEnd = aEnd; }
//	Vector2f myStart;
//	Vector2f myEnd;
//};
//
//class VFXElement
//{
//	friend class EditorState;
//public:
//	VFXElement();
//	~VFXElement();
//	virtual bool Init(VFXName aName, std::string aModelPath, float aTime, VFXType aType = VFXType::ScrollMeshType, bool aAnimated = false) = 0;
//	virtual bool Init(VFXName aName, std::string aTexturePath, int aRow, int aCol, int aMissing = 0, VFXType aType = VFXType::FlipbookType) = 0;
//	virtual void Update(float aDeltaTime) = 0;
//	void Draw(Transform& aTransform);
//	virtual void Play() = 0;
//	std::vector<Mesh*> GetMeshes() { return myMesh; }
//	void SetIntensity(float aIntensity) { myIntensity = aIntensity; }
//	float& GetIntensity() { return myIntensity; }
//	void SetLoop(bool aLoop) { myLoop = aLoop; }
//	bool& GetLoop() { return myLoop; }
//	bool GetIsPlaying() { return myIsPlaying; }
//	void SetIsPlaying(bool aIsPlaying) { myIsPlaying = aIsPlaying; }
//	void SetPlayTime(float aPlayTime) { myPlayTime = aPlayTime; }
//	float& GetPlayTime() { return myPlayTime; }
//	void SetTimer(float aTimer) { myTimer = aTimer; }
//	float GetTimer() { return myTimer; }
//	void SetName(VFXName aName) { myName = aName; }
//	VFXName GetName() { return myName; }
//	void SetType(VFXType aType) { myType = aType; }
//	VFXType GetType() { return myType; }
//	void SetSprite(std::string aTexturePath) { mySprite.Init(aTexturePath); }
//	Sprite& GetSprite() { return mySprite; }	
//	void SetMeshes(std::vector<Mesh*> aMeshes) { myMesh = aMeshes; }
//	void SetAnimatedMeshes(std::vector<AnimatedMesh*> aMeshes) { myAnimatedMesh = aMeshes; }
//	
//	inline void SetBufferData(float time, float intensity);
//	
//	virtual void ImguiUpdate()
//	{
//		//ImGui::Checkbox("IsPlaying", &myIsPlaying);
//
//		if (ImGui::Button("Play", ImVec2(50.f, 20.f)))
//		{
//			Play();
//		}
//		ImGui::Checkbox("Loop", &myLoop);
//		ImGui::SliderFloat("PlayTime", &myPlayTime, 0, 10);
//		ImGui::SliderFloat("Intensity", &myIntensity, 0, 1);
//		if (myMesh.size() > 0)
//		{
//			ImGui::Text("Meshes");
//			for (int i = 0; i < myMesh.size(); ++i)
//			{
//				ImGui::Text(myMesh[i]->GetPath().c_str());
//			}
//		}
//		//if (mySprite.GetVertexBuffer() != nullptr)
//		//{
//		//	ImGui::Text("Sprite");
//		//	//ImGui::Text(mySprite->GetPath().c_str());
//		//}
//	}
//private:
//	ComPtr<ID3D11Buffer> myVFXBuffer;
//	std::vector<Mesh*> myMesh;
//	std::vector<AnimatedMesh*> myAnimatedMesh;
//	//Sprite mySprite;
//
//	float myTimer;
//	float myPlayTime;
//	bool myIsPlaying;
//	float myIntensity;
//	bool myLoop;
//
//	VFXName myName;
//	VFXType myType;
//};
