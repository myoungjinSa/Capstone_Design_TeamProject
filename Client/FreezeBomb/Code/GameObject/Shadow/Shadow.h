#pragma once

#include "../GameObject.h"

class CShadow : public CGameObject
{
public:
	CShadow(CLoadedModelInfo* pLoadedModel, CGameObject* pGameObject);
	virtual ~CShadow();

	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState);
};