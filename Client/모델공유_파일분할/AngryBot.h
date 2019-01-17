#pragma once

#include "GameObject.h"

class CAngryBot : public CGameObject
{
public:
	CAngryBot(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature);
	virtual ~CAngryBot();

	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void OnPrepareAnimate();
	virtual void Animate(float fTimeElapsed);

private:
	ID3D12Resource*		m_pd3dcbBoneTransforms = NULL;
	XMFLOAT4X4*			m_pcbxmf4x4BoneTransforms = NULL;
};
