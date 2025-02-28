#include "stdafx.h"
#include "Stencil.h"
#include "graphics\GraphicsEngine.h"

Stencil::Stencil(eMode aMode)
{
	CD3D11_DEPTH_STENCIL_DESC  dsDesc = CD3D11_DEPTH_STENCIL_DESC{ CD3D11_DEFAULT{} };
	if (aMode == eMode::Write)
	{
		dsDesc.StencilEnable = true;
		dsDesc.StencilWriteMask = 0xFF;
		dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	}
	else if (aMode == eMode::Mask)
	{
		dsDesc.DepthEnable = false;
		dsDesc.StencilEnable = true;
		dsDesc.StencilReadMask = 0xFF;
		dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_NOT_EQUAL;
		dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	}

	GraphicsEngine::GetInstance()->GetDevice()->CreateDepthStencilState(&dsDesc, &myStencilBuffer);
}

void Stencil::Bind()
{
	GraphicsEngine::GetInstance()->GetContext()->OMSetDepthStencilState(myStencilBuffer.Get(), 0xFF);
}
