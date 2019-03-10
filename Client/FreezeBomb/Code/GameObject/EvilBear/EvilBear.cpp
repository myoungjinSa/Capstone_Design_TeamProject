#include "../../Stdafx/Stdafx.h"
#include "EvilBear.h"
#include "../Shadow/Shadow.h"
#include "../../Material/Material.h"
#include "../../Shader/Shader.h"

CEvilBear::CEvilBear(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature,int matID)
{
	m_matID = matID;
}

CEvilBear::~CEvilBear()
{
}

void CEvilBear::Initialize_Shadow(CLoadedModelInfo* pLoadedModel, CGameObject* pGameObject)
{
	m_pShadow = new CShadow(pLoadedModel, this);
}

void CEvilBear::Render(ID3D12GraphicsCommandList *pd3dCommandList, bool bHammer, bool bBomb, bool bIce, int matID, CCamera* pCamera, int nPipelineState)
{
	CGameObject::Render(pd3dCommandList, bHammer, bBomb, bIce, matID, pCamera, GameObject);

	if (m_pShadow)
		m_pShadow->Render(pd3dCommandList, bHammer, bBomb, bIce, matID, pCamera, GameObject_Shadow);
}

