#include "../../Stdafx/Stdafx.h"
#include "Shadow.h"

CShadow::CShadow(CLoadedModelInfo* pLoadedModel, CGameObject* pGameObject)
{
	SetChild(pLoadedModel->m_pModelRootObject, true);

	if(pGameObject->m_pAnimationController)
		m_pAnimationController = pGameObject->m_pAnimationController;
	if (pGameObject->m_pSkinningBoneTransforms)
		m_pSkinningBoneTransforms = pGameObject->m_pSkinningBoneTransforms;
	if (pGameObject->m_pFrameTransform)
		m_pFrameTransform = pGameObject->m_pFrameTransform;
}

CShadow::~CShadow()
{
}

void CShadow::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState)
{
	CGameObject::Render(pd3dCommandList, pCamera, nPipelineState);
}

void CShadow::Render(ID3D12GraphicsCommandList *pd3dCommandList, bool bHammer, bool bBomb, bool bIce, int matID, CCamera* pCamera, int nPipelineState)
{
	CGameObject::Render(pd3dCommandList, bHammer, bBomb, bIce, matID, pCamera, nPipelineState);
}