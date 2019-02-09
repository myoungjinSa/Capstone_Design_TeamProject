#pragma once

#include "../BillboardShader.h"

class CUIShader : public CBillboardShader
{
public:
	CUIShader();
	virtual ~CUIShader();

	virtual D3D12_RASTERIZER_DESC CreateRasterizerState();
	virtual D3D12_BLEND_DESC CreateBlendState();
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();

	void setTime(float time) { m_Time = time; }
	float getTime()	const { return m_Time; }
protected:
	float m_Time = 0;
};