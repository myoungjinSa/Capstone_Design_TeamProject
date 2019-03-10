#include "../../Stdafx/Stdafx.h"
#include "Surrounding.h"
#include "../Shadow/Shadow.h"

//ID3D12RootSignature* pd3dGraphicsRootSignature 필요없어 보임 - 인자로 안넘겨주는게 좋아 보임
CSurrounding::CSurrounding(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
}

CSurrounding::~CSurrounding()
{
}

void CSurrounding::Initialize_Shadow(CLoadedModelInfo* pLoadedModel, CGameObject* pGameObject)
{
	m_pShadow = new CShadow(pLoadedModel, this);
}

void CSurrounding::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera, int nPipelineState)
{
	CGameObject::Render(pd3dCommandList, pCamera, GameObject);

	if (m_pShadow)
		m_pShadow->Render(pd3dCommandList, pCamera, GameObject_Shadow);
}