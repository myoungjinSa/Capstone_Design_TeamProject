#pragma once

#include "../Billboard.h"

class CMaterial;
class CUI : public CBillboard
{
public:
	CUI(int nMaterial);
	virtual ~CUI();

	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, int nPipelineState);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera, int nPipelineState);
};