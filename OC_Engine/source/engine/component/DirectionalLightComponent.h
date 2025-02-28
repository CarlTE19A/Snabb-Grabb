#pragma once
#include "Component.h"

class DirectionalLight;

class DirectionalLightComponent : public Component
{
public:
	//DirectionalLightComponent(DirectionalLight& aDirLight);
	DirectionalLightComponent(DirectionalLight* aDirLight);
	~DirectionalLightComponent() override;

	void Update(const float aDeltaTime, Scene& aScene) override;

	void ImguiUpdate();

	void Render();
	inline DirectionalLight& GetDirectionalLight() { return *myDirLight; }
	//void SetLightProjectionMatrix(const DirectX::XMMATRIX& aProjectionMatrix);

	void Reset() override
	{
		//Does not need reset
	}
private:
	DirectionalLight* myDirLight;
};