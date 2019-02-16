#include "../../Stdafx/Stdafx.h"
#include "ShadowShader.h"
#include "../../GameObject/GameObject.h"
#include "../../GameObject/Terrain/Terrain.h"
#include "../../GameObject/Surrounding/Surrounding.h"

CShadowShader::CShadowShader()
{
}

CShadowShader::~CShadowShader()
{
}

D3D12_SHADER_BYTECODE CShadowShader::CreateVertexShader()
{
	return(CShader::CompileShaderFromFile(L"../Code/Shader/HLSL/Shaders.hlsl", "VSShadow", "vs_5_1", &m_pd3dVertexShaderBlob));
}

D3D12_SHADER_BYTECODE CShadowShader::CreatePixelShader()
{
	return(CShader::CompileShaderFromFile(L"../Code/Shader/HLSL/Shaders.hlsl", "PSShadow", "ps_5_1", &m_pd3dPixelShaderBlob));
}

D3D12_INPUT_LAYOUT_DESC CShadowShader::CreateInputLayout()
{
	UINT nInputElementDescs = 1;
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

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
	//d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	//d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
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
	//XMFLOAT4X4 xmf4x4World;
	//XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(pShadowWorld)));
	//pd3dCommandList->SetGraphicsRoot32BitConstants(20, 16, &xmf4x4World, 0);
}

void CShadowShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, void *pContext)
{
	CLoadedModelInfo* pDeer01 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_Deer.bin", this, false);
	m_nObjects = 1;
	m_ppObjects = new CGameObject*[m_nObjects];
	m_ppObjects[0] = new CSurrounding(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	
	CTerrain* pTerrain = (CTerrain*)pContext;

	XMFLOAT3 Position;
	Position.x = Random(10.f, 490.f);
	Position.z = Random(10.f, 290.f);
	m_ppObjects[0]->SetPosition(Position.x, pTerrain->GetHeight(Position.x, Position.z), Position.z);
	m_ppObjects[0]->SetChild(pDeer01->m_pModelRootObject, true);

	cout << Position.x << ", " << Position.z << endl;

	if (pDeer01)
		delete pDeer01;
}

void CShadowShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CShader::Render(pd3dCommandList, pCamera);

	// 점 광원
	//XMFLOAT4 xmf4Light(0.f, 35.f, 0.f, 1.f);
	XMFLOAT4 xmf4Light(0.57735f, 1.f, 0.57735f, 0.f);
	XMFLOAT4 xmf4Plane(0.f, 1.f, 0.f, 0.f);

	for (int i = 0; i < m_nObjects; ++i)
	{
		if (m_ppObjects[i])
		{
			// 그림자 행렬 생성
			XMMATRIX xmmtxPlane = XMMatrixShadow(XMLoadFloat4(&xmf4Plane), XMLoadFloat4(&xmf4Light));
			XMFLOAT4X4 xmf4x4Plane;
			// 그림자 행렬을 XMFLOAT4X4로 변환
			XMStoreFloat4x4(&xmf4x4Plane, xmmtxPlane);
			// 그림자 행렬에 객체의 월드행렬을 곱해서 그림자의 월드행렬을 만들어 준다.
			XMFLOAT4X4 ShadowWorld = Matrix4x4::Multiply(xmf4x4Plane, m_ppObjects[i]->m_xmf4x4World);
			// 그림자의 월드행렬을 상수버퍼로 넘겨준다.
			m_ppObjects[i]->m_xmf4x4World = ShadowWorld;
			m_ppObjects[i]->UpdateTransform(nullptr);
			m_ppObjects[i]->Render(pd3dCommandList, pCamera);


			//// 그림자 행렬 생성
			//XMMATRIX xmmtxPlane = XMMatrixShadow(XMLoadFloat4(&xmf4Plane), XMLoadFloat4(&xmf4Light));
			//XMFLOAT4X4 xmf4x4Plane;
			//// 그림자 행렬에 객체의 월드행렬을 곱해서 그림자의 월드행렬을 만들어 준다.
			//// 그림자 행렬을 XMFLOAT4X4로 변환
			//XMStoreFloat4x4(&xmf4x4Plane, XMLoadFloat4x4(&m_ppObjects[i]->m_xmf4x4World) * xmmtxPlane);

			//m_ppObjects[i]->UpdateTransform(nullptr);
			//m_ppObjects[i]->Render(pd3dCommandList, pCamera);
		}
	}
}
