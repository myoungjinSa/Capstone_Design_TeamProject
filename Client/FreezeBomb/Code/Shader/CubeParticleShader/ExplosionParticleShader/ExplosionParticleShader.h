#pragma once

#include "../CubeParticleShader.h"

class CTexture;
class CExplosionParticle;
class CExplosionParticleShader : public CCubeParticleShader
{
public:
	CExplosionParticleShader();
	virtual ~CExplosionParticleShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(int nPipelineState);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(int nPipelineState);
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();

	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, void *pContext = NULL);
	virtual void AnimateObjects(float fTimeElapsed, CCamera *pCamera, CPlayer* pPlayer = NULL);
	virtual void ReleaseObjects();
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera* pCamera, int nPipelineState);
	void SetParticleBlowUp(XMFLOAT3& position);
	void ResetParticles();
	//void SetBlowingUp(bool blow) { m_isBlowing = blow; }

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();
private:
	list<CExplosionParticle*> m_ExplosionParticleList;
	//bool m_isBlowing{ false };
//	float m_elapsedTime{ 0.0f };
};
