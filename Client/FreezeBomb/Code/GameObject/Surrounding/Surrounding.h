#pragma once

#include "../GameObject.h"

// 주변 정적인 오브젝트
class CSurrounding : public CGameObject
{
public:
	CSurrounding(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	~CSurrounding();

	void Initialize_Shadow(CLoadedModelInfo* pLoadedModel, CGameObject* pGameObject);

	void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera, int nPipelineState);
};