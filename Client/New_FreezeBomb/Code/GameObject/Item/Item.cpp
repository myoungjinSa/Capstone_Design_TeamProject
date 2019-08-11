#include "../../Stdafx/Stdafx.h"
#include "Item.h"
#include "../Shadow/Shadow.h"

#include "../../FrameTransform/FrameTransform.h"
#include "../../Material/Material.h"
#include "../../Shader/Shader.h"

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
		CGameObject::Item_Render(pd3dCommandList, pCamera, m_ItemType, GameObject);

		if (m_pShadow)
			m_pShadow->Item_Render(pd3dCommandList, pCamera, m_ItemType, GameObject_Shadow);
	}
}
