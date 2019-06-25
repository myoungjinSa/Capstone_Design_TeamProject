#include "../../../Stdafx/Stdafx.h"
#include "ItemShader.h"
#include "../../../GameObject/Item/Item.h"
#include "../../../GameObject/Terrain/Terrain.h"
#include "../../../ResourceManager/ResourceManager.h"
#include "../../../FrameTransform/FrameTransform.h"

CItemShader::CItemShader()
{
}

CItemShader::~CItemShader()
{
}

void CItemShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature,
	const map<string, CLoadedModelInfo*>& ModelMap, const multimap<string, MapObjectInfo*>& MapObjectInfo, const map<string, Bounds*>& BoundMap, void* pContext)
{
	int nNormalHammer = 0, nGoldHammer = 0, nGoldTimer = 0;

	auto iter = ModelMap.find("Hammer");
	if (iter != ModelMap.end())
	{
		string name = (*iter).first;
		// multimap 컨테이너에서 같은 키를 갖는 벨류를 찾을 때, 사용하는 루프
		for (auto iter2 = MapObjectInfo.lower_bound(name); iter2 != MapObjectInfo.upper_bound(name); ++iter2)
		{
			CItem* pItem = new CItem;
			pItem->m_pFrameTransform = new CFrameTransform(pd3dDevice, pd3dCommandList, (*iter).second);

			pItem->SetChild((*iter).second->m_pModelRootObject, true);
			XMFLOAT3 position = (*iter2).second->m_Position;
			pItem->SetPosition(position.x, position.y + 0.5, position.z);

			pItem->SetLookVector((*iter2).second->m_Look);
			pItem->SetUpVector((*iter2).second->m_Up);
			pItem->SetRightVector((*iter2).second->m_Right);
			pItem->Initialize_Shadow((*iter).second, pItem);

			// 황금망치, 일반망치 랜덤으로 결정
			switch (rand() % 2)
			{
			case CItem::NormalHammer:
				pItem->setItemType(CItem::NormalHammer);
				m_ItemMap.emplace("NormalHammer" + to_string(nNormalHammer++), pItem);
				break;

			case CItem::GoldHammer:
				pItem->setItemType(CItem::GoldHammer);
				m_ItemMap.emplace("GoldHammer" + to_string(nGoldHammer++), pItem);
					break;
			}
		}	
	}

	iter = ModelMap.find("GoldTimer");
	if (iter != ModelMap.end())
	{
		string name = (*iter).first;
		// multimap 컨테이너에서 같은 키를 갖는 벨류를 찾을 때, 사용하는 루프
		for (auto iter2 = MapObjectInfo.lower_bound(name); iter2 != MapObjectInfo.upper_bound(name); ++iter2)
		{
			CItem* pItem = new CItem;
			pItem->m_pFrameTransform = new CFrameTransform(pd3dDevice, pd3dCommandList, (*iter).second);

			pItem->SetChild((*iter).second->m_pModelRootObject, true);
			XMFLOAT3 position = (*iter2).second->m_Position;
			pItem->SetPosition(position.x, position.y + 0.5, position.z);

			pItem->SetLookVector((*iter2).second->m_Look);
			pItem->SetUpVector((*iter2).second->m_Up);
			pItem->SetRightVector((*iter2).second->m_Right);
			pItem->Initialize_Shadow((*iter).second, pItem);

			pItem->setItemType(CItem::GoldTimer);
			m_ItemMap.emplace("GoldTimer" + to_string(nGoldTimer++), pItem);
		}
	}


	//int nNormalHammer = 10, nGoldHammer = 10, nGoldTimer = 10;
	//m_nObjects = nNormalHammer + nGoldHammer + nGoldTimer;

	//auto iter = ModelMap.find("Hammer");
	//if (iter != ModelMap.end())
	//{
	//	CTerrain* pTerrain = (CTerrain*)pContext;

	//	for (int i = 0; i < nNormalHammer; ++i)
	//	{
	//		CItem* pItem = new CItem;
	//		pItem->SetChild((*iter).second->m_pModelRootObject, true);
	//		// 그림자를 모델좌표계에서 계산하도록 변경하면 될듯?
	//		XMFLOAT3 Position = XMFLOAT3(Random(10.f, 490.f), 0.5f, Random(10.f, 290.f));
	//		pItem->SetPosition(Position);
	//		pItem->setItemType(CItem::NormalHammer);
	//		pItem->setID("<Hammer>");
	//		auto iter2 = Context.find(pItem->getID());
	//		if (iter2 != Context.end())
	//			pItem->SetOOBB((*iter2).second->m_xmf3Center, (*iter2).second->m_xmf3Extent, XMFLOAT4(0, 0, 0, 1));

	//		pItem->m_pFrameTransform = new CFrameTransform(pd3dDevice, pd3dCommandList, (*iter).second);
	//		pItem->Initialize_Shadow((*iter).second, pItem);

	//		m_ItemMap.emplace("NormalHammer" + to_string(i), dynamic_cast<CItem*>(pItem));
	//	}
	//
	//	for (int i = 0; i < nGoldHammer; ++i)
	//	{
	//		CItem* pItem = new CItem;
	//		pItem->SetChild((*iter).second->m_pModelRootObject, true);
	//		XMFLOAT3 Position = XMFLOAT3(Random(10.f, 490.f), 0.5, Random(10.f, 290.f));
	//		pItem->SetPosition(Position);
	//		pItem->setItemType(CItem::GoldHammer);
	//		pItem->setID("<Hammer>");
	//		auto iter2 = Context.find(pItem->getID());
	//		if (iter2 != Context.end())
	//			pItem->SetOOBB((*iter2).second->m_xmf3Center, (*iter2).second->m_xmf3Extent, XMFLOAT4(0, 0, 0, 1));

	//		pItem->m_pFrameTransform = new CFrameTransform(pd3dDevice, pd3dCommandList, (*iter).second);
	//		// 그림자 생성
	//		pItem->Initialize_Shadow((*iter).second, pItem);
	//		m_ItemMap.emplace("GoldHammer" + to_string(i), dynamic_cast<CItem*>(pItem));
	//	}
	//}

	//iter = ModelMap.find("GoldTimer");
	//if (iter != ModelMap.end())
	//{
	//	for (int i = 0; i < nGoldTimer; ++i)
	//	{
	//		CItem* pItem = new CItem;
	//		pItem->SetChild((*iter).second->m_pModelRootObject, true);
	//		XMFLOAT3 Position = XMFLOAT3(Random(10.f, 490.f), 0.3f, Random(10.f, 290.f));
	//		pItem->SetPosition(Position);
	//		pItem->setItemType(CItem::GoldTimer);
	//		pItem->setID("<Pocket_Watch>");
	//		auto iter2 = Context.find(pItem->getID());
	//		if (iter2 != Context.end())
	//			pItem->SetOOBB((*iter2).second->m_xmf3Center, (*iter2).second->m_xmf3Extent, XMFLOAT4(0, 0, 0, 1));

	//		pItem->m_pFrameTransform = new CFrameTransform(pd3dDevice, pd3dCommandList, (*iter).second);
	//		pItem->Initialize_Shadow((*iter).second, pItem);

	//		m_ItemMap.emplace("GoldTimer" + to_string(i), dynamic_cast<CItem*>(pItem));
	//	}
	//}
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
		(*iter).second->UpdateTransform(nullptr,false);
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