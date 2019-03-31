#pragma once

#include "../Billboard.h"

struct CB_ANIMATIONCLIP;

class CLampParticle : public CBillboard
{
public:
	CLampParticle(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual ~CLampParticle();

	void SetLookAt(XMFLOAT3& xmfTarget);

public:
	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera, int nPipelineState);

	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();

	virtual void UpdateShaderVariable(ID3D12GraphicsCommandList *pd3dCommandList, XMFLOAT4X4 *pxmf4x4World);

private:
	UINT														m_AnimationClip = 0;

	CB_ANIMATIONCLIP*							m_pcbMappedAnimationClip = NULL;
	ID3D12Resource*									m_pd3dcbAnimationClip = NULL;
};