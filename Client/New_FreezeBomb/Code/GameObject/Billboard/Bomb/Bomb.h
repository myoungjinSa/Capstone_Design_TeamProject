#pragma once

#include "../Billboard.h"

class CBomb : public CBillboard
{
public:
	CBomb(int nMaterial);
	virtual ~CBomb();

	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera, int nPipelineState);

	void SetLookAt(XMFLOAT3& xmfTarget);

	bool getIsBlowing()	const { return m_IsBlowing; }
	void setIsBlowing(bool value) { m_IsBlowing = value; }

public:
	UINT		m_AnimationClip = 0;
	bool		m_IsBlowing{ false };
};