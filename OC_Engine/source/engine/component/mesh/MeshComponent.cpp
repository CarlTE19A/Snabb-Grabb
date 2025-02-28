#include "stdafx.h"

#include "MeshComponent.h"
#include "gameObject\GameObject.h"
#include "graphics\model\ModelShader.h"

//ImGui stuff ._.
#include "mainSingelton\MainSingelton.h"
#include "events\EventSystem.h"

MeshComponent::MeshComponent(std::vector<Mesh*> aMesh) : myMeshes(aMesh) { }
MeshComponent::~MeshComponent()
{
	//for (Mesh* mesh : myMeshes)
	//{
	//	delete mesh;
	//};
	myMeshes.clear();
}

void MeshComponent::ImguiUpdate()
{
	if (ImGui::BeginTabItem("Mesh"))
	{
		ImguiNativeUpdate();
		ImVec2 size = ImVec2(200, 200);
		for (int i = 0; i < myMeshes.size(); i++)
		{
			//if (ImGui::BeginListBox(("##" + std::to_string(i+5)).c_str(), size))
			if(ImGui::TreeNode(("Mesh: " + std::to_string(i)).c_str()))
			{
				ImGui::Text("Albedo");
				ImGui::Image(myMeshes[i]->GetTexture(0).GetShaderResourceView(), ImVec2(150, 150), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 1));
				//std::vector<Texture&> textures(myMeshes[i]->GetTextureCount());
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(".dds"))
					{
						Texture texture;
						std::string* payLoadPath = static_cast<std::string*>(payload->Data);
						std::string payLoadString = *payLoadPath;
						if (payLoadString == "")
						{
							try
							{
								payLoadString = std::any_cast<std::string>(MainSingleton::GetInstance().GetEventSystem().aTempData);
							}
							catch (const std::exception&)
							{
								LogWarning("Failed to cast any to string, you stupid");
							}
							
						}
						texture.Initialize(TextureInitializationType::DDS, payLoadString, false);
						myMeshes[i]->ChangeMaterial(texture, 0);
					}
					ImGui::EndDragDropTarget();
				}


				ImGui::Text("Normal");
			if(&myMeshes[i]->GetTexture(1) != nullptr)
			{
				ImGui::Image(myMeshes[i]->GetTexture(1).GetShaderResourceView(), ImVec2(150, 150), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 1));
			}
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(".dds"))
					{
						Texture texture;
						std::string* payLoadPath = static_cast<std::string*>(payload->Data);
						std::string payLoadString = *payLoadPath;
						if (payLoadString == "")
						{
							try
							{
								payLoadString = std::any_cast<std::string>(MainSingleton::GetInstance().GetEventSystem().aTempData);
							}
							catch (const std::exception&)
							{
								LogWarning("Failed to cast any to string, you stupid");
							}

						}
						texture.Initialize(TextureInitializationType::DDS, payLoadString, false);
						myMeshes[i]->ChangeMaterial(texture, 1);
					}
					ImGui::EndDragDropTarget();
				}

				ImGui::Text("Material");
				if (&myMeshes[i]->GetTexture(2) != nullptr)
				{
					ImGui::Image(myMeshes[i]->GetTexture(2).GetShaderResourceView(), ImVec2(150, 150), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 1));
				}
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(".dds"))
					{
						Texture texture;
						std::string* payLoadPath = static_cast<std::string*>(payload->Data);
						std::string payLoadString = *payLoadPath;
						if (payLoadString == "")
						{
							try
							{
								payLoadString = std::any_cast<std::string>(MainSingleton::GetInstance().GetEventSystem().aTempData);
							}
							catch (const std::exception&)
							{
								LogWarning("Failed to cast any to string, you stupid");
							}

						}
						texture.Initialize(TextureInitializationType::DDS, payLoadString, false);
						myMeshes[i]->ChangeMaterial(texture, 2);
					}
					ImGui::EndDragDropTarget();
				}

				ImGui::Text("Fx");
				if (&myMeshes[i]->GetTexture(3) != nullptr)
				{
					ImGui::Image(myMeshes[i]->GetTexture(3).GetShaderResourceView(), ImVec2(150, 150), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 1));
				}
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(".dds"))
					{
						Texture texture;
						std::string* payLoadPath = static_cast<std::string*>(payload->Data);
						std::string payLoadString = *payLoadPath;
						if (payLoadString == "")
						{
							try
							{
								payLoadString = std::any_cast<std::string>(MainSingleton::GetInstance().GetEventSystem().aTempData);
							}
							catch (const std::exception&)
							{
								LogWarning("Failed to cast any to string, you stupid");
							}

						}
						texture.Initialize(TextureInitializationType::DDS, payLoadString, false);
						myMeshes[i]->ChangeMaterial(texture, 3);
					}
					ImGui::EndDragDropTarget();
				}

				for(size_t j = 4; j < myMeshes[i]->GetTextureCount(); j++)
				{
					//MAY cause crash, has not tried, I mean you stupid
					ImGui::Text(std::to_string(i).c_str());
					ImGui::Image(myMeshes[i]->GetTexture((int)j).GetShaderResourceView(), ImVec2(150, 150));
				}

				ImGui::TreePop();
			}
		}
		ImGui::EndTabItem();
	}
}

//void MeshComponent::Update(const float aDeltaTime)

void MeshComponent::Update(const float aDeltaTime, Scene&)
{
	UNREFERENCED_PARAMETER(aDeltaTime);
}

void MeshComponent::Render(ModelShader& aModelShader)
{
	if (!IsActive())
	{
		return;
	}
	for (Mesh* mesh : myMeshes)
	{
		LogWarning("MeshComponent::Render Hey should the be used?! should it not be modelDrawer?!");
		mesh->Render(aModelShader, *myParent->GetTransform(), IshighLighted);
		//Transform* transform = new Transform(myParent->MultiplyWithParent());
		//mesh->Render(aModelShader, *transform, IshighLighted);
	}
}

std::vector<Mesh*>& MeshComponent::GetMesh() { return myMeshes; }

void MeshComponent::SetPath(std::string aPath) { path = aPath; }
std::string MeshComponent::GetPath() { return path; }

void MeshComponent::SetIsHighLighted(const bool aBool)
{
	IshighLighted = aBool;
}
