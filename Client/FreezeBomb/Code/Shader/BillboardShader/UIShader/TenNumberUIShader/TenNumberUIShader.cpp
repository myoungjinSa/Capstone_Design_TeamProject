#include "../../../../Stdafx/Stdafx.h"
#include"TenNumberUIShader.h"
#include "../../../../Mesh/BillboardMesh/BillboardMesh.h"
#include "../../../../Texture/Texture.h"
#include "../../../../Material/Material.h"
#include "../../../../Scene/Scene.h"
#include "../../../../GameObject/Billboard/UI/UI.h"

CTenNumberUIShader::CTenNumberUIShader()
{
}

CTenNumberUIShader::~CTenNumberUIShader()
{
}

D3D12_SHADER_BYTECODE CTenNumberUIShader::CreateVertexShader()
{
	return(CShader::CompileShaderFromFile(L"../Code/Shader/HLSL/UI.hlsl", "VSTenNumberUI", "vs_5_1", &m_pd3dVertexShaderBlob));
}

void CTenNumberUIShader::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, 
	const map<string, CTexture*>& Context, void* pContext)
{
	CBillboardMesh* pNumberMesh = new CBillboardMesh(pd3dDevice, pd3dCommandList, 20.f, 20.f, 0.0f, 0.0f, 0.0f, 0.0f);

	m_nObjects = 10;
	// 일의자리
	m_ppUIMaterial = new CMaterial*[m_nObjects];
	for (int i = 0; i < m_nObjects; ++i)
	{
		m_ppUIMaterial[i] = new CMaterial(1);

		auto iter = Context.find(to_string(i));
		if (iter != Context.end())
			m_ppUIMaterial[i]->SetTexture((*iter).second, 0);
	}

	m_ppObjects = new CGameObject*[m_nObjects];
	for (int i = 0; i < m_nObjects; ++i)
	{
		CUI* pUI = new CUI(1);
		pUI->SetMesh(pNumberMesh);
		pUI->SetMaterial(0, m_ppUIMaterial[i]);
		m_UIMap.emplace(i, pUI);
		m_ppObjects[i] = pUI;
	}
}

void CTenNumberUIShader::AnimateObjects(float elapsedTime, CCamera* pCamera, CPlayer* pPlayer)
{
}

void CTenNumberUIShader::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	if (m_Time >= 6)
		m_Time = 0;

	OnPrepareRender(pd3dCommandList);

	auto iter = m_UIMap.find((int)m_Time);
	if (iter != m_UIMap.end())
		(*iter).second->Render(pd3dCommandList, pCamera);
}

void CTenNumberUIShader::ReleaseObjects()
{
	if (m_ppObjects)
		for (int i = 0; i < m_nObjects; ++i)
			if (m_ppObjects[i])
				m_ppObjects[i]->Release();

	if (m_ppUIMaterial)
		delete[] m_ppUIMaterial;
}