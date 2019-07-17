#pragma once

#include "../Billboard.h"

class CMaterial;
class CUI : public CBillboard
{
public:
	CUI(int nMaterial);
	virtual ~CUI();

	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, int nPipelineState, int nTexture);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, int nPipelineState);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera, int nPipelineState);

	const XMFLOAT2& GetUIMinPos()	const { return m_UIMinPos; }
	void SetUIMinPos(XMFLOAT2 minPos) { m_UIMinPos = minPos; }
	const XMFLOAT2& GetUIMaxPos()	const { return m_UIMaxPos; }
	void SetUIMaxPos(XMFLOAT2 maxPos) { m_UIMaxPos = maxPos; }

private:
	XMFLOAT2 m_UIMinPos;
	XMFLOAT2 m_UIMaxPos;
};