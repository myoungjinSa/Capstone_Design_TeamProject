#include "../../Stdafx/Stdafx.h"
#include "SkyBoxShader.h"
#include "../../GameObject/SkyBox/SkyBox.h"
#include "../../Material/Material.h"
#include "../../Texture/Texture.h"

CSkyBoxShader::CSkyBoxShader()
{
}

CSkyBoxShader::~CSkyBoxShader()
{
}

D3D12_INPUT_LAYOUT_DESC CSkyBoxShader::CreateInputLayout()
{
	UINT nInputElementDescs = 1;
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_DEPTH_STENCIL_DESC CSkyBoxShader::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	d3dDepthStencilDesc.DepthEnable = FALSE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0xff;
	d3dDepthStencilDesc.StencilWriteMask = 0xff;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_INCR;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_DECR;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

	return(d3dDepthStencilDesc);
}

D3D12_SHADER_BYTECODE CSkyBoxShader::CreateVertexShader()
{
	return(CShader::CompileShaderFromFile(L"../Code/Shader/HLSL/Shaders.hlsl", "VSSkyBox", "vs_5_1", &m_pd3dVertexShaderBlob));
}

D3D12_SHADER_BYTECODE CSkyBoxShader::CreatePixelShader()
{
	return(CShader::CompileShaderFromFile(L"../Code/Shader/HLSL/Shaders.hlsl", "PSSkyBox", "ps_5_1", &m_pd3dPixelShaderBlob));
}

void CSkyBoxShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature,
	const map<string, CTexture*>& Context, void* pContext)
{
	m_pSkyBox = new CSkyBox(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);

	CSkyBoxMesh *pSkyBoxMesh = new CSkyBoxMesh(pd3dDevice, pd3dCommandList, 20.0f, 20.0f, 2.0f);
	m_pSkyBox->SetMesh(pSkyBoxMesh);

	CMaterial *pSkyBoxMaterial = new CMaterial(1);
	auto iter = Context.find("SkyBox");
	if(iter != Context.end())
		pSkyBoxMaterial->SetTexture((*iter).second);
	m_pSkyBox->SetMaterial(0, pSkyBoxMaterial);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CSkyBoxShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera, int nPipelineState)
{
	CShader::Render(pd3dCommandList, pCamera, nPipelineState);

	UpdateShaderVariables(pd3dCommandList);

	if (m_pSkyBox)
		m_pSkyBox->Render(pd3dCommandList, pCamera, nPipelineState);
}

void CSkyBoxShader::ReleaseObjects()
{
	if (m_pSkyBox)
		m_pSkyBox->Release();
}

void CSkyBoxShader::ReleaseUploadBuffers()
{
	if (m_pSkyBox)
		m_pSkyBox->ReleaseUploadBuffers();
}

void CSkyBoxShader::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_World) + 255) & ~255); //256ÀÇ ¹è¼ö
	m_pd3dcbWorld = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dcbWorld->Map(0, nullptr, (void**)&m_pcbMappedWorld);
}

void CSkyBoxShader::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	if (m_pd3dcbWorld)
	{
		XMStoreFloat4x4(&m_pcbMappedWorld->m_World, XMMatrixTranspose(XMLoadFloat4x4(&m_pSkyBox->m_xmf4x4World)));
		D3D12_GPU_VIRTUAL_ADDRESS GpuVirtualAddress = m_pd3dcbWorld->GetGPUVirtualAddress();
		pd3dCommandList->SetGraphicsRootConstantBufferView(1, GpuVirtualAddress);
	}
}

void CSkyBoxShader::ReleaseShaderVariables()
{
	if (m_pd3dcbWorld)
	{
		m_pd3dcbWorld->Unmap(0, nullptr);
		m_pd3dcbWorld->Release();
	}
}