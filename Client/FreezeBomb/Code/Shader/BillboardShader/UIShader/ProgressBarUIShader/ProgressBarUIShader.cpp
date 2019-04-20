#include "../../../../Stdafx/Stdafx.h"
#include "ProgressBarUIShader.h"
#include "../../../../GameObject/Billboard/UI/UI.h"
#include "../../../../Mesh/BillboardMesh/BillboardMesh.h"
#include "../../../../Texture/Texture.h"
#include "../../../../Material/Material.h"
#include "../../../../Scene/Scene.h"

CProgressBarUIShader::CProgressBarUIShader()
{
}

CProgressBarUIShader::~CProgressBarUIShader()
{
}

void CProgressBarUIShader::CreateShader(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];

	for (int i = 0; i < m_nPipelineStates; ++i)
	{
		::ZeroMemory(&m_d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
		m_d3dPipelineStateDesc.pRootSignature = pd3dGraphicsRootSignature;
		m_d3dPipelineStateDesc.VS = CreateVertexShader(i);
		m_d3dPipelineStateDesc.PS = CreatePixelShader();
		m_d3dPipelineStateDesc.RasterizerState = CreateRasterizerState();
		m_d3dPipelineStateDesc.BlendState = CreateBlendState();
		m_d3dPipelineStateDesc.DepthStencilState = CreateDepthStencilState();
		m_d3dPipelineStateDesc.InputLayout = CreateInputLayout();
		m_d3dPipelineStateDesc.SampleMask = UINT_MAX;
		m_d3dPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		m_d3dPipelineStateDesc.NumRenderTargets = 1;
		m_d3dPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		m_d3dPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		m_d3dPipelineStateDesc.SampleDesc.Count = 1;
		m_d3dPipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

		HRESULT hResult = pd3dDevice->CreateGraphicsPipelineState(&m_d3dPipelineStateDesc, __uuidof(ID3D12PipelineState), (void **)&m_ppd3dPipelineStates[i]);
	}

	if (m_pd3dVertexShaderBlob) m_pd3dVertexShaderBlob->Release();
	if (m_pd3dPixelShaderBlob) m_pd3dPixelShaderBlob->Release();

	if (m_d3dPipelineStateDesc.InputLayout.pInputElementDescs) delete[] m_d3dPipelineStateDesc.InputLayout.pInputElementDescs;
}

D3D12_SHADER_BYTECODE CProgressBarUIShader::CreateVertexShader(int UIType)
{
	switch (UIType)
	{
	case ProgressBar:
		return(CShader::CompileShaderFromFile(L"../Code/Shader/HLSL/UI.hlsl", "VSProgressBarUI", "vs_5_1", &m_pd3dVertexShaderBlob));
		break;
	}
}

void CProgressBarUIShader::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature,
	const map<string, CTexture*>& Context, void* pContext)
{
	CBillboardMesh* pItemBoxMesh = new CBillboardMesh(pd3dDevice, pd3dCommandList, 20.f, 20.f, 0.0f, 0.0f, 0.0f, 0.0f);

	auto iter = Context.find("ProgressBar");
	if (iter != Context.end())
	{
		CUI* pUI = new CUI(1);
		pUI->SetMesh(pItemBoxMesh);
		CMaterial* pUIMaterial = new CMaterial(1);
		pUIMaterial->SetTexture((*iter).second, 0);
		pUI->SetMaterial(0, pUIMaterial);
		m_UIMap.emplace(ProgressBar, pUI);

		CreateShaderVariables(pd3dDevice, pd3dCommandList);
	}
}

void CProgressBarUIShader::AnimateObjects(float elapsedTime, CCamera* pCamera, CPlayer* pPlayer)
{
	if (m_ProgressBarX > 1.f)
	{
		m_ProgressBarX = - 0.25f;
	}
	m_ProgressBarX += elapsedTime;
}

void CProgressBarUIShader::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState)
{
	auto iter = m_UIMap.find(ProgressBar);
	if (iter != m_UIMap.end())
	{
		CUIShader::OnPrepareRender(pd3dCommandList, ProgressBar);
		UpdateShaderVariables(pd3dCommandList);
		(*iter).second->Render(pd3dCommandList, ProgressBar);
	}
}

void CProgressBarUIShader::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_Position) + 255) & ~255); //256ÀÇ ¹è¼ö
	m_pd3dPositionData = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dPositionData->Map(0, nullptr, (void**)&m_pMappedPositionData);
}

void CProgressBarUIShader::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (m_pd3dPositionData)
	{
		D3D12_GPU_VIRTUAL_ADDRESS GPUVirtualAddress = m_pd3dPositionData->GetGPUVirtualAddress();
		pd3dCommandList->SetGraphicsRootConstantBufferView(23, GPUVirtualAddress);

		XMFLOAT2 position = XMFLOAT2(m_ProgressBarX, 0.125);
		m_pMappedPositionData->m_Position = position;
	}
}

void CProgressBarUIShader::ReleaseShaderVariables()
{
	if (m_pd3dPositionData)
	{
		m_pd3dPositionData->Unmap(0, nullptr);
		m_pd3dPositionData->Release();
	}
}
