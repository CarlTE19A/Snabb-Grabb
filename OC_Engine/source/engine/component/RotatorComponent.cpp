#include "stdafx.h"

#include "RotatorComponent.h"
#include "../gameObject/GameObject.h"

RotatorComponent::RotatorComponent()
{
	
}

RotatorComponent::~RotatorComponent()
{
}

void RotatorComponent::Update(const float aDeltaTime, Scene& aScene)
{
	if (!IsActive()) return;
	aDeltaTime;
	aScene;
	Transform* transform = myParent->GetTransform();
	transform->RotateAroundX(myRotation.x * aDeltaTime);
	transform->RotateAroundY(myRotation.y * aDeltaTime);
	transform->RotateAroundZ(myRotation.z * aDeltaTime);
}

void RotatorComponent::ImguiUpdate()
{
	if (ImGui::BeginTabItem("Rotator"))
	{
		ImguiNativeUpdate();
		ImGui::DragFloat3("Rotation", &myRotation.x, 0.1f);

		
		ImGui::EndTabItem();
	};
}