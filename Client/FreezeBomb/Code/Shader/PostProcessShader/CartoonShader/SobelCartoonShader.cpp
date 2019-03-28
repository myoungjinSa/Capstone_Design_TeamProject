
#include "../../../Stdafx/Stdafx.h"
#include "../../../Texture/Texture.h"
#include "SobelCartoonShader.h"

CSobelCartoonShader::CSobelCartoonShader()
	:m_nHeight(0),m_nWidth(0),m_nTexture(0)
{

}
CSobelCartoonShader::~CSobelCartoonShader()
{
}

D3D12_INPUT_LAYOUT_DESC CSobelCartoonShader::CreateInputLayout()
{
	UINT nInputElementDescs = 2;
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE CSobelCartoonShader::CreateVertexShader()
{
	return(CShader::CompileShaderFromFile(L"../Code/Shader/HLSL/Cartoon.hlsl", "VSToonShader", "vs_5_1", &m_pd3dVertexShaderBlob));
}
D3D12_SHADER_BYTECODE CSobelCartoonShader::CreatePixelShader()
{
	return(CShader::CompileShaderFromFile(L"../Code/Shader/HLSL/Cartoon.hlsl", "PSToonShader", "ps_5_1", &m_pd3dPixelShaderBlob));
}



void CSobelCartoonShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, CTexture *pContext)
{
	m_nTexture = 1;
	m_pTexture = new CTexture*[m_nTexture];

	m_pTexture[0] = (CTexture *)pContext;

	//auto iter = Context.find("CartoonRenderTarget1");
	//if (iter != Context.end())
	//{
	//	m_pTexture[0] = iter->second;
	//}


	CreateCbvAndSrvAndUavDescriptorHeaps(pd3dDevice, pd3dCommandList, 0, 2, 2);

	CreateSrvAndUavViews(pd3dDevice, pd3dCommandList, m_pTexture[0], 0, 2);
}

void CSobelCartoonShader::ReleaseObjects()
{
	if (m_pTexture)
	{
		for (int i = 0; i < m_nTexture; i++) 
			if(m_pTexture[i])
				m_pTexture[i]->Release();
	}
}
void CSobelCartoonShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, ID3D12Resource *pRenderTexture, ID3D12Resource **pDestTexture, CCamera *pCamera)
{
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dComputeRootSignature);
	pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[1]);

	if (m_pd3dCbvSrvUavDescriptorHeap) 
		pd3dCommandList->SetDescriptorHeaps(1, &m_pd3dCbvSrvUavDescriptorHeap);

	pd3dCommandList->SetGraphicsRootDescriptorTable(0, m_pTexture[0]->m_pRootArgumentInfos[0].m_d3dSrvGpuDescriptorHandle);
	pd3dCommandList->SetGraphicsRootDescriptorTable(1, m_pTexture[0]->m_pRootArgumentInfos[1].m_d3dSrvGpuDescriptorHandle);
	pd3dCommandList->SetGraphicsRootDescriptorTable(2, m_pTexture[0]->m_pRootArgumentInfos[1].m_d3dUavGpuDescriptorHandle);

	pd3dCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pd3dCommandList->DrawInstanced(6, 1, 0, 0);
}