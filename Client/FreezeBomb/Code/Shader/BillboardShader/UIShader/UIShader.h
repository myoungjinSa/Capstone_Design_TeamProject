#pragma once

#include "../BillboardShader.h"

class CMaterial;
class CUI;
class CUIShader : public CBillboardShader
{
public:
	CUIShader();
	virtual ~CUIShader();

	virtual D3D12_SHADER_BYTECODE CreatePixelShader();
	virtual D3D12_RASTERIZER_DESC CreateRasterizerState();
	virtual D3D12_BLEND_DESC CreateBlendState();
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();
	virtual void OnPrepareRender(ID3D12GraphicsCommandList *pd3dCommandList, int nPipelineState);

	virtual void ReleaseUploadBuffers();
	virtual void ReleaseObjects();
	
protected:
	ID3D12PipelineState**				m_ppd3dPipelineStates{ nullptr };
	int												m_nPipelineStates = 0;

	CMaterial**			m_ppUIMaterial;
	map<int, CUI*>	m_UIMap;
};