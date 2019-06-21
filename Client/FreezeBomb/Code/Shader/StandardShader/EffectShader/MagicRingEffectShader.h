#pragma once

#include "../StandardShader.h"

class CLoadedModelInfo;
class CMagicRing;
class CTexture;
class CSkinnedAnimationObjectShader;
class CEvilBear;

class CMagicRingShader : public CStandardShader
{
public:
	CMagicRingShader();
	virtual ~CMagicRingShader();

	void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature,
		const map<string, CTexture*>& textureMap, void* pContext);
	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(int nPipelineState);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(int nPipelineState);
	virtual D3D12_RASTERIZER_DESC CreateRasterizerState();
	virtual D3D12_BLEND_DESC CreateBlendState(int nPipelineState);
	virtual void AnimateObjects(float fTimeElapsed, CCamera *pCamera, CPlayer* pPlayer);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState);
	virtual void ReleaseObjects();
	virtual void ReleaseUploadBuffers();

	void CheckMagicRingActive(CPlayer *pPlayer);
private:
	
	CSkinnedAnimationObjectShader* m_ptrSkinnedAnimationObjects{ nullptr };
	const int maxObjects = 7;
	array<CMagicRing*,7> m_MagicRingObjects;

};