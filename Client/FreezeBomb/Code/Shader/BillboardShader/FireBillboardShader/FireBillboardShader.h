#pragma once
#include "../BillboardShader.h"

class CTexture;

class CFireBillboardShader : public CBillboardShader
{
public:

	CFireBillboardShader();
	virtual ~CFireBillboardShader();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader();
	virtual D3D12_SHADER_BYTECODE CreatePixelShader();
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();
	virtual D3D12_BLEND_DESC CreateBlendState();
	virtual void AnimateObjects(float elapsedTime, CCamera* pCamera, CPlayer *pPlayer = NULL);

	virtual void ReleaseObjects();

	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);


};