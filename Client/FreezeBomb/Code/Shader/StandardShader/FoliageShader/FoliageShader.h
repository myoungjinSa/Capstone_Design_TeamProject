#pragma once

#include "../StandardShader.h"
#include "../../../ResourceManager/ResourceManager.h"

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
		const unordered_map<unsigned char, RoundInfo>& RoundMapObjectInfo, void *pContext);
	virtual void AnimateObjects(float fTimeElapsed, CCamera *pCamera, CPlayer *pPlayer =NULL);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera, int nPipelineState);

	//virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	//virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	//virtual void ReleaseShaderVariables();
	virtual void ReleaseObjects();
	virtual void ReleaseUploadBuffers();

	float GetDistanceToCamera(CGameObject* pObject, CCamera* pCamera);

private:
	CLoadedModelInfo*	m_pFoliageModel0{ nullptr };
	CLoadedModelInfo*	m_pFoliageModel1{ nullptr };
	CLoadedModelInfo*	m_pFoliageModel2{ nullptr };

	using RoundMapObjectList = list<CGameObject*>;
	unordered_map<unsigned char, RoundMapObjectList> m_MapObjectList;
};