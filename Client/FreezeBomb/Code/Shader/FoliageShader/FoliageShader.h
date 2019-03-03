#pragma once
#include "../StandardShader/StandardObjectsShader/StandardObjectsShader.h"

class CLoadedModelInfo;
class CFoliageShader : public CStandardObjectsShader
{
public:
	CFoliageShader();
	virtual ~CFoliageShader();

	virtual D3D12_BLEND_DESC CreateBlendState();
	virtual D3D12_SHADER_BYTECODE CreatePixelShader();

	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, 
		void *pContext);
	virtual void AnimateObjects(float fTimeElapsed, CCamera *pCamera, CPlayer *pPlayer =NULL);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera, int nPipelineState);

	float GetDistanceToCamera(CGameObject* pObject, CCamera* pCamera);

private:
	CLoadedModelInfo*	m_ppFoliageModel01{ nullptr };
	CLoadedModelInfo*	m_ppFoliageModel02{ nullptr };
	CLoadedModelInfo*	m_ppFoliageModel03{ nullptr };
};