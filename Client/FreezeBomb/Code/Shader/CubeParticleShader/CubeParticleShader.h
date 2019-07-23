#pragma once
#include "../Shader.h"

class CTexture;
class CCubeParticle;
class CCubeParticleShader : public CShader
{
public:
	CCubeParticleShader();
	virtual ~CCubeParticleShader();

	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE  CreateVertexShader(int nPipelineState);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(int nPipelineState);

	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature,const map<string,CTexture*>& Context, void *pContext = NULL);
	virtual void AnimateObjects(float fTimeElapsed, CCamera *pCamera, CPlayer* pPlayer = NULL);
	virtual void ReleaseObjects();
	virtual void OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList, int nPipelineState);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera* pCamera, int nPipelineState);

	void SetParticleBlowUp(XMFLOAT3& position,int index);

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList,int index);
	virtual void ReleaseShaderVariables();

	
private:
	array<list<CCubeParticle*>,6>		m_CubeParticleList;
protected:
	ID3D12PipelineState**		m_ppd3dPipelineStates{ nullptr };
	int										m_nPipelineStates = 0;

	

	struct InstancingData
	{
		XMFLOAT4X4 m_World;
	};

	ID3D12Resource*				m_pd3dInstancingData[6]{ nullptr };
	InstancingData*				m_pMappedInstancingData[6]{ nullptr };
};