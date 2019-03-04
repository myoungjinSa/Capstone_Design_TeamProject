#pragma once

#include "../GameObject.h"

class CShadow;
class CEvilBear : public CGameObject
{
public:
	CEvilBear(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature,int matID);
	virtual ~CEvilBear();

	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera* pCamera, int nPipelineState);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, bool bHammer, bool bBomb, bool bIce, int matID, CCamera *pCamera, int nPipelineState = GameObject);

	void Initialize_Shadow(CLoadedModelInfo* pLoadedModel, CGameObject* pGameObject);

private:
	CShadow*	m_pShadow{ nullptr };
};