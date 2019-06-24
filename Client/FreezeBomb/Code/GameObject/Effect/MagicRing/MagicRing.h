#pragma once
#include "../../GameObject.h"

class CMaterial;
class CMagicRing : public CGameObject
{
public:
	CMagicRing();
	virtual ~CMagicRing();

	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState);

	void SetActive() { m_bActive = true; }
	void SetDeActive() { m_bActive = false; }

	bool GetIsActive() { return m_bActive; }

protected:
	bool m_bActive;
	
};