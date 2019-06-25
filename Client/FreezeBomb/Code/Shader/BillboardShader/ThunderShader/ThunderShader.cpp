#include "../../../Stdafx/Stdafx.h"
#include "ThunderShader.h"
#include "../../../GameObject/GameObject.h"
#include "../../../GameObject/Billboard/Thunder/ThunderBillboard.h"
#include "../../../Mesh/BillboardMesh/BillboardMesh.h"
#include "../../../Material/Material.h"
#include "../../../Texture/Texture.h"
#include "../../../Scene/Scene.h"

CThunderBillboardShader::CThunderBillboardShader()
{

}


CThunderBillboardShader::~CThunderBillboardShader()
{

}

D3D12_SHADER_BYTECODE CThunderBillboardShader::CreateVertexShader()
{
	return(CShader::CompileShaderFromFile(L"../Code/Shader/HLSL/Shaders.hlsl", "VSThunder", "vs_5_1", &m_pd3dVertexShaderBlob));
}

D3D12_SHADER_BYTECODE CThunderBillboardShader::CreatePixelShader()
{
	return(CShader::CompileShaderFromFile(L"../Code/Shader/HLSL/Shaders.hlsl", "PSThunder", "ps_5_1", &m_pd3dPixelShaderBlob));
}


void CThunderBillboardShader::AnimateObjects(float elapsedTime,CCamera *pCamera,CPlayer *pPlayer)
{
	if(m_ppObjects)
	{
		for(int i = 0;i<m_nObjects;++i)
		{
			if(m_ppObjects[i])
			{
				XMFLOAT3 xmf3CameraPosition = pCamera->GetPosition();
				((CThunderBillboard*)m_ppObjects[i])->SetLookAt(xmf3CameraPosition);
				m_ppObjects[i]->Animate(elapsedTime);
				
			}
		}
	}
}

void CThunderBillboardShader::Render(ID3D12GraphicsCommandList* pd3dCommandList,CCamera* pCamera)
{
	
	OnPrepareRender(pd3dCommandList);

	if (m_ppObjects)
		for (int i = 0; i < m_nObjects; i++)
			if (m_ppObjects[i])
				m_ppObjects[i]->Render(pd3dCommandList, pCamera);
}

void CThunderBillboardShader::ReleaseObjects()
{
	if (m_ppObjects)
		for (int i = 0; i < m_nObjects; ++i)
			m_ppObjects[i]->Release();
}


D3D12_BLEND_DESC CThunderBillboardShader::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = TRUE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}
D3D12_DEPTH_STENCIL_DESC CThunderBillboardShader::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = TRUE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return(d3dDepthStencilDesc);
}
