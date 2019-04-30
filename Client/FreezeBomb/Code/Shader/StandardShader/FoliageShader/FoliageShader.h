#pragma once

#include "../StandardShader.h"

class CLoadedModelInfo;
class CFoliageShader : public CStandardShader
{
public:
	CFoliageShader();
	virtual ~CFoliageShader();

	virtual void CreateShader(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature);
	//virtual D3D12_SHADER_BYTECODE  CreateVertexShader(int nPipelineState);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(int nPipelineState);

	virtual D3D12_BLEND_DESC CreateBlendState();
	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, 
		const map<string,CLoadedModelInfo*>& ModelMap,void *pContext);
	virtual void AnimateObjects(float fTimeElapsed, CCamera *pCamera, CPlayer *pPlayer =NULL);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera, int nPipelineState);

	//virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	//virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	//virtual void ReleaseShaderVariables();
	virtual void ReleaseObjects();
	virtual void ReleaseUploadBuffers();

	float GetDistanceToCamera(CGameObject* pObject, CCamera* pCamera);

private:
	CLoadedModelInfo*	m_ppFoliageModel01{ nullptr };
	CLoadedModelInfo*	m_ppFoliageModel02{ nullptr };
	CLoadedModelInfo*	m_ppFoliageModel03{ nullptr };

	struct InstancingData
	{
		XMFLOAT4X4 m_World;
	};

	ID3D12Resource*				m_pd3dInstancingData{ nullptr };
	InstancingData*				m_pMappedInstancingData{ nullptr };
};