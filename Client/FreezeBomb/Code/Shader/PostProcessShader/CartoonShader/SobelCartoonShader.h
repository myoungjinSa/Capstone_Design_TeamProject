#pragma once
#include "../ComputeShader/ComputeShader.h"


class CSobelCartoonShader : public CComputeShader
{
public:
	CSobelCartoonShader();
	virtual ~CSobelCartoonShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader();
	virtual D3D12_SHADER_BYTECODE CreatePixelShader();

	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, CTexture *pContext);
	virtual void ReleaseObjects();
	
	void SobelFilter(ID3D12GraphicsCommandList *pd3dCommandList, ID3D12Resource* pRenderTexture, ID3D12Resource **pDestTexture, CCamera *pCamera);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, ID3D12Resource *pRenderTexture, ID3D12Resource **pDestTexture, CCamera *pCamera);

protected:
	
	CTexture**				m_pTexture{ nullptr };
	int								m_nTexture;

	UINT							m_nWidth;
	UINT							m_nHeight;

};