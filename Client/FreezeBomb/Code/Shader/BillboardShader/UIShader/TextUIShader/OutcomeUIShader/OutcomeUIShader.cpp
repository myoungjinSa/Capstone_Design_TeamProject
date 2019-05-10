#include "../../../../../Stdafx/Stdafx.h"
#include "OutcomeUIShader.h"
#include "../../../../../Mesh/BillboardMesh/BillboardMesh.h"
#include "../../../../../Texture/Texture.h"
#include "../../../../../Material/Material.h"
#include "../../../../../Scene/Scene.h"
#include "../../../../../GameObject/Billboard/UI/UI.h"

COutcomeUIShader::COutcomeUIShader()
{

}

COutcomeUIShader::~COutcomeUIShader()
{

}

void COutcomeUIShader::CreateShader(ID3D12Device *pd3dDevice,ID3D12GraphicsCommandList* pd3dCommandList,ID3D12RootSignature *pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];

	for (int i = 0; i < m_nPipelineStates; ++i)
	{
		::ZeroMemory(&m_d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
		m_d3dPipelineStateDesc.pRootSignature = pd3dGraphicsRootSignature;
		m_d3dPipelineStateDesc.VS = CreateVertexShader();
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

D3D12_SHADER_BYTECODE COutcomeUIShader::CreateVertexShader()
{
	return(CShader::CompileShaderFromFile(L"../Code/Shader/HLSL/UI.hlsl", "VS_OutCome", "vs_5_1", &m_pd3dVertexShaderBlob));
}



void COutcomeUIShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature,
		const map<string, CTexture*>& Context, void* pContext)
{
	CBillboardMesh* pOutcomeMesh = new CBillboardMesh(pd3dDevice, pd3dCommandList, 20.f, 20.f, 0.0f, 0.0f, 0.0f, 0.0f);

	const int outcome = 2;
	for (int i = 0; i < outcome; ++i)
	{
		CMaterial* pMaterial = new CMaterial(1);

		switch(i)
		{
		case WIN:
		{
			auto iter = Context.find("Win");
			if (iter != Context.end())
				pMaterial->SetTexture((*iter).second, 0);
			break;
		}
		case LOSE:
		{
			auto iter = Context.find("Lose");
			if (iter != Context.end())
				pMaterial->SetTexture((*iter).second, 0);
			break;
		}
		}
		
		CUI* pUI = new CUI(1);
		pUI->SetMesh(pOutcomeMesh);
		pUI->SetMaterial(0, pMaterial);
		m_UIMap.emplace(i, pUI);
	}
}

void COutcomeUIShader::AnimateObjects(float fTimeElapsed,CCamera *pCamera, CPlayer* pPlayer)
{

}

void COutcomeUIShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera, int nPipelineState)
{
	if (m_IsRender)
	{
		CUIShader::OnPrepareRender(pd3dCommandList, 0);
		auto iter = m_UIMap.find(m_outcome);
		if (iter != m_UIMap.end())
			(*iter).second->Render(pd3dCommandList, nPipelineState, 0);
	}
}
void COutcomeUIShader::ReleaseObjects()
{
	for (auto iter = m_UIMap.begin(); iter != m_UIMap.end();)
	{
		(*iter).second->Release();
		iter = m_UIMap.erase(iter);
	}
	m_UIMap.clear();
}