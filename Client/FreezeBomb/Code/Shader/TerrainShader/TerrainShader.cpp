#include "../../Stdafx/Stdafx.h"
#include "TerrainShader.h"
#include "../../GameObject/Terrain/Terrain.h"
#include "../../Material/Material.h"
#include "../../Texture/Texture.h"
#include "../../Scene/Scene.h"

CTerrainShader::CTerrainShader()
{
}

CTerrainShader::~CTerrainShader()
{
}

D3D12_SHADER_BYTECODE CTerrainShader::CreateVertexShader()
{
	return(CShader::CompileShaderFromFile(L"../Code/Shader/HLSL/Shaders.hlsl", "VSTerrain", "vs_5_1", &m_pd3dVertexShaderBlob));
}

D3D12_SHADER_BYTECODE CTerrainShader::CreatePixelShader()
{
	return(CShader::CompileShaderFromFile(L"../Code/Shader/HLSL/Shaders.hlsl", "PSTerrain", "ps_5_1", &m_pd3dPixelShaderBlob));
}

D3D12_DEPTH_STENCIL_DESC CTerrainShader::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = false;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return(d3dDepthStencilDesc);
}

D3D12_INPUT_LAYOUT_DESC CTerrainShader::CreateInputLayout()
{
	UINT nInputElementDescs = 4;
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[2] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 2, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[3] = { "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 3, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}


void CTerrainShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature,
	const map<string, CTexture*>& Context, void* pContext)
{
	int nWidth = 256, nLength = 256;
	XMFLOAT3 xmf3Scale(2.f, 1.0f, 1.2f);
	XMFLOAT4 xmf4Color(0.0f, 0.3f, 0.0f, 0.0f);

	m_pTerrain = new CTerrain(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, _T("../Resource/Textures/Terrain/Terrain.raw"), nWidth, nLength, xmf3Scale, xmf4Color);

	CHeightMapGridMesh* pMesh = new CHeightMapGridMesh(pd3dDevice, pd3dCommandList, 0, 0, nWidth, nLength, xmf3Scale, xmf4Color, m_pTerrain->getHeightMapImage());
	m_pTerrain->SetMesh(pMesh);

	CMaterial* pTerrainMaterial = new CMaterial(2);
	auto iter = Context.find("BaseTerrain");
	if (iter != Context.end())
		pTerrainMaterial->SetTexture((*iter).second, 0);
	iter = Context.find("SpecularTerrain");
	if (iter != Context.end())
		pTerrainMaterial->SetTexture((*iter).second, 1);

	m_pTerrain->SetMaterial(0, pTerrainMaterial);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CTerrainShader::ReleaseObjects()
{
	if (m_pTerrain)
		delete m_pTerrain;
}

void CTerrainShader::ReleaseUploadBuffers()
{
	if (m_pTerrain)
		m_pTerrain->ReleaseUploadBuffers();
}

void CTerrainShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera, int nPipelineState)
{
	CShader::Render(pd3dCommandList, pCamera, nPipelineState);

	UpdateShaderVariables(pd3dCommandList);
	D3D12_GPU_VIRTUAL_ADDRESS GpuVirtualAddress = m_pd3dcbWorld->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(23, GpuVirtualAddress);
	if (m_pTerrain)
		m_pTerrain->Render(pd3dCommandList, pCamera);
}

void CTerrainShader::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_World) + 255) & ~255); //256ÀÇ ¹è¼ö
	m_pd3dcbWorld = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dcbWorld->Map(0, nullptr, (void**)&m_pcbMappedWorld);
}

void CTerrainShader::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	if(m_pd3dcbWorld)
		XMStoreFloat4x4(&m_pcbMappedWorld->m_World, XMMatrixTranspose(XMLoadFloat4x4(&m_pTerrain->m_xmf4x4World)));
}

void CTerrainShader::ReleaseShaderVariables()
{
	if (m_pd3dcbWorld)
	{
		m_pd3dcbWorld->Unmap(0, nullptr);
		m_pd3dcbWorld->Release();
	}
}