#include "../../../Stdafx/Stdafx.h"
#include "ItemShader.h"
#include "../../../GameObject/Item/Item.h"
#include "../../../GameObject/Shadow/Shadow.h"
#include "../../../GameObject/Terrain/Terrain.h"
#include "../../../ResourceManager/ResourceManager.h"

CItemShader::CItemShader()
{
}

CItemShader::~CItemShader()
{
}

void CItemShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature,
	const map<string, Bounds*>& Context, void* pContext)
{

	CLoadedModelInfo* pHammer = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, 
		"../Resource/Models/Hammer.bin", nullptr, false, "Hammer");

	CTerrain* pTerrain = (CTerrain*)pContext;

	m_nObjects = 10;

	for (int i = 0; i < m_nObjects; ++i)
	{
		CItem* pItem = new CItem;
		XMFLOAT3 Position = XMFLOAT3(Random(10.f, 490.f), 0, Random(10.f, 290.f));
		pItem->SetPosition(Position);
		// ¸ÁÄ¡°¡ ´©¿öÀÖ°ÔÇÏ±â À§ÇØ È¸Àü½ÃÅ´
		XMFLOAT3 Axis = XMFLOAT3(1.f, 0.f, 0.f);
		pItem->Rotate(&Axis, 90);
		pItem->SetChild(pHammer->m_pModelRootObject, true);
		pItem->setID("<Hammer>");
		auto iter = Context.find(pItem->getID());
		if (iter != Context.end())
			pItem->SetOOBB((*iter).second->m_xmf3Center, (*iter).second->m_xmf3Extent, XMFLOAT4(0, 0, 0, 1));
		pItem->Initialize(pHammer, pItem);
		pItem->getShadow()->Rotate(&Axis, 90);

		m_ItemMap.emplace("¸ÁÄ¡" + to_string(i), dynamic_cast<CItem*>(pItem));
	}

	if (pHammer)
		delete pHammer;
}

void CItemShader::AnimateObjects(float fTimeElapsed, CCamera* pCamera, CPlayer* pPlayer)
{
	m_fElapsedTime += fTimeElapsed;

	for (auto iter = m_ItemMap.begin(); iter != m_ItemMap.end(); ++iter)
		(*iter).second->Animate(m_fElapsedTime);
}

void CItemShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera, int nPipelineState)
{
	CStandardShader::Render(pd3dCommandList, pCamera, nPipelineState);
	//CStandardShader::Render(pd3dCommandList, pCamera, GameObject_Shadow);
	//CStandardShader::Render(pd3dCommandList, pCamera, nPipelineState);

	for (auto iter = m_ItemMap.begin(); iter != m_ItemMap.end(); ++iter)
	{
		(*iter).second->UpdateTransform(nullptr);
		(*iter).second->Render(pd3dCommandList, pCamera, nPipelineState);
	}
}

void CItemShader::ReleaseObjects()
{
	for (auto iter = m_ItemMap.begin(); iter != m_ItemMap.end(); )
	{
		delete (*iter).second;
		iter = m_ItemMap.erase(iter);
	}
	m_ItemMap.clear();
}

void CItemShader::ReleaseUploadBuffers()
{
	for (auto iter = m_ItemMap.begin(); iter != m_ItemMap.end(); ++iter)
		(*iter).second->ReleaseUploadBuffers();
}

void CItemShader::ItemDelete(string key)
{
	auto iter = m_ItemMap.find(key);
	if (iter != m_ItemMap.end())
	{
		cout << (*iter).first << " È¹µæ" << endl;
		delete (*iter).second;
		iter = m_ItemMap.erase(iter);
	}
}