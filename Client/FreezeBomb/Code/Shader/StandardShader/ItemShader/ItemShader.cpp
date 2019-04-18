#include "../../../Stdafx/Stdafx.h"
#include "ItemShader.h"
#include "../../../GameObject/Item/Item.h"
#include "../../../GameObject/Shadow/Shadow.h"
#include "../../../GameObject/Terrain/Terrain.h"
#include "../../../ResourceManager/ResourceManager.h"
#include "../../../FrameTransform/FrameTransform.h"

CItemShader::CItemShader()
{
}

CItemShader::~CItemShader()
{
}

void CItemShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature,
	const map<string, CLoadedModelInfo*>& ModelMap, const map<string, Bounds*>& Context, void* pContext)
{
	int nNormalHammer = 10, nGoldHammer = 1, nGoldTimer = 5;
	m_nObjects = nNormalHammer + nGoldHammer + nGoldTimer;

	auto iter = ModelMap.find("Hammer");
	if (iter != ModelMap.end())
	{
		CTerrain* pTerrain = (CTerrain*)pContext;

		for (int i = 0; i < nNormalHammer; ++i)
		{
			CItem* pItem = new CItem;
			pItem->SetChild((*iter).second->m_pModelRootObject, true);
			// 그림자를 모델좌표계에서 계산하도록 변경하면 될듯?
			XMFLOAT3 Position = XMFLOAT3(Random(10.f, 490.f), 0.5f, Random(10.f, 290.f));
			pItem->SetPosition(Position);
			pItem->setItemType(CItem::NormalHammer);
			pItem->setID("<Hammer>");
			auto iter2 = Context.find(pItem->getID());
			if (iter2 != Context.end())
				pItem->SetOOBB((*iter2).second->m_xmf3Center, (*iter2).second->m_xmf3Extent, XMFLOAT4(0, 0, 0, 1));

			pItem->m_pFrameTransform = new CFrameTransform(pd3dDevice, pd3dCommandList, (*iter).second);
			pItem->Initialize_Shadow((*iter).second, pItem);

			m_ItemMap.emplace("NormalHammer" + to_string(i), dynamic_cast<CItem*>(pItem));
		}
	
		for (int i = 0; i < nGoldHammer; ++i)
		{
			CItem* pItem = new CItem;
			pItem->SetChild((*iter).second->m_pModelRootObject, true);
			XMFLOAT3 Position = XMFLOAT3(Random(10.f, 490.f), 0.5, Random(10.f, 290.f));
			pItem->SetPosition(Position);
			pItem->setItemType(CItem::GoldHammer);
			pItem->setID("<Hammer>");
			auto iter2 = Context.find(pItem->getID());
			if (iter2 != Context.end())
				pItem->SetOOBB((*iter2).second->m_xmf3Center, (*iter2).second->m_xmf3Extent, XMFLOAT4(0, 0, 0, 1));

			pItem->m_pFrameTransform = new CFrameTransform(pd3dDevice, pd3dCommandList, (*iter).second);
			// 그림자 생성
			pItem->Initialize_Shadow((*iter).second, pItem);
			m_ItemMap.emplace("GoldHammer" + to_string(i), dynamic_cast<CItem*>(pItem));
		}
	}

	iter = ModelMap.find("GoldTimer");
	if (iter != ModelMap.end())
	{
		for (int i = 0; i < nGoldTimer; ++i)
		{
			CItem* pItem = new CItem;
			pItem->SetChild((*iter).second->m_pModelRootObject, true);
			XMFLOAT3 Position = XMFLOAT3(Random(10.f, 490.f), 0.3f, Random(10.f, 290.f));
			pItem->SetPosition(Position);
			pItem->setItemType(CItem::GoldTimer);
			pItem->setID("<Pocket_Watch>");
			auto iter2 = Context.find(pItem->getID());
			if (iter2 != Context.end())
				pItem->SetOOBB((*iter2).second->m_xmf3Center, (*iter2).second->m_xmf3Extent, XMFLOAT4(0, 0, 0, 1));

			pItem->m_pFrameTransform = new CFrameTransform(pd3dDevice, pd3dCommandList, (*iter).second);
			pItem->Initialize_Shadow((*iter).second, pItem);

			m_ItemMap.emplace("GoldTimer" + to_string(i), dynamic_cast<CItem*>(pItem));
		}
	}
}

void CItemShader::AnimateObjects(float fTimeElapsed, CCamera* pCamera, CPlayer* pPlayer)
{
	m_fElapsedTime += fTimeElapsed;
}

void CItemShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera, int nPipelineState)
{
	for (auto iter = m_ItemMap.begin(); iter != m_ItemMap.end(); ++iter)
	{
		(*iter).second->Animate(m_fElapsedTime);
		(*iter).second->UpdateTransform(nullptr);
		(*iter).second->Render(pd3dCommandList, pCamera, nPipelineState);
	}
}

void CItemShader::ReleaseObjects()
{
	for (auto iter = m_ItemMap.begin(); iter != m_ItemMap.end(); )
	{
		(*iter).second->Release();
		iter = m_ItemMap.erase(iter);
	}
	m_ItemMap.clear();

	for (auto iter = m_RemovedItemList.begin(); iter != m_RemovedItemList.end(); )
	{
		(*iter)->Release();
		iter = m_RemovedItemList.erase(iter);
	}
	m_RemovedItemList.clear();
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
		cout << (*iter).first << " 획득" << endl;
		// 제거된 아이템 리스트에 넣어줌
		m_RemovedItemList.emplace_back((*iter).second);
		iter = m_ItemMap.erase(iter);
	}
}