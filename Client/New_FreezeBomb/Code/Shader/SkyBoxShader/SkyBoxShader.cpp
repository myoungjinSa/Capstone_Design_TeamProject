#include "../../Stdafx/Stdafx.h"
#include "SkyBoxShader.h"
#include "../../GameObject/SkyBox/SkyBox.h"
#include "../../Material/Material.h"
#include "../../Texture/Texture.h"


extern unsigned char g_Round;
CSkyBoxShader::CSkyBoxShader()
{
}

CSkyBoxShader::~CSkyBoxShader()
{
}

void CSkyBoxShader::CreateShader(ID3D12Device *pd3dDevice,ID3D12GraphicsCommandList *pd3dCommandList,ID3D12RootSignature *pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 2;
	m_ppd3dPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];

	for(int i=0; i <m_nPipelineStates;++i)
	{
		::ZeroMemory(&m_d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
		m_d3dPipelineStateDesc.pRootSignature = pd3dGraphicsRootSignature;
		m_d3dPipelineStateDesc.VS = CreateVertexShader();
		m_d3dPipelineStateDesc.PS = CreatePixelShader(i);
		m_d3dPipelineStateDesc.RasterizerState = CreateRasterizerState();
		m_d3dPipelineStateDesc.BlendState = CreateBlendState();
		m_d3dPipelineStateDesc.DepthStencilState = CreateDepthStencilState();
		m_d3dPipelineStateDesc.InputLayout = CreateInputLayout();
		m_d3dPipelineStateDesc.SampleMask = UINT_MAX;
		m_d3dPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		m_d3dPipelineStateDesc.NumRenderTargets = 1;
		m_d3dPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		m_d3dPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		m_d3dPipelineStateDesc.SampleDesc.Count = 1;
		m_d3dPipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

		HRESULT hResult = pd3dDevice->CreateGraphicsPipelineState(&m_d3dPipelineStateDesc, __uuidof(ID3D12PipelineState), (void**)&m_ppd3dPipelineStates[i]);	

		if (m_pd3dVertexShaderBlob) m_pd3dVertexShaderBlob->Release();
		if (m_pd3dPixelShaderBlob) m_pd3dPixelShaderBlob->Release();
		if (m_d3dPipelineStateDesc.InputLayout.pInputElementDescs) delete[] m_d3dPipelineStateDesc.InputLayout.pInputElementDescs;
	}
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

D3D12_SHADER_BYTECODE CSkyBoxShader::CreatePixelShader(int shaderVersion)
{
	D3D12_SHADER_BYTECODE p;
	switch(shaderVersion)
	{
	case SHADER_TYPE::FOG:
	{
		p = CShader::CompileShaderFromFile(L"../Code/Shader/HLSL/Shaders.hlsl", "PSFogSkyBox", "ps_5_1", &m_pd3dPixelShaderBlob);
		break;
	}	
	case SHADER_TYPE::NO_FOG:
	{
		p = CShader::CompileShaderFromFile(L"../Code/Shader/HLSL/Shaders.hlsl", "PSSkyBox", "ps_5_1", &m_pd3dPixelShaderBlob);
		break;
	}
	}
	return p;
}

void CSkyBoxShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature,
	const map<string, CTexture*>& Context, void* pContext)
{
	m_pSkyBox = new CSkyBox(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);

	CSkyBoxMesh *pSkyBoxMesh = new CSkyBoxMesh(pd3dDevice, pd3dCommandList, 20.0f, 20.0f, 2.0f);
	m_pSkyBox->SetMesh(pSkyBoxMesh);

	CMaterial *pSkyBoxMaterial = new CMaterial(3);
	auto iter = Context.find("SkyBox1");
	if(iter != Context.end())
		pSkyBoxMaterial->SetTexture((*iter).second,0);

	iter = Context.find("SkyBox2");
	if (iter != Context.end())
		pSkyBoxMaterial->SetTexture((*iter).second, 1);

	iter = Context.find("SkyBox3");
	if (iter != Context.end())
		pSkyBoxMaterial->SetTexture((*iter).second, 2);

	m_pSkyBox->SetMaterial(0, pSkyBoxMaterial);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CSkyBoxShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera, int nPipelineState)
{
	//CShader::Render(pd3dCommandList, pCamera, nPipelineState);
	nPipelineState = MappingStageToPipelineState(g_Round);
	if (m_ppd3dPipelineStates[nPipelineState])
		pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[nPipelineState]);

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

int CSkyBoxShader::MappingStageToPipelineState(int stage)
{
	enum ROUND {ROUND_0,ROUND_1,ROUND_2};
	int ret= ROUND_0;
	switch(stage)
	{
	case ROUND_0:
		ret = FOG;
		break;
	case ROUND_1:
		ret = NO_FOG;
		break;
	case ROUND_2:
		ret = FOG;
		break;
	}
	return ret;
}