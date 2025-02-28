#include "stdafx.h"
//
//#include "VFXElement.h"
//#include "../GraphicsEngine.h"
//#include "../model/ModelFactory.h"
//#include "../model/Mesh.h"
//#include "graphics\model\ModelDrawer.h"
//#include "graphics\sprite\SpriteDrawer.h"
//
//VFXElement::VFXElement()
//{
//	HRESULT result = S_OK;
//
//	D3D11_BUFFER_DESC bufferDesc;
//	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
//	bufferDesc.ByteWidth = sizeof(VFXBufferData);
//	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
//	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
//	bufferDesc.MiscFlags = 0;
//	bufferDesc.StructureByteStride = 0;
//
//	result = GraphicsEngine::GetInstance()->GetDevice()->CreateBuffer(&bufferDesc, NULL, myVFXBuffer.ReleaseAndGetAddressOf());
//	if (FAILED(result))
//	{
//		return;
//	}
//}
//
//VFXElement::~VFXElement()
//{
//}
//
//void VFXElement::Draw(Transform& aTransform)
//{
//	if (!myIsPlaying) return;
//	auto* graphicsEngine = GraphicsEngine::GetInstance();
//
//	switch (myType)
//	{
//	case ScrollMeshType:
//		graphicsEngine->SetBlendState(BlendState::AddAndFadeBackground);
//		graphicsEngine->SetRasterizerState(RasterizerState::NoFaceCulling);
//		graphicsEngine->SetDepthStencilState(DepthStencilState::ReadOnly);
//		graphicsEngine->UpdateGPUStates();
//		switch (myName)
//		{
//		case Bloodnova:
//		{
//			if (myTimer <= myPlayTime * 19 / 24)
//			{
//				SetBufferData(1 - myTimer / (myPlayTime * 19 / 24), myIntensity);
//				myMesh[1]->Render(graphicsEngine->GetModelDrawer().GetBloodNovaShader(), aTransform, false);
//			}
//			SetBufferData(1 - myTimer / myPlayTime, myIntensity);
//			myMesh[0]->Render(graphicsEngine->GetModelDrawer().GetBloodNovaShader(), aTransform, false);
//			break;
//		}
//		case BloodSpike:
//		{
//			SetBufferData(1 - myTimer / myPlayTime, myIntensity);
//			myAnimatedMesh[0]->Render(graphicsEngine->GetModelDrawer().GetBloodNovaShader(), aTransform, false);
//			break;
//		}
//		case BloodSlash:
//			break;
//		default:
//			break;
//		}
//		graphicsEngine->SetBlendState(BlendState::Disabled);
//		graphicsEngine->SetRasterizerState(RasterizerState::BackfaceCulling);
//		graphicsEngine->SetDepthStencilState(DepthStencilState::Write);
//		graphicsEngine->UpdateGPUStates();
//		break;
//	case FlipbookType:
//		GraphicsEngine::GetInstance()->GetSpriteDrawer().DrawSprite(mySprite);
//		break;
//	default:
//		break;
//	}
//}
//
//inline void VFXElement::SetBufferData(float time, float intensity)
//{
//	auto* graphicsEngine = GraphicsEngine::GetInstance();
//	auto context = graphicsEngine->GetContext();
//
//	VFXBufferData vfxBufferData = {};
//	vfxBufferData.vfxIntencity = intensity;
//
//	vfxBufferData.vfxTime = time;
//
//	D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
//	context->Map(myVFXBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
//	memcpy(mappedBuffer.pData, &vfxBufferData, sizeof(VFXBufferData));
//
//	context->Unmap(myVFXBuffer.Get(), 0);
//
//	context->VSSetConstantBuffers(5, 1, myVFXBuffer.GetAddressOf());
//	context->PSSetConstantBuffers(5, 1, myVFXBuffer.GetAddressOf());
//}
