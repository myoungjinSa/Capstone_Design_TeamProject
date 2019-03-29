#pragma once

#include "../BillboardShader.h"

struct CB_ANIMATIONCLIP
{
	UINT		m_AnimationClip = 0;
};

class CBomb;
class CTexture;
class CBombParticleShader : public CBillboardShader
{
public:
	CBombParticleShader();
	virtual ~CBombParticleShader();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader();
	virtual D3D12_SHADER_BYTECODE CreatePixelShader();

	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature,
		const map<string, CTexture*>& Context, void* pContext);
	virtual void AnimateObjects(float elapsedTime, CCamera *pCamera, CPlayer* pPlayer = NULL);
	virtual void ReleaseObjects();

	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera, int nPipelineState);

	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();

	virtual void UpdateShaderVariable(ID3D12GraphicsCommandList *pd3dCommandList, XMFLOAT4X4 *pxmf4x4World);

	CBomb*	getBomb()	const { return m_BombParticle; }

public:
	CBomb*	m_BombParticle{ nullptr };

	CB_ANIMATIONCLIP*							m_pcbMappedAnimationClip = NULL;
	ID3D12Resource*									m_pd3dcbAnimationClip = NULL;
};