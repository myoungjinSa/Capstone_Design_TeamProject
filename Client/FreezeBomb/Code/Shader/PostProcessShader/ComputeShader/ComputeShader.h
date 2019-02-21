#pragma once

#include "../../Shader.h"

class CTexture;

D3D12_UNORDERED_ACCESS_VIEW_DESC GetUnorderedAccessViewDesc(D3D12_RESOURCE_DESC d3dResourceDesc);

class CComputeShader : public CShader
{
public:
	CComputeShader();
	virtual ~CComputeShader();

	virtual D3D12_SHADER_BYTECODE CreateComputeShader();

	virtual void CreateGraphicsRootSignature(ID3D12Device *pd3dDevice);
	virtual void CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature *pd3dGraphicsRootSignature, UINT nRenderTargets);


	void CreateCbvAndSrvAndUavDescriptorHeaps(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, int nConstantBufferView, int nShaderResourceView, int nUnorderedAccessView);
	void CreateSrvAndUavViews(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList,CTexture *pTexture,UINT nRootParameterStartIndex,UINT nRootParameterStartIndex2);

	ID3D12RootSignature *GetGraphicsRootSignature() { return(m_pd3dComputeRootSignature); }


protected:
	//ID3D12RootSignature				*m_pd3dGraphicsRootSignature{ nullptr };
	ID3D12RootSignature				*m_pd3dComputeRootSignature{ nullptr };

	ID3D12PipelineState				**m_ppd3dPipelineStates{ nullptr };
	UINT							m_nPipelineStates;

	ID3DBlob*						m_pd3dComputeBlob{ nullptr };

	UINT							m_nSrvUavDescriptorIncrementSize;

	ID3D12DescriptorHeap			*m_pd3dCbvSrvUavDescriptorHeap = NULL;
	

	D3D12_CPU_DESCRIPTOR_HANDLE		m_d3dCbvCPUDescriptorStartHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE		m_d3dCbvGPUDescriptorStartHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE		m_d3dSrvCPUDescriptorStartHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE		m_d3dSrvGPUDescriptorStartHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE		m_d3dUavCPUDescriptorStartHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE		m_d3dUavGPUDescriptorStartHandle;

};