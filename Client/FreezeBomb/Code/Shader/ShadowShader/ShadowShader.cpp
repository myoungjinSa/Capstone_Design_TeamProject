#include "../../Stdafx/Stdafx.h"
#include "ShadowShader.h"
#include "../../GameObject/GameObject.h"
#include "../../GameObject/Terrain/Terrain.h"
#include "../../GameObject/Surrounding/Surrounding.h"
#include "../../Mesh/Mesh.h"
#include "../../Texture/Texture.h"
#include "../../Material/Material.h"

CShadowShader::CShadowShader()
{
}

CShadowShader::~CShadowShader()
{
	if (m_ppd3dPipelineStates)
	{
		for (int i = 0; i < m_nPipelineStates; ++i)
			if (m_ppd3dPipelineStates[i])
				m_ppd3dPipelineStates[i]->Release();
		delete[] m_ppd3dPipelineStates;
	}
}

void CShadowShader::CreateShader(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 2;
	m_ppd3dPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];

	for (int i = 0; i < m_nPipelineStates; ++i)
	{
		::ZeroMemory(&m_d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
		m_d3dPipelineStateDesc.pRootSignature = pd3dGraphicsRootSignature;
		m_d3dPipelineStateDesc.VS = CreateVertexShader(i);
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

		HRESULT hResult = pd3dDevice->CreateGraphicsPipelineState(&m_d3dPipelineStateDesc, __uuidof(ID3D12PipelineState), (void **)&m_ppd3dPipelineStates[i]);
	}

	if (m_pd3dVertexShaderBlob) m_pd3dVertexShaderBlob->Release();
	if (m_pd3dPixelShaderBlob) m_pd3dPixelShaderBlob->Release();

	if (m_d3dPipelineStateDesc.InputLayout.pInputElementDescs) delete[] m_d3dPipelineStateDesc.InputLayout.pInputElementDescs;
}

D3D12_SHADER_BYTECODE CShadowShader::CreateVertexShader(int Type)
{
	switch (Type)
	{
	case Object:
		return(CShader::CompileShaderFromFile(L"../Code/Shader/HLSL/Shaders.hlsl", "VSIceCube", "vs_5_1", &m_pd3dVertexShaderBlob));
		break;

	case Shadow:
		return(CShader::CompileShaderFromFile(L"../Code/Shader/HLSL/Shaders.hlsl", "VSShadow", "vs_5_1", &m_pd3dVertexShaderBlob));
		break;
	}	
}

D3D12_SHADER_BYTECODE CShadowShader::CreatePixelShader(int Type)
{
	switch (Type)
	{
	case Object:
		return(CShader::CompileShaderFromFile(L"../Code/Shader/HLSL/Shaders.hlsl", "PSIceCube", "ps_5_1", &m_pd3dPixelShaderBlob));
		break;

	case Shadow:
		return(CShader::CompileShaderFromFile(L"../Code/Shader/HLSL/Shaders.hlsl", "PSShadow", "ps_5_1", &m_pd3dPixelShaderBlob));
		break;
	}
}

D3D12_INPUT_LAYOUT_DESC CShadowShader::CreateInputLayout()
{
	UINT nInputElementDescs = 2;
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_BLEND_DESC CShadowShader::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = true;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = false;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
	//d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;
	//d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}

D3D12_DEPTH_STENCIL_DESC CShadowShader::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = true;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = true;
	d3dDepthStencilDesc.StencilReadMask = 0xff;
	d3dDepthStencilDesc.StencilWriteMask = 0xff;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;

	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_INCR;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_EQUAL;

	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_INCR;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_EQUAL;

	return(d3dDepthStencilDesc);
}

void CShadowShader::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList, XMFLOAT4X4* pShadowWorld)
{
}

void CShadowShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature,
	const map<string, CTexture*>& Context, void *pContext)
{
	CCubeMeshTextured *pCubeMesh = new CCubeMeshTextured(pd3dDevice, pd3dCommandList, 4.f, 4.0f, 4.0f);

	CMaterial*pMaterial = new CMaterial(1);
	auto iter = Context.find("IceTexture");
	if (iter != Context.end())
		pMaterial->SetTexture((*iter).second, 0);

	m_nObjects = 5;
	m_ppObjects = new CGameObject*[m_nObjects];

	for (int i = 0; i < m_nObjects; ++i)
	{
		CCubeObject* pCube = new CCubeObject(1);
		XMFLOAT3 Position(Random(10, 490), 5, Random(10, 290));
		pCube->SetPosition(Position);
		pCube->SetMesh(pCubeMesh);
		pCube->SetMaterial(0, pMaterial);
		m_ppObjects[i] = pCube;
	}

	CCubeObject* pCube{ nullptr };
	for (int i = 0; i < m_nObjects; ++i)
	{
		 pCube = new CCubeObject(1);
		XMFLOAT3 Position(m_ppObjects[i]->GetPosition());
		pCube->SetPosition(Position);
		pCube->SetMesh(pCubeMesh);
		pCube->SetMaterial(0, pMaterial);
		m_ShadowObjectVector.emplace_back(pCube);
	}
}

void CShadowShader::OnPrepareRender(ID3D12GraphicsCommandList *pd3dCommandList, int nPipelineState)
{
	if (m_ppd3dPipelineStates)
		pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[nPipelineState]);
}

void CShadowShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	for (int i = 0; i < m_nObjects; ++i)
	{
		if (m_ppObjects[i])
		{
			CShadowShader::OnPrepareRender(pd3dCommandList, Object);
			m_ppObjects[i]->UpdateShaderVariable(pd3dCommandList, &m_ppObjects[i]->m_xmf4x4World);
			m_ppObjects[i]->Render(pd3dCommandList, pCamera);
		}
	}

	int i = 0;
	for (auto iter = m_ShadowObjectVector.begin(); iter != m_ShadowObjectVector.end(); ++iter)
	{
		CShadowShader::OnPrepareRender(pd3dCommandList, Shadow);
		(*iter)->UpdateShaderVariable(pd3dCommandList, &UpdateShadow(i));
		(*iter)->Render(pd3dCommandList, pCamera);
		++i;
	}
}

XMFLOAT4X4 CShadowShader::UpdateShadow(int index)
{
	// 점 광원
	// Light의 w 벡터가 그림자의 크기를 결정?	
	XMFLOAT4 xmf4Light(0.f, 35.f, 0.f, 1.f);
	//XMFLOAT4 xmf4Light(0.57735f, -0.57735f, 0.57735f, 0);

	// Plane의 w 벡터가 그림자의 y에 영향을 준다.
	XMFLOAT4 xmf4Plane(0.f, 1.f, 0.f, 4.9f);
	//XMFLOAT4 xmf4Plane(0.f, 1.f, 0.f, 0.f);

	// 그림자 행렬 생성
	XMMATRIX xmmtxPlane = XMMatrixShadow(XMLoadFloat4(&xmf4Plane), XMLoadFloat4(&xmf4Light));
	//XMMATRIX xmmtxPlane = XMMatrixShadow(XMLoadFloat4(&xmf4Plane), -XMLoadFloat4(&xmf4Light));

	// 그림자 행렬에 객체의 월드행렬을 곱해서 그림자의 월드행렬을 만들어 준다.
	XMFLOAT4X4 ShadowWorld = Matrix4x4::Multiply(xmmtxPlane, m_ppObjects[index]->m_xmf4x4World);

	return ShadowWorld;
}