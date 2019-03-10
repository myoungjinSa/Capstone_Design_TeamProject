#include "../../Stdafx/Stdafx.h"
#include "Item.h"
#include "../Shadow/Shadow.h"

CItem::CItem()
{
}

CItem::~CItem()
{
}

void CItem::Initialize_Shadow(CLoadedModelInfo* pLoadedModel, CGameObject* pGameObject)
{
	m_pShadow = new CShadow(pLoadedModel, this);
}

void CItem::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera, int nPipelineState)
{
	if (m_pShadow)
		m_pShadow->Render(pd3dCommandList, pCamera, GameObject_Shadow);
	CGameObject::Render(pd3dCommandList, pCamera, GameObject);
}