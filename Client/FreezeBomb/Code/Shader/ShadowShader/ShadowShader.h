#pragma once

#include "../Shader.h"

class CTexture;
class CShadowShader : public CShader
{
public:
	CShadowShader();
	virtual ~CShadowShader();

	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(int Type);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(int Type);
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_BLEND_DESC CreateBlendState(int Type);
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState(int Type);

	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, 
		const map<string, CTexture*>& Context, void *pContext = NULL);

	XMFLOAT4X4 UpdateShadow(int index);
	XMMATRIX UpdateXMShadow(int index);

	virtual void OnPrepareRender(ID3D12GraphicsCommandList *pd3dCommandList, int nPipelineState);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera, int nPipelineState);

	virtual void ReleaseObjects();
	virtual void ReleaseUploadBuffers();

private:
	ID3D12PipelineState**				m_ppd3dPipelineStates{ nullptr };
	int												m_nPipelineStates = 0;

	vector<CGameObject*>			m_ShadowObjectVector;
};