#include "../../Stdafx/Stdafx.h"
#include "ShadowShader.h"
#include "../../GameObject/GameObject.h"
#include "../../GameObject/Terrain/Terrain.h"
#include "../../Texture/Texture.h"
#include "../../GameObject/Surrounding/Surrounding.h"

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
		m_d3dPipelineStateDesc.BlendState = CreateBlendState(i);
		m_d3dPipelineStateDesc.DepthStencilState = CreateDepthStencilState(i);
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
	case GameObject:
		return(CShader::CompileShaderFromFile(L"../Code/Shader/HLSL/Shaders.hlsl", "VSStandard", "vs_5_1", &m_pd3dVertexShaderBlob));
		break;
	case GameObject_Shadow:
		return(CShader::CompileShaderFromFile(L"../Code/Shader/HLSL/Shaders.hlsl", "VSShadow", "vs_5_1", &m_pd3dVertexShaderBlob));
		break;
	}	
}

D3D12_SHADER_BYTECODE CShadowShader::CreatePixelShader(int Type)
{
	switch (Type)
	{
	case GameObject:
		return(CShader::CompileShaderFromFile(L"../Code/Shader/HLSL/Shaders.hlsl", "PSStandard", "ps_5_1", &m_pd3dPixelShaderBlob));
		break;
	case GameObject_Shadow:
		return(CShader::CompileShaderFromFile(L"../Code/Shader/HLSL/Shaders.hlsl", "PSShadow", "ps_5_1", &m_pd3dPixelShaderBlob));
		break;
	}
}

D3D12_INPUT_LAYOUT_DESC CShadowShader::CreateInputLayout()
{
	UINT nInputElementDescs = 5;
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[2] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 2, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[3] = { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 3, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[4] = { "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 4, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_BLEND_DESC CShadowShader::CreateBlendState(int Type)
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	switch (Type)
	{
	case GameObject:
		d3dBlendDesc.AlphaToCoverageEnable = FALSE;
		d3dBlendDesc.IndependentBlendEnable = FALSE;
		d3dBlendDesc.RenderTarget[0].BlendEnable = FALSE;
		d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
		d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
		d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
		d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
		d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
		d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
		d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
		d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		break;

	case GameObject_Shadow:
		d3dBlendDesc.AlphaToCoverageEnable = true;
		d3dBlendDesc.IndependentBlendEnable = true;
		d3dBlendDesc.RenderTarget[0].BlendEnable = true;
		d3dBlendDesc.RenderTarget[0].LogicOpEnable = false;
		//d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
		d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		//d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;
		d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
		d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
		d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
		//d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
		d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_CLEAR;
		d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		break;
	}

	return(d3dBlendDesc);
}

D3D12_DEPTH_STENCIL_DESC CShadowShader::CreateDepthStencilState(int Type)
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));

	switch (Type)
	{
	case GameObject:
		//d3dDepthStencilDesc.DepthEnable = TRUE;
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
		break;

	case GameObject_Shadow:
		d3dDepthStencilDesc.DepthEnable = false;
		//d3dDepthStencilDesc.DepthEnable = true;
		d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		//d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		d3dDepthStencilDesc.StencilEnable = true;
		d3dDepthStencilDesc.StencilReadMask = 0xff;
		//d3dDepthStencilDesc.StencilReadMask = 0x02;
		d3dDepthStencilDesc.StencilWriteMask = 0xff;
		//d3dDepthStencilDesc.StencilWriteMask = 0x02;
		d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
		d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
		d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_INCR;
		//d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_REPLACE;
		d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_EQUAL;
		//d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NOT_EQUAL;

		d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
		d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
		d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_INCR;
		//d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_REPLACE;
		d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_EQUAL;
		//d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		break;
	}

	return(d3dDepthStencilDesc);
}

void CShadowShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature,
	const map<string, CTexture*>& Context, void *pContext)
{
	CTerrain* pTerrain = (CTerrain*)pContext;

	m_nObjects = 5;
	m_ppObjects = new CGameObject*[m_nObjects];
	XMFLOAT3 Position(0, 0, 0);

	CLoadedModelInfo* pDeer01 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, 
		"../Resource/Models/SM_Deer3.bin", this, false, "Surrounding");

	for (int i = 0; i < m_nObjects; ++i)
	{
		m_ppObjects[i] = new CSurrounding(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
		Position = XMFLOAT3(Random(10.f, 490.f), 0, Random(10.f, 290.f));
		m_ppObjects[i]->SetPosition(Position);
		m_ppObjects[i]->SetScale(10, 10, 10);
		m_ppObjects[i]->SetChild(pDeer01->m_pModelRootObject, true);
		m_ppObjects[i]->setID("<Deer01>");
	}

	for (int i = 0; i < m_nObjects; ++i)
	{
		CSurrounding* pShadow = new CSurrounding(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
		Position = XMFLOAT3(m_ppObjects[i]->GetPosition());
		pShadow->SetPosition(Position);
		pShadow->SetScale(10, 10, 10);
		pShadow->SetChild(pDeer01->m_pModelRootObject, true);
		m_ShadowObjectVector.emplace_back(pShadow);
	}

	if (pDeer01)
		delete pDeer01;
}

void CShadowShader::OnPrepareRender(ID3D12GraphicsCommandList *pd3dCommandList, int nPipelineState)
{
	if (m_ppd3dPipelineStates)
		pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[nPipelineState]);
}

void CShadowShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera, int nPipelineState)
{
	int i = 0;
	for (auto iter = m_ShadowObjectVector.begin(); iter != m_ShadowObjectVector.end(); ++iter)
	{
		CShadowShader::OnPrepareRender(pd3dCommandList, GameObject_Shadow);
		(*iter)->UpdateTransform(nullptr);
		(*iter)->Render(pd3dCommandList, pCamera);
	}

	for (int i = 0; i < m_nObjects; ++i)
	{
		if (m_ppObjects[i])
		{
			CShadowShader::OnPrepareRender(pd3dCommandList, GameObject);
			m_ppObjects[i]->UpdateTransform(nullptr);
			m_ppObjects[i]->Render(pd3dCommandList, pCamera);
		}
	}
}

void CShadowShader::ReleaseObjects() 
{
	if (m_ppObjects)
	{
		for (int i = 0; i < m_nObjects; ++i)
			if (m_ppObjects[i])
				m_ppObjects[i]->Release();
		delete[] m_ppObjects;
	}

	for (auto iter = m_ShadowObjectVector.begin(); iter != m_ShadowObjectVector.end();)
	{
		delete (*iter);
		iter = m_ShadowObjectVector.erase(iter);
	}
	m_ShadowObjectVector.clear();
}

void CShadowShader::ReleaseUploadBuffers()
{
	for (int i = 0; i < m_nObjects; ++i)
		if (m_ppObjects[i])
			m_ppObjects[i]->ReleaseUploadBuffers();

	for (auto iter = m_ShadowObjectVector.begin(); iter != m_ShadowObjectVector.end(); ++iter)
		(*iter)->ReleaseUploadBuffers();
}

XMFLOAT4X4 CShadowShader::UpdateShadow(int index)
{
	// 점 광원
	// 빛의 위치를 나타내는 4D 벡터. 광원의 w- 성분이 0.0f이면 원점에서 빛까지의 광선이 방향성을 나타냅니다. 1.0f이면 점등입니다.
	// Light의 w 벡터가 그림자의 크기를 결정?
	float x, y, z, w;
	x = 0;
	y = 1.;
	z = 0;
	w = 1.0;
	//XMFLOAT4 xmf4Light(x, y, z, w);
	//XMFLOAT4 xmf4Light(35.f, 35.f,35.f, 10.f);
	XMFLOAT4 xmf4Light(0.57735f, -0.57735f, 0.57735f, 0);

	// Plane의 w 벡터가 그림자의 y에 영향을 준다.
	XMFLOAT4 xmf4Plane(0.f, 1.f, 0.f, 0.f);
	//XMFLOAT4 xmf4Plane(0.f, 1.f, 0.f, 0.f);

	// 그림자 행렬 생성
	//XMMATRIX xmmtxPlane = XMMatrixShadow(XMLoadFloat4(&xmf4Plane), XMLoadFloat4(&xmf4Light));
	XMMATRIX xmmtxPlane = XMMatrixShadow(XMLoadFloat4(&xmf4Plane), -XMLoadFloat4(&xmf4Light));

	// 그림자 행렬에 객체의 월드행렬을 곱해서 그림자의 월드행렬을 만들어 준다.
	XMFLOAT4X4 ShadowWorld = Matrix4x4::Multiply(xmmtxPlane, m_ppObjects[index]->m_xmf4x4World);
	
	//cout << ShadowWorld._41 << ", " << ShadowWorld._42 << ", " << ShadowWorld._43 << endl;
	return ShadowWorld;
}

XMMATRIX CShadowShader::UpdateXMShadow(int index)
{
	XMFLOAT4 xmf4Light(0.57735f, -0.57735f, 0.57735f, 0);
	// Plane의 w 벡터가 그림자의 y에 영향을 준다.
	XMFLOAT4 xmf4Plane(0.f, 1.f, 0.f, 0.f);
	//XMFLOAT4 xmf4Plane(0.f, 1.f, 0.f, 0.f);

	// 그림자 행렬 생성
	//XMMATRIX xmmtxPlane = XMMatrixShadow(XMLoadFloat4(&xmf4Plane), XMLoadFloat4(&xmf4Light));
	XMMATRIX xmmtxPlane = XMMatrixShadow(XMLoadFloat4(&xmf4Plane), -XMLoadFloat4(&xmf4Light));

	return xmmtxPlane;
}