#pragma once

#include "../Shader.h"

class CGameObject;

class CStandardShader : public CShader
{
public:
	CStandardShader();
	virtual ~CStandardShader();

	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(int nPipelineState);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(int nPipelineState);
	virtual D3D12_BLEND_DESC CreateBlendState(int nPipelineState);
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState(int nPipelineState);
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_RASTERIZER_DESC CreateRasterizerState();

	virtual void OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList, int nPipelineState);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState = GameObject);

	virtual void ReleaseObjects();
	virtual void ReleaseUploadBuffers();

	//스테이지에 따라 다른 Pipeline이 필요하기 때문에 Stage와 PSO를 매핑 해야한다.
	int MappingStageToPiplineStates(const int& stage,const int& pipelineStates);
protected:

	ID3D12PipelineState**				m_ppd3dPipelineStates{ nullptr };
	int												m_nPipelineStates = 0;
};
