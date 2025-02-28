#include "stdafx.h"

#include "DirectionalLightComponent.h"
#include "graphics/light/DirectionalLight.h"
#include "graphics/GraphicsEngine.h"
#include "gameObject/GameObject.h"

//DirectionalLightComponent::DirectionalLightComponent(DirectionalLight& aDirLight) 
//	: myDirLight(&aDirLight)
//{
//	myDirLight = new DirectionalLight();
//	myDirLight->SetColor(aDirLight.GetColor());
//	myDirLight->SetIntensity(aDirLight.GetIntensity());
//}

DirectionalLightComponent::DirectionalLightComponent(DirectionalLight* aDirLight) 
	: myDirLight(aDirLight)
{}

DirectionalLightComponent::~DirectionalLightComponent()
{
	delete myDirLight;
}

void DirectionalLightComponent::Update(const float /*aDeltaTime*/, Scene&)
{
	//myDirLight->SetTransform(*myParent->GetTransform());
	myDirLight->SetTransform(myParent->MultiplyWithParent());
	GraphicsEngine::GetInstance()->SetDirectionalLight(myDirLight);
}

void DirectionalLightComponent::ImguiUpdate()
{
	if (ImGui::BeginTabItem("Directional Light"))
	{
		if(ImGui::Button("Set as active light"))
		{
			GraphicsEngine::GetInstance()->SetDirectionalLight(myDirLight);
		}
		//Color
		Vector4f color = myDirLight->GetColor();
		float floatColor[4] = {color.x, color.y, color.z, color.w};
		ImguiNativeUpdate();
		ImGui::ColorEdit4("Color", floatColor);
		if (color.x != floatColor[0] || color.y != floatColor[1] || color.z != floatColor[2] || color.w != floatColor[3])
		{
			color = Vector4f(floatColor[0], floatColor[1], floatColor[2], floatColor[3]);
			myDirLight->SetColor(color);
		}
		ImGui::EndTabItem();
	};
}

void DirectionalLightComponent::Render()
{}

//void DirectionalLightComponent::SetLightProjectionMatrix(const DirectX::XMMATRIX& aProjectionMatrix)
//{
//	myDirLight->SetLightProjectionMatrix(aProjectionMatrix);
//}
