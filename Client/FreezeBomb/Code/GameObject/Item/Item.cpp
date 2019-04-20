#include "../../Stdafx/Stdafx.h"
#include "Item.h"
#include "../Shadow/Shadow.h"

CItem::CItem()
{
}

CItem::~CItem()
{
	if (m_pShadow)
		delete m_pShadow;
}

void CItem::Initialize_Shadow(CLoadedModelInfo* pLoadedModel, CGameObject* pGameObject)
{
	m_pShadow = new CShadow(pLoadedModel, this);
}

void CItem::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera, int nPipelineState)
{
	if (IsVisible(pCamera) == true)
	{
		CGameObject::Render(pd3dCommandList, pCamera, GameObject);

		if (m_pShadow)
			m_pShadow->Render(pd3dCommandList, pCamera, GameObject_Shadow);
	}
}