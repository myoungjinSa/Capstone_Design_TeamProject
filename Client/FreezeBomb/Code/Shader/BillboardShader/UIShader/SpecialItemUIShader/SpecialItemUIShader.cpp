#include "../../../../Stdafx/Stdafx.h"
#include "SpecialItemUIShader.h"
#include "../../../../Mesh/BillboardMesh/BillboardMesh.h"
#include "../../../../Texture/Texture.h"
#include "../../../../Material/Material.h"
#include "../../../../Scene/Scene.h"
#include "../../../../GameObject/Billboard/UI/UI.h"

CSpecialItemUIShader::CSpecialItemUIShader()
{
}

CSpecialItemUIShader::~CSpecialItemUIShader()
{
}

D3D12_SHADER_BYTECODE CSpecialItemUIShader::CreateVertexShader()
{
	return(CShader::CompileShaderFromFile(L"../Code/Shader/HLSL/UI.hlsl", "VSSpecialItemUI", "vs_5_1", &m_pd3dVertexShaderBlob));
}

void CSpecialItemUIShader::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature,
	const map<string, CTexture*>& Context, void* pContext)
{
	CBillboardMesh* pItemBoxMesh = new CBillboardMesh(pd3dDevice, pd3dCommandList, 20.f, 20.f, 0.0f, 0.0f, 0.0f, 0.0f);

	m_nObjects = 2;

	m_ppUIMaterial = new CMaterial*[m_nObjects];

	m_ppUIMaterial[0] = new CMaterial(1);
	auto iter = Context.find("GoldHammer_Item");
	if (iter != Context.end())
		m_ppUIMaterial[0]->SetTexture((*iter).second, 0);

	m_ppUIMaterial[1] = new CMaterial(1);
	iter = Context.find("GoldTimer_Item");
	if (iter != Context.end())
		m_ppUIMaterial[1]->SetTexture((*iter).second, 0);

	m_ppObjects = new CGameObject*[m_nObjects];
	for (int i = 0; i < m_nObjects; ++i)
	{
		CUI* pUI = new CUI(1);
		pUI->SetMesh(pItemBoxMesh);
		pUI->SetMaterial(0, m_ppUIMaterial[i]);
		m_UIMap.emplace(i, pUI);
		m_ppObjects[i] = pUI;
	}
}

void CSpecialItemUIShader::AnimateObjects(float elapsedTime, CCamera* pCamera, CPlayer* pPlayer)
{
}

void CSpecialItemUIShader::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	if (m_Render)
	{
		OnPrepareRender(pd3dCommandList);

		auto iter = m_UIMap.find(0);
		if (iter != m_UIMap.end())
			(*iter).second->Render(pd3dCommandList, pCamera);
	}
}

void CSpecialItemUIShader::ReleaseObjects()
{
	if (m_ppObjects)
		for (int i = 0; i < m_nObjects; ++i)
			if (m_ppObjects[i])
				m_ppObjects[i]->Release();

	if (m_ppUIMaterial)
		delete[] m_ppUIMaterial;
}