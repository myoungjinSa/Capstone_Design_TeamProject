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
		CGameObject::Render(pd3dCommandList, pCamera, GameObject);

		if (m_pShadow)
			m_pShadow->Render(pd3dCommandList, pCamera, GameObject_Shadow);
	}
}

//void CItem::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera* pCamera, int ItemType, int nPipelineState)
//{
//	if (m_pFrameTransform)
//		m_pFrameTransform->SetFrameMeshWorldConstantBuffer();
//
//	if (m_pMesh)
//	{
//		if (m_nMaterials > 0)
//		{
//			if (m_ppMaterials[ItemType])
//			{
//				if (m_ppMaterials[ItemType]->m_pShader)
//					m_ppMaterials[ItemType]->m_pShader->Render(pd3dCommandList, pCamera, nPipelineState);
//
//				m_ppMaterials[ItemType]->UpdateShaderVariables(pd3dCommandList);
//			}
//
//			m_pMesh->Render(pd3dCommandList, ItemType);
//		}
//	}
//
//	if (m_pSibling)
//		((CItem*)m_pSibling)->CItem::Render(pd3dCommandList, pCamera, ItemType, nPipelineState);
//	if (m_pChild)
//		((CItem*)m_pChild)->CItem::Render(pd3dCommandList, pCamera, ItemType, nPipelineState);
//}