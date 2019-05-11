#pragma once

#include "../GameObject.h"

class CShadow : public CGameObject
{
public:
	CShadow(CLoadedModelInfo* pLoadedModel, CGameObject* pGameObject);
	virtual ~CShadow();

	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState, int nInstance);
	
	virtual void Item_Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int ItemType, int nPipelineState);
	virtual void Tagger_Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera* pCamera, int matID, bool HasGoldTimer, int nPipelineState);
	virtual void RunAway_Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera* pCamera, int matID, bool isICE, bool HasHammer, bool HasGoldHammer, int nPipelineState);

private:
	CGameObject* m_pTruth{ nullptr };
};