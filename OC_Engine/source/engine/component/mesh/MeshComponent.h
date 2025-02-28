#pragma once
#include "../Component.h"
#include "graphics/model/Mesh.h"

class ModelShader;

class MeshComponent : public Component
{
public:
	MeshComponent(std::vector<Mesh*> aMesh);
	~MeshComponent() override;

	void Update(const float aDeltaTime, Scene& aScene) override;

	void ImguiUpdate();

	void Render(ModelShader& aModelShader);

	std::vector<Mesh*>& GetMesh();

	void SetPath(std::string aPath);
	std::string GetPath();
	bool GetIsHighLighted() { return IshighLighted; }
	void SetIsHighLighted(const bool aBool);

	void Reset() override
	{
		//Does not need reset
	}

private:
	std::vector<Mesh*> myMeshes;
	std::string path;
	bool IshighLighted;
};

//!Sets all mesh Transform to the same