#pragma once
#include "../BillboardShader.h"

class CTexture;
class CLampParticleShader : public CBillboardShader
{
public:
	CLampParticleShader();
	virtual ~CLampParticleShader();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader();
	virtual D3D12_SHADER_BYTECODE CreatePixelShader();

	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, 
		const map<string, CTexture*>& Context, void* pContext);
	virtual void AnimateObjects(float elapsedTime, CCamera *pCamera, CPlayer* pPlayer = NULL);
	virtual void ReleaseObjects();

	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);
};