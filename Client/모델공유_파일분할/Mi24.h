#pragma once

#include "GameObject.h"

class CMi24 : public CGameObject
{
public:
	CMi24(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature);
	virtual ~CMi24();

private:
	CGameObject*		m_pMainRotorFrame = NULL;
	CGameObject*		m_pTailRotorFrame = NULL;

public:
	virtual void OnPrepareAnimate();
	virtual void Animate(float fTimeElapsed);
};