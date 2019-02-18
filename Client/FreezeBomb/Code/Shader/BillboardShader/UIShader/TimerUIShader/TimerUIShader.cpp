#include "../../../../Stdafx/Stdafx.h"
#include "TimerUIShader.h"
#include "../../../../Mesh/BillboardMesh/BillboardMesh.h"
#include "../../../../Texture/Texture.h"
#include "../../../../Material/Material.h"
#include "../../../../Scene/Scene.h"
#include "../../../../GameObject/Billboard/UI/UI.h"

CTimerUIShader::CTimerUIShader()
{
}

CTimerUIShader::~CTimerUIShader()
{
}

void CTimerUIShader::CreateShader(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 4;
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

D3D12_SHADER_BYTECODE CTimerUIShader::CreateVertexShader(int UIType)
{
	switch (UIType)
	{
	case OneSec:
		return(CShader::CompileShaderFromFile(L"../Code/Shader/HLSL/UI.hlsl", "VSOneSecUI", "vs_5_1", &m_pd3dVertexShaderBlob));
		break;

	case TenSec:
		return(CShader::CompileShaderFromFile(L"../Code/Shader/HLSL/UI.hlsl", "VSTenSecUI", "vs_5_1", &m_pd3dVertexShaderBlob));
		break;

	case OneMin:
		return(CShader::CompileShaderFromFile(L"../Code/Shader/HLSL/UI.hlsl", "VSOneMinUI", "vs_5_1", &m_pd3dVertexShaderBlob));
		break;

	case Colon:
		return(CShader::CompileShaderFromFile(L"../Code/Shader/HLSL/UI.hlsl", "VSColonUI", "vs_5_1", &m_pd3dVertexShaderBlob));
		break;
	}
}

void CTimerUIShader::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature,
	const map<string, CTexture*>& Context, void* pContext)
{
	CBillboardMesh* pTimerMesh = new CBillboardMesh(pd3dDevice, pd3dCommandList, 20.f, 20.f, 0.0f, 0.0f, 0.0f, 0.0f);

	m_nObjects = 31;

	// 일초
	m_ppUIMaterial = new CMaterial*[m_nObjects];
	for (int i = 0; i < 10; ++i)
	{
		m_ppUIMaterial[i] = new CMaterial(1);

		auto iter = Context.find(to_string(i));
		if (iter != Context.end())
			m_ppUIMaterial[i]->SetTexture((*iter).second, 0);

		CUI* pUI = new CUI(1);
		pUI->SetMesh(pTimerMesh);
		pUI->SetMaterial(0, m_ppUIMaterial[i]);
		m_OneSecUIMap.emplace(i, pUI);
	}
	// 십초
	for (int i =10, j = 0; i < 20; ++i, ++j)
	{
		m_ppUIMaterial[i] = new CMaterial(1);

		auto iter = Context.find(to_string(j));
		if (iter != Context.end())
			m_ppUIMaterial[i]->SetTexture((*iter).second, 0);

		CUI* pUI = new CUI(1);
		pUI->SetMesh(pTimerMesh);
		pUI->SetMaterial(0, m_ppUIMaterial[i]);
		m_TenSecUIMap.emplace(j, pUI);
	}
	// 일분
	for (int i = 20, j = 0; i < 30; ++i, ++j)
	{
		m_ppUIMaterial[i] = new CMaterial(1);

		auto iter = Context.find(to_string(j));
		if (iter != Context.end())
			m_ppUIMaterial[i]->SetTexture((*iter).second, 0);

		CUI* pUI = new CUI(1);
		pUI->SetMesh(pTimerMesh);
		pUI->SetMaterial(0, m_ppUIMaterial[i]);
		m_OneMinUIMap.emplace(j, pUI);
	}

	m_ppUIMaterial[m_nObjects - 1] = new CMaterial(1);

	auto iter = Context.find("Colon");
	if (iter != Context.end())
		m_ppUIMaterial[m_nObjects - 1]->SetTexture((*iter).second, 0);

	CUI* pUI = new CUI(1);
	pUI->SetMesh(pTimerMesh);
	pUI->SetMaterial(0, m_ppUIMaterial[m_nObjects - 1]);
	m_UIMap.emplace(Colon, pUI);

}

void CTimerUIShader::AnimateObjects(float elapsedTime, CCamera* pCamera, CPlayer* pPlayer)
{
	m_Time += elapsedTime;

	m_Sec = (int)m_Time % 10;
	m_TenSec = (int)m_Time % 60 / 10;
	m_Min = (int)m_Time / 60 % 10;
	m_TenMin = (int)m_Time / 60 % 60 / 10;
}

void CTimerUIShader::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	CUIShader::OnPrepareRender(pd3dCommandList, OneSec);
	auto iter = m_OneSecUIMap.find(m_Sec);
	if (iter != m_OneSecUIMap.end())
		(*iter).second->Render(pd3dCommandList, pCamera);

	CUIShader::OnPrepareRender(pd3dCommandList, TenSec);
	iter = m_TenSecUIMap.find(m_TenSec);
	if (iter != m_TenSecUIMap.end())
		(*iter).second->Render(pd3dCommandList, pCamera);

	CUIShader::OnPrepareRender(pd3dCommandList, OneMin);
	iter = m_OneMinUIMap.find(m_Min);
	if (iter != m_OneMinUIMap.end())
		(*iter).second->Render(pd3dCommandList, pCamera);

	CUIShader::OnPrepareRender(pd3dCommandList, Colon);
	iter = m_UIMap.find(Colon);
	if (iter != m_UIMap.end())
		(*iter).second->Render(pd3dCommandList, pCamera);
}

void CTimerUIShader::ReleaseObjects()
{
	for (auto iter = m_UIMap.begin(); iter != m_UIMap.end(); )
	{
		delete (*iter).second;
		iter = m_UIMap.erase(iter);
	}
	m_UIMap.clear();

	for (auto iter = m_OneSecUIMap.begin(); iter != m_OneSecUIMap.end();)
	{
		delete (*iter).second;
		iter = m_OneSecUIMap.erase(iter);
	}
	m_OneSecUIMap.clear();

	for (auto iter = m_TenSecUIMap.begin(); iter != m_TenSecUIMap.end();)
	{
		delete (*iter).second;
		iter = m_TenSecUIMap.erase(iter);
	}
	m_TenSecUIMap.clear();

	for (auto iter = m_OneMinUIMap.begin(); iter != m_OneMinUIMap.end();)
	{
		delete (*iter).second;
		iter = m_OneMinUIMap.erase(iter);
	}
	m_OneMinUIMap.clear();

	if (m_ppUIMaterial)
		delete[] m_ppUIMaterial;
}