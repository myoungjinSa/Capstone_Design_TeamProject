#pragma once

#include "../StandardShader.h"

class CPlayerSkinnedShader : public CStandardShader
{
public:
	CPlayerSkinnedShader();
	virtual ~CPlayerSkinnedShader();

	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(int nPipelineState);
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera, int nPipelineState);
};