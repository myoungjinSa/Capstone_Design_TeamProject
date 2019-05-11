#include "../../Stdafx/Stdafx.h"
#include "EvilBear.h"
#include <vector>
#include "../Shadow/Shadow.h"
#include "../../Material/Material.h"
#include "../../Shader/Shader.h"
#include "../../FrameTransform/FrameTransform.h"

CEvilBear::CEvilBear(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature,int matID)
{
	m_matID = matID;

}

CEvilBear::~CEvilBear()
{
	if (m_pShadow)
		delete m_pShadow;
}


void CEvilBear::Initialize_Shadow(CLoadedModelInfo* pLoadedModel, CGameObject* pGameObject)
{
	m_pShadow = new CShadow(pLoadedModel, this);
}

void CEvilBear::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera* pCamera, int nPipelineState)
{
	if (IsVisible(pCamera) == true)
	{
		if (m_bBomb == true)
		{
			CGameObject::Tagger_Render(pd3dCommandList, pCamera, m_matID, m_bGoldTimer, GameObject);
			if (m_pShadow)
				m_pShadow->Tagger_Render(pd3dCommandList, pCamera, m_matID, m_bGoldTimer, GameObject_Shadow);
		}
		else
		{
			CGameObject::RunAway_Render(pd3dCommandList, pCamera, m_matID, m_bHammer, m_bGoldHammer, nPipelineState);
			if (m_pShadow)
				m_pShadow->RunAway_Render(pd3dCommandList, pCamera, m_matID, m_bHammer, m_bGoldHammer, GameObject_Shadow);
		}
	}
}

void CEvilBear::Animate(float fTimeElapsed)
{
	// 루트 오브젝트를 제외한 나머지는 모두 nullptr이다. 왜냐하면, 루트 오브젝트를 제어하기 위함이므로
	if (m_pAnimationController)
		m_pAnimationController->AdvanceTime(fTimeElapsed, (float*)&m_fDistanceToTarget);

	m_xmOOBBTransformed.Transform(m_xmOOBB, XMLoadFloat4x4(&m_xmf4x4World));
	XMStoreFloat4(&m_xmOOBBTransformed.Orientation, XMQuaternionNormalize(XMLoadFloat4(&m_xmOOBBTransformed.Orientation)));

	if (m_pSibling)
		m_pSibling->Animate(fTimeElapsed);
	if (m_pChild)
		m_pChild->Animate(fTimeElapsed);
}