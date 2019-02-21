#pragma once

#include "../Shader.h"


class CBillboardShader : public CShader
{
public:
	CBillboardShader();
	virtual ~CBillboardShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_BLEND_DESC CreateBlendState();

	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);
};