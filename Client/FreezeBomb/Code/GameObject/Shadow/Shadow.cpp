#include "../../Stdafx/Stdafx.h"
#include "Shadow.h"

CShadow::CShadow(CLoadedModelInfo* pLoadedModel, CGameObject* pGameObject)
{
	SetChild(pLoadedModel->m_pModelRootObject, true);

	m_pTruth = pGameObject;

	if(m_pTruth->m_pAnimationController)
		m_pAnimationController = m_pTruth->m_pAnimationController;
	if (m_pTruth->m_pSkinningBoneTransforms)
		m_pSkinningBoneTransforms = m_pTruth->m_pSkinningBoneTransforms;
	if (m_pTruth->m_pFrameTransform)
		m_pFrameTransform = m_pTruth->m_pFrameTransform;
}

CShadow::~CShadow()
{
	if (m_pAnimationController)
	{
		m_pTruth->m_pAnimationController = nullptr;
		delete m_pAnimationController;
		m_pAnimationController = nullptr;
	}

	if (m_pSkinningBoneTransforms)
	{
		m_pTruth->m_pSkinningBoneTransforms = nullptr;
		delete m_pSkinningBoneTransforms;
		m_pSkinningBoneTransforms = nullptr;
	}

	if (m_pFrameTransform)
	{
		m_pTruth->m_pFrameTransform = nullptr;
		delete m_pFrameTransform;
		m_pFrameTransform = nullptr;
	}
}

void CShadow::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState)
{
	CGameObject::Render(pd3dCommandList, pCamera, nPipelineState);
}

void CShadow::Render(ID3D12GraphicsCommandList *pd3dCommandList, bool bHammer, bool bBomb, bool bIce, int matID, CCamera* pCamera, int nPipelineState)
{
	CGameObject::Render(pd3dCommandList, bHammer, bBomb, bIce, matID, pCamera, nPipelineState);
}