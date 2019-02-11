//-----------------------------------------------------------------------------
// File: Shader.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Shader.h"
#include "Scene.h"


int getRandomNumber(int min, int max)
{
	//1단계 시드 설정
	std::random_device rn;
	default_random_engine rnd(rn());


	//2단계 분포 설정(실수)
	std::uniform_int_distribution<int> range(min, max);

	return range(rnd);
}

float Random(float, float);

CShader::CShader()
{
}

CShader::~CShader()
{
	ReleaseShaderVariables();

	if (m_pd3dPipelineState) m_pd3dPipelineState->Release();
}

D3D12_SHADER_BYTECODE CShader::CreateVertexShader()
{
	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = 0;
	d3dShaderByteCode.pShaderBytecode = NULL;

	return(d3dShaderByteCode);
}

D3D12_SHADER_BYTECODE CShader::CreatePixelShader()
{
	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = 0;
	d3dShaderByteCode.pShaderBytecode = NULL;

	return(d3dShaderByteCode);
}

D3D12_SHADER_BYTECODE CShader::CompileShaderFromFile(WCHAR *pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderProfile, ID3DBlob **ppd3dShaderBlob)
{
	UINT nCompileFlags = 0;
#if defined(_DEBUG)
	nCompileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	ID3DBlob* errorBlob = nullptr;

	::D3DCompileFromFile(pszFileName, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, pszShaderName, pszShaderProfile, nCompileFlags, 0, ppd3dShaderBlob, &errorBlob);

	if (errorBlob)
	{
		OutputDebugStringA((char*)errorBlob->GetBufferPointer());
		errorBlob->Release();
	}


	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = (*ppd3dShaderBlob)->GetBufferSize();
	d3dShaderByteCode.pShaderBytecode = (*ppd3dShaderBlob)->GetBufferPointer();

	return(d3dShaderByteCode);
}

#define _WITH_WFOPEN
//#define _WITH_STD_STREAM

#ifdef _WITH_STD_STREAM
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#endif

D3D12_SHADER_BYTECODE CShader::ReadCompiledShaderFromFile(WCHAR *pszFileName, ID3DBlob **ppd3dShaderBlob)
{
	UINT nReadBytes = 0;
#ifdef _WITH_WFOPEN
	FILE *pFile = NULL;
	::_wfopen_s(&pFile, pszFileName, L"rb");
	::fseek(pFile, 0, SEEK_END);
	int nFileSize = ::ftell(pFile);
	BYTE *pByteCode = new BYTE[nFileSize];
	::rewind(pFile);
	nReadBytes = (UINT)::fread(pByteCode, sizeof(BYTE), nFileSize, pFile);
	::fclose(pFile);
#endif
#ifdef _WITH_STD_STREAM
	std::ifstream ifsFile;
	ifsFile.open(pszFileName, std::ios::in | std::ios::ate | std::ios::binary);
	nReadBytes = (int)ifsFile.tellg();
	BYTE *pByteCode = new BYTE[*pnReadBytes];
	ifsFile.seekg(0);
	ifsFile.read((char *)pByteCode, nReadBytes);
	ifsFile.close();
#endif

	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	if (ppd3dShaderBlob)
	{
		*ppd3dShaderBlob = NULL;
		HRESULT hResult = D3DCreateBlob(nReadBytes, ppd3dShaderBlob);
		memcpy((*ppd3dShaderBlob)->GetBufferPointer(), pByteCode, nReadBytes);
		d3dShaderByteCode.BytecodeLength = (*ppd3dShaderBlob)->GetBufferSize();
		d3dShaderByteCode.pShaderBytecode = (*ppd3dShaderBlob)->GetBufferPointer();
	}
	else
	{
		d3dShaderByteCode.BytecodeLength = nReadBytes;
		d3dShaderByteCode.pShaderBytecode = pByteCode;
	}

	return(d3dShaderByteCode);
}

D3D12_INPUT_LAYOUT_DESC CShader::CreateInputLayout()
{
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = NULL;
	d3dInputLayoutDesc.NumElements = 0;

	return(d3dInputLayoutDesc);
}

D3D12_RASTERIZER_DESC CShader::CreateRasterizerState()
{
	D3D12_RASTERIZER_DESC d3dRasterizerDesc;
	::ZeroMemory(&d3dRasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));
	//	d3dRasterizerDesc.FillMode = D3D12_FILL_MODE_WIREFRAME;
	d3dRasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	d3dRasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	d3dRasterizerDesc.FrontCounterClockwise = FALSE;
	d3dRasterizerDesc.DepthBias = 0;
	d3dRasterizerDesc.DepthBiasClamp = 0.0f;
	d3dRasterizerDesc.SlopeScaledDepthBias = 0.0f;
	d3dRasterizerDesc.DepthClipEnable = TRUE;
	d3dRasterizerDesc.MultisampleEnable = FALSE;
	d3dRasterizerDesc.AntialiasedLineEnable = FALSE;
	d3dRasterizerDesc.ForcedSampleCount = 0;
	d3dRasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	return(d3dRasterizerDesc);
}

D3D12_DEPTH_STENCIL_DESC CShader::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = TRUE;
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

D3D12_BLEND_DESC CShader::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
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

	return(d3dBlendDesc);
}

void CShader::CreateShader(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature)
{
	::ZeroMemory(&m_d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	m_d3dPipelineStateDesc.pRootSignature = pd3dGraphicsRootSignature;
	m_d3dPipelineStateDesc.VS = CreateVertexShader();
	m_d3dPipelineStateDesc.PS = CreatePixelShader();
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

	HRESULT hResult = pd3dDevice->CreateGraphicsPipelineState(&m_d3dPipelineStateDesc, __uuidof(ID3D12PipelineState), (void **)&m_pd3dPipelineState);

	if (m_pd3dVertexShaderBlob) m_pd3dVertexShaderBlob->Release();
	if (m_pd3dPixelShaderBlob) m_pd3dPixelShaderBlob->Release();

	if (m_d3dPipelineStateDesc.InputLayout.pInputElementDescs) delete[] m_d3dPipelineStateDesc.InputLayout.pInputElementDescs;
}

void CShader::OnPrepareRender(ID3D12GraphicsCommandList *pd3dCommandList, int nPipelineState)
{
	if (m_pd3dPipelineState) pd3dCommandList->SetPipelineState(m_pd3dPipelineState);

	UpdateShaderVariables(pd3dCommandList);
}

void CShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	OnPrepareRender(pd3dCommandList);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CTerrainShader::CTerrainShader()
{
}

CTerrainShader::~CTerrainShader()
{
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

D3D12_SHADER_BYTECODE CTerrainShader::CreateVertexShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSTerrain", "vs_5_1", &m_pd3dVertexShaderBlob));
}

D3D12_SHADER_BYTECODE CTerrainShader::CreatePixelShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSTerrain", "ps_5_1", &m_pd3dPixelShaderBlob));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
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
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSSkyBox", "vs_5_1", &m_pd3dVertexShaderBlob));
}

D3D12_SHADER_BYTECODE CSkyBoxShader::CreatePixelShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSSkyBox", "ps_5_1", &m_pd3dPixelShaderBlob));
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CIceShader::CIceShader()
{

}

CIceShader::~CIceShader()
{

}

void CIceShader::SetExplode(bool bBlowing)
{
	if (m_pIceParticleObjects) {
	
		if (bBlowing) {
			m_pIceParticleObjects->SetExplode(bBlowing);
		}
	}
	
}

D3D12_INPUT_LAYOUT_DESC CIceShader::CreateInputLayout()
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


D3D12_SHADER_BYTECODE CIceShader::CreateVertexShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSIceCube", "vs_5_1", &m_pd3dVertexShaderBlob));
}

D3D12_SHADER_BYTECODE CIceShader::CreatePixelShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSIceCube", "ps_5_1", &m_pd3dPixelShaderBlob));
}


void CIceShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature,void *pContext)
{

	CTexture *pParticleTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	pParticleTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Model/Textures/texture2.dds", 0);

	CScene::CreateShaderResourceViews(pd3dDevice, pParticleTexture, 19, false);

	m_pMaterial = new CMaterial(1);
	m_pMaterial->SetTexture(pParticleTexture);

	

	//CCubeMeshDiffused *pExplosionMesh = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, 0.5f, 0.5f, 0.5f);
	CCubeMeshTextured *pExplosionMesh = new CCubeMeshTextured(pd3dDevice, pd3dCommandList, 10.0f, 10.0f, 10.0f);


	m_pIceParticleObjects = new CIceCubeObject(1);
	m_pIceParticleObjects->SetMesh(pExplosionMesh);
	m_pIceParticleObjects->SetMaterial(0, m_pMaterial);
	m_pIceParticleObjects->SetPosition(200.0f, 300.0f, 300.0f);

	m_pIceParticleObjects->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 0.0f));
	m_pIceParticleObjects->SetRotationSpeed(90.f);
	m_pIceParticleObjects->SetMovingSpeed(1.0f);
		//SetMesh(pExplosionMesh);


	CIceCubeObject::PrepareExplosion(pd3dDevice, pd3dCommandList);
}

void CIceShader::AnimateObjects(float fTimeElapsed, CCamera *pCamera, CPlayer* pPlayer)
{
	
	m_pIceParticleObjects->Animate(fTimeElapsed, pCamera);
	
}

void CIceShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CShader::Render(pd3dCommandList, pCamera);


	if (m_pIceParticleObjects)
	{
		m_pIceParticleObjects->Render(pd3dCommandList, pCamera);
	}
	
}



void CIceShader::ReleaseObjects()
{

	if (m_pMaterial)
	{
		delete m_pMaterial;
	}
}
/////////////////////////////////////////////////////////////////////////

CBillboardShader::CBillboardShader()
{

}

CBillboardShader::~CBillboardShader()
{

}

D3D12_INPUT_LAYOUT_DESC CBillboardShader::CreateInputLayout()
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

D3D12_SHADER_BYTECODE CBillboardShader::CreateVertexShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSParticle", "vs_5_1", &m_pd3dVertexShaderBlob));
}

D3D12_SHADER_BYTECODE CBillboardShader::CreatePixelShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSParticle", "ps_5_1", &m_pd3dVertexShaderBlob));
}

D3D12_BLEND_DESC CBillboardShader::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = TRUE;
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

	return(d3dBlendDesc);
}


void CBillboardShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CShader::Render(pd3dCommandList, pCamera);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//


CSnowBillboardShader::CSnowBillboardShader()
	
{
	
}

CSnowBillboardShader::~CSnowBillboardShader()
{

}

D3D12_SHADER_BYTECODE CSnowBillboardShader::CreateVertexShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSSnowBillboard", "vs_5_1", &m_pd3dVertexShaderBlob));
}

D3D12_SHADER_BYTECODE CSnowBillboardShader::CreatePixelShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSSnowBillboard", "ps_5_1", &m_pd3dVertexShaderBlob));
}

void CSnowBillboardShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, void *pContext)
{
	//SnowBillboard는 지형의 높이정보를 알고있어야함. 그래야 지형의 밑으로 내려갔을때 다시 하늘에서 떨어지게 할 수 있다.
	m_pTerrain = (CHeightMapTerrain*)pContext;

	
	CTexture *pSnowTexture = new CTexture(1,RESOURCE_TEXTURE2D,0);
	pSnowTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Model/Textures/SnowFlake.dds", 0);

	CScene::CreateShaderResourceViews(pd3dDevice, pSnowTexture, 17, false);


	m_pMaterial = new CMaterial(1);
	m_pMaterial->SetTexture(pSnowTexture);


	CTexturedRectMesh* pSnowBillboardMesh = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f);
	
	
	//가상의 박스 -> 렌더링 하지는 않음
	CCubeObject *pCubeObject = new CCubeObject(50.0f, 50.0f, 50.0f);

	m_refCubeObject = pCubeObject->GetCubeObject();

	m_nObjects = 100;
	m_ppObjects = new CBillboardObject*[m_nObjects];
	XMFLOAT3 xmf3RoatationAxis = XMFLOAT3(0.0f,0.0,1.0f);

	int minX = -m_refCubeObject.GetWidth();
	int maxX =  m_refCubeObject.GetWidth();
	int minZ = -m_refCubeObject.GetDepth();
	int maxZ =  m_refCubeObject.GetDepth();

	for (UINT i = 0; i < m_nObjects; i++)
	{
		m_ppObjects[i] = new CSnowBillboardObject();
		m_ppObjects[i]->SetMesh(pSnowBillboardMesh);
		m_ppObjects[i]->SetMaterial(0, m_pMaterial);
		m_ppObjects[i]->SetOffsetX(getRandomNumber(minX, maxX));
		m_ppObjects[i]->SetOffsetZ(getRandomNumber(minZ, maxZ));
		m_ppObjects[i]->SetPosition(0.0f,getRandomNumber(300.0f,400.0f),0.0f);
		//m_ppObjects[i]->Rotate(&xmf3RoatationAxis, Random(0.0f, 89.0f));	
	}
}

void CSnowBillboardShader::AnimateObjects(float fTimeElapsed,CCamera *pCamera,CPlayer *pPlayer)
{
	m_refCubeObject.UpdatePosition(pPlayer->GetPosition(),m_fTimeLagScale);

	
	if (m_ppObjects)
	{
		for (int i = 0; i < m_nObjects; i++)
		{
			if (m_ppObjects[i])
			{
				//
				m_ppObjects[i]->SetPositionXZ(m_refCubeObject.GetPosition().x + m_ppObjects[i]->GetOffsetX(),
					m_refCubeObject.GetPosition().z + m_ppObjects[i]->GetOffsetZ());
				m_ppObjects[i]->Animate(fTimeElapsed, pCamera);

				// 눈이 지형 아래로 내려가면 다시 위에서 생성되서 다시 내려오게 함
				if (m_ppObjects[i]->GetPosition().y < m_pTerrain->GetHeight(m_ppObjects[i]->GetPosition().x, m_ppObjects[i]->GetPosition().z, false))
				{
					m_ppObjects[i]->SetPosition(m_refCubeObject.GetPosition().x + m_ppObjects[i]->GetOffsetX(), getRandomNumber(300.0f, 400.0f),
						m_refCubeObject.GetPosition().z + m_ppObjects[i]->GetOffsetZ());
				}
				
			}
		}
	}
}


void CSnowBillboardShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	OnPrepareRender(pd3dCommandList);

	if (m_ppObjects)
	{
		for (int i = 0; i < m_nObjects; i++)
		{
			if (m_ppObjects[i])
			{
				m_ppObjects[i]->Render(pd3dCommandList, pCamera);

			}
		}
	}
}
void CSnowBillboardShader::ReleaseObjects()
{

	if (m_pMaterial)
	{
		delete m_pMaterial;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CGrassBillboardShader::CGrassBillboardShader() {

}
CGrassBillboardShader::~CGrassBillboardShader(){}

D3D12_SHADER_BYTECODE CGrassBillboardShader::CreateVertexShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSGrassBillboard", "vs_5_1", &m_pd3dVertexShaderBlob));
}

D3D12_SHADER_BYTECODE CGrassBillboardShader::CreatePixelShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSGrassBillboard", "ps_5_1", &m_pd3dVertexShaderBlob));
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CStandardShader::CStandardShader()
{
}

CStandardShader::~CStandardShader()
{
}

D3D12_INPUT_LAYOUT_DESC CStandardShader::CreateInputLayout()
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

D3D12_SHADER_BYTECODE CStandardShader::CreateVertexShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSStandard", "vs_5_1", &m_pd3dVertexShaderBlob));
}

D3D12_SHADER_BYTECODE CStandardShader::CreatePixelShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSStandard", "ps_5_1", &m_pd3dPixelShaderBlob));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CSkinnedAnimationShader::CSkinnedAnimationShader()
{
}

CSkinnedAnimationShader::~CSkinnedAnimationShader()
{
}

D3D12_INPUT_LAYOUT_DESC CSkinnedAnimationShader::CreateInputLayout()
{
	UINT nInputElementDescs = 7;
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[2] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 2, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[3] = { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 3, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[4] = { "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 4, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[5] = { "BONEINDEX", 0, DXGI_FORMAT_R32G32B32A32_UINT, 5, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[6] = { "BONEWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 6, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE CSkinnedAnimationShader::CreateVertexShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSSkinnedAnimationStandard", "vs_5_1", &m_pd3dVertexShaderBlob));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CStandardObjectsShader::CStandardObjectsShader()
{
}

CStandardObjectsShader::~CStandardObjectsShader()
{
}

void CStandardObjectsShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, void *pContext)
{
}

void CStandardObjectsShader::ReleaseObjects()
{
	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++)
		{
			if (m_ppObjects[j]) m_ppObjects[j]->Release();
		}
		delete[] m_ppObjects;
	}
}

void CStandardObjectsShader::AnimateObjects(float fTimeElapsed)
{
	m_fElapsedTime = fTimeElapsed;
}

void CStandardObjectsShader::ReleaseUploadBuffers()
{
	for (int j = 0; j < m_nObjects; j++) if (m_ppObjects[j]) m_ppObjects[j]->ReleaseUploadBuffers();
}

void CStandardObjectsShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CStandardShader::Render(pd3dCommandList, pCamera);

	for (int j = 0; j < m_nObjects; j++)
	{
		if (m_ppObjects[j])
		{
			m_ppObjects[j]->Animate(m_fElapsedTime);
			m_ppObjects[j]->UpdateTransform(NULL);
			m_ppObjects[j]->Render(pd3dCommandList, pCamera);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CHellicopterObjectsShader::CHellicopterObjectsShader()
{
}

CHellicopterObjectsShader::~CHellicopterObjectsShader()
{
}

float Random(float fMin, float fMax)
{
	float fRandomValue = (float)rand();
	if (fRandomValue < fMin) fRandomValue = fMin;
	if (fRandomValue > fMax) fRandomValue = fMax;
	return(fRandomValue);
}

float Random()
{
	return(rand() / float(RAND_MAX));
}

XMFLOAT3 RandomPositionInSphere(XMFLOAT3 xmf3Center, float fRadius, int nColumn, int nColumnSpace)
{
    float fAngle = Random() * 360.0f * (2.0f * 3.14159f / 360.0f);

	XMFLOAT3 xmf3Position;
    xmf3Position.x = xmf3Center.x + fRadius * sin(fAngle);
    xmf3Position.y = xmf3Center.y - (nColumn * float(nColumnSpace) / 2.0f) + (nColumn * nColumnSpace) + Random();
    xmf3Position.z = xmf3Center.z + fRadius * cos(fAngle);

	return(xmf3Position);
}

void CHellicopterObjectsShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, void *pContext)
{
	m_nObjects = 40;
	m_ppObjects = new CGameObject*[m_nObjects];

	CLoadedModelInfo *pSuperCobraModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/SuperCobra.bin", this, false);
	CLoadedModelInfo *pGunshipModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Gunship.bin", this, false);

	int nColumnSpace = 5, nColumnSize = 30;           
    int nFirstPassColumnSize = (m_nObjects % nColumnSize) > 0 ? (nColumnSize - 1) : nColumnSize;

	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)pContext;

	int nObjects = 0;
    for (int h = 0; h < nFirstPassColumnSize; h++)
    {
        for (int i = 0; i < floor(float(m_nObjects) / float(nColumnSize)); i++)
        {
			if (nObjects % 2)
			{
				m_ppObjects[nObjects] = new CSuperCobraObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
				m_ppObjects[nObjects]->SetChild(pSuperCobraModel->m_pModelRootObject);
				pSuperCobraModel->m_pModelRootObject->AddRef();
			}
			else
			{
				m_ppObjects[nObjects] = new CGunshipObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
				m_ppObjects[nObjects]->SetChild(pGunshipModel->m_pModelRootObject);
				pGunshipModel->m_pModelRootObject->AddRef();
			}
			float fHeight = pTerrain->GetHeight(390.0f, 670.0f);
			XMFLOAT3 xmf3Position = RandomPositionInSphere(XMFLOAT3(390.0f, fHeight + 35.0f, 670.0f), Random(20.0f, 100.0f), h - int(floor(nColumnSize / 2.0f)), nColumnSpace);
			xmf3Position.y = pTerrain->GetHeight(xmf3Position.x, xmf3Position.z) + Random(0.0f, 25.0f);
			m_ppObjects[nObjects]->SetPosition(xmf3Position);
			m_ppObjects[nObjects]->Rotate(0.0f, 90.0f, 0.0f);
			m_ppObjects[nObjects++]->OnPrepareAnimate();
		}
    }

    if (nFirstPassColumnSize != nColumnSize)
    {
        for (int i = 0; i < m_nObjects - int(floor(float(m_nObjects) / float(nColumnSize)) * nFirstPassColumnSize); i++)
        {
			if (nObjects % 2)
			{
				m_ppObjects[nObjects] = new CSuperCobraObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
				m_ppObjects[nObjects]->SetChild(pSuperCobraModel->m_pModelRootObject);
				pSuperCobraModel->m_pModelRootObject->AddRef();
			}
			else
			{
				m_ppObjects[nObjects] = new CGunshipObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
				m_ppObjects[nObjects]->SetChild(pGunshipModel->m_pModelRootObject);
				pGunshipModel->m_pModelRootObject->AddRef();
			}
			m_ppObjects[nObjects]->SetPosition(RandomPositionInSphere(XMFLOAT3(0.0f, 0.0f, 0.0f), Random(20.0f, 100.0f), nColumnSize - int(floor(nColumnSize / 2.0f)), nColumnSpace));
			m_ppObjects[nObjects]->Rotate(0.0f, 90.0f, 0.0f);
			m_ppObjects[nObjects++]->OnPrepareAnimate();
        }
    }

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	if (pSuperCobraModel) delete pSuperCobraModel;
	if (pGunshipModel) delete pGunshipModel;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CPineTreesObjectsShader::CPineTreesObjectsShader()
{

}

CPineTreesObjectsShader::~CPineTreesObjectsShader()
{

}

void CPineTreesObjectsShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, void *pContext)
{
	m_nObjects = 19;
	m_ppObjects = new CGameObject*[m_nObjects];


	CLoadedModelInfo* pDeadTreesModel01 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/SM_DeadTrunk_01.bin", this, false);
	CLoadedModelInfo* pDeadTreesModel02 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/SM_DeadTrunk_02.bin", this, false);
	CLoadedModelInfo* pDeadTreesModel03 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/SM_DeadTrunk_03.bin", this, false);
	CLoadedModelInfo* pDeadTreesModel04 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/SM_DeadTrunk_04.bin", this, false);
	CLoadedModelInfo* pDeadTreesModel06 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/SM_DeadTrunk_06.bin", this, false);


	CLoadedModelInfo* pPineTreesModel01 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/SM_PineTree_Snow_01.bin", this, false);


	CLoadedModelInfo* pPineTreesModel02 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/SM_PineTree_Snow_02.bin", this, false);
	CLoadedModelInfo* pPineTreesModel03 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/SM_PineTree_Snow_03.bin", this, false);
	CLoadedModelInfo* pPineTreesModel04 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/SM_PineTree_Snow_04.bin", this, false);
	CLoadedModelInfo* pPineTreesModel05 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/SM_PineTree_Snow_05.bin", this, false);
	CLoadedModelInfo* pPineTreesModel06 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/SM_PineTree_Snow_06.bin", this, false);
	CLoadedModelInfo* pPineTreesModel07 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/SM_PineTree_Snow_07.bin", this, false);
	CLoadedModelInfo* pPineTreesModel08 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/SM_PineTree_Snow_08.bin", this, false);


	CLoadedModelInfo* pRocks01 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/SM_BigPlainRock_Snow_01.bin", this, false);
	CLoadedModelInfo* pRocks02 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/SM_BigPlainRock_Snow_02.bin", this, false);
	CLoadedModelInfo* pRocks03 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/SM_BigPlainRock_Snow_03.bin", this, false);
	CLoadedModelInfo* pRocks04 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/SM_BigPlainRock_Snow_04.bin", this, false);
	CLoadedModelInfo* pRocks05 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/SM_PlainSmall_Snow_01.bin", this, false);



	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)pContext;


	m_ppObjects[0] = new CDeadTreeObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppObjects[0]->SetChild(pDeadTreesModel06->m_pModelRootObject);
	pDeadTreesModel06->m_pModelRootObject->AddRef();

	m_ppObjects[0]->SetPosition(200.0f, pTerrain->GetHeight(200.0f, 250.0f), 250.0f);

	m_ppObjects[1] = new CDeadTreeObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppObjects[1]->SetChild(pDeadTreesModel01->m_pModelRootObject);
	pDeadTreesModel01->m_pModelRootObject->AddRef();

	m_ppObjects[1]->SetPosition(100.0f, pTerrain->GetHeight(100.0f, 150.0f), 150.0f);

	m_ppObjects[2] = new CDeadTreeObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppObjects[2]->SetChild(pDeadTreesModel02->m_pModelRootObject);
	pDeadTreesModel02->m_pModelRootObject->AddRef();

	m_ppObjects[2]->SetPosition(200.0f, pTerrain->GetHeight(200.0f, 100.0f), 100.0f);

	m_ppObjects[3] = new CDeadTreeObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppObjects[3]->SetChild(pDeadTreesModel03->m_pModelRootObject);
	pDeadTreesModel03->m_pModelRootObject->AddRef();

	m_ppObjects[3]->SetPosition(220.0f, pTerrain->GetHeight(220.0f, 100.0f), 100.0f);

	m_ppObjects[4] = new CDeadTreeObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppObjects[4]->SetChild(pDeadTreesModel04->m_pModelRootObject);
	pDeadTreesModel04->m_pModelRootObject->AddRef();

	m_ppObjects[4]->SetPosition(310.0f, pTerrain->GetHeight(310.0f, 350.0f), 350.0f);

	m_ppObjects[5] = new CPineTreeObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppObjects[5]->SetChild(pPineTreesModel01->m_pModelRootObject);
	pPineTreesModel01->m_pModelRootObject->AddRef();

	m_ppObjects[5]->SetPosition(390.0f, pTerrain->GetHeight(390.0f, 350.0f), 350.0f);

	m_ppObjects[6] = new CPineTreeObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppObjects[6]->SetChild(pPineTreesModel02->m_pModelRootObject);
	pPineTreesModel02->m_pModelRootObject->AddRef();

	m_ppObjects[6]->SetPosition(420.0f, pTerrain->GetHeight(420.0f, 350.0f), 350.0f);


	m_ppObjects[7] = new CPineTreeObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppObjects[7]->SetChild(pPineTreesModel03->m_pModelRootObject);
	pPineTreesModel03->m_pModelRootObject->AddRef();

	m_ppObjects[7]->SetPosition(450.0f, pTerrain->GetHeight(450.0f, 400.0f), 400.0f);

	m_ppObjects[8] = new CPineTreeObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppObjects[8]->SetChild(pPineTreesModel04->m_pModelRootObject);
	pPineTreesModel04->m_pModelRootObject->AddRef();

	m_ppObjects[8]->SetPosition(390.0f, pTerrain->GetHeight(390.0f, 450.0f), 450.0f);

	m_ppObjects[9] = new CPineTreeObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppObjects[9]->SetChild(pPineTreesModel05->m_pModelRootObject);
	pPineTreesModel05->m_pModelRootObject->AddRef();

	m_ppObjects[9]->SetPosition(380.0f, pTerrain->GetHeight(380.0f, 500.0f), 500.0f);

	m_ppObjects[10] = new CPineTreeObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppObjects[10]->SetChild(pPineTreesModel06->m_pModelRootObject);
	pPineTreesModel06->m_pModelRootObject->AddRef();

	m_ppObjects[10]->SetPosition(320.0f, pTerrain->GetHeight(320.0f, 500.0f), 500.0f);


	m_ppObjects[11] = new CPineTreeObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppObjects[11]->SetChild(pPineTreesModel06->m_pModelRootObject);
	pPineTreesModel06->m_pModelRootObject->AddRef();

	m_ppObjects[11]->SetPosition(290.0f, pTerrain->GetHeight(290.0f, 500.0f), 500.0f);

	m_ppObjects[12] = new CPineTreeObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppObjects[12]->SetChild(pPineTreesModel07->m_pModelRootObject);
	pPineTreesModel07->m_pModelRootObject->AddRef();

	m_ppObjects[12]->SetPosition(250.0f, pTerrain->GetHeight(250.0f, 520.0f), 520.0f);


	m_ppObjects[13] = new CPineTreeObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppObjects[13]->SetChild(pPineTreesModel08->m_pModelRootObject);
	pPineTreesModel08->m_pModelRootObject->AddRef();

	m_ppObjects[13]->SetPosition(270.0f, pTerrain->GetHeight(270.0f, 600.0f), 600.0f);


	m_ppObjects[14] = new CPineTreeObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppObjects[14]->SetChild(pRocks01->m_pModelRootObject);
	pRocks01->m_pModelRootObject->AddRef();

	m_ppObjects[14]->SetPosition(300.0f, pTerrain->GetHeight(300.0f, 600.0f), 600.0f);


	m_ppObjects[15] = new CPineTreeObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppObjects[15]->SetChild(pRocks02->m_pModelRootObject);
	pRocks02->m_pModelRootObject->AddRef();

	m_ppObjects[15]->SetPosition(350.0f, pTerrain->GetHeight(350.0f, 620.0f), 620.0f);

	m_ppObjects[16] = new CPineTreeObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppObjects[16]->SetChild(pRocks03->m_pModelRootObject);
	pRocks03->m_pModelRootObject->AddRef();

	m_ppObjects[16]->SetPosition(350.0f, pTerrain->GetHeight(350.0f, 700.0f), 700.0f);

	m_ppObjects[17] = new CPineTreeObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppObjects[17]->SetChild(pRocks04->m_pModelRootObject);
	pRocks04->m_pModelRootObject->AddRef();

	m_ppObjects[17]->SetPosition(400.0f, pTerrain->GetHeight(400.0f, 700.0f), 700.0f);

	m_ppObjects[18] = new CPineTreeObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppObjects[18]->SetChild(pRocks05->m_pModelRootObject);
	pRocks05->m_pModelRootObject->AddRef();

	m_ppObjects[18]->SetPosition(430.0f, pTerrain->GetHeight(430.0f, 760.0f), 760.0f);



	CreateShaderVariables(pd3dDevice, pd3dCommandList);



	if (pDeadTreesModel01)
	{
		delete pDeadTreesModel01;
	}
	if (pDeadTreesModel02)
	{
		delete pDeadTreesModel02;
	}
	if (pDeadTreesModel03)
	{
		delete pDeadTreesModel03;
	}
	if (pDeadTreesModel04)
	{
		delete pDeadTreesModel04;
	}
	if (pDeadTreesModel06)
	{
		delete pDeadTreesModel06;
	}

	if (pPineTreesModel01)
	{
		delete pPineTreesModel01;
	}
	if (pPineTreesModel02)
	{
		delete pPineTreesModel02;
	}

	if (pPineTreesModel03)
	{
		delete pPineTreesModel03;
	}

	if (pPineTreesModel04)
	{
		delete pPineTreesModel04;
	}

	if (pPineTreesModel05)
	{
		delete pPineTreesModel05;
	}

	if (pPineTreesModel06)
	{
		delete pPineTreesModel06;
	}

	if (pPineTreesModel07)
	{
		delete pPineTreesModel07;
	}
	if (pPineTreesModel08)
	{
		delete pPineTreesModel08;
	}

	if (pRocks01)
	{
		delete pRocks01;
	}

	if (pRocks02)
	{
		delete pRocks02;
	}

	if (pRocks03)
	{
		delete pRocks03;
	}
	if (pRocks04)
	{
		delete pRocks04;
	}
	if (pRocks05)
	{
		delete pRocks05;
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////
CDeerObjectsShader::CDeerObjectsShader()
{

}

CDeerObjectsShader::~CDeerObjectsShader()
{

}

void CDeerObjectsShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, void *pContext)
{
	m_nObjects = 1;
	m_ppObjects = new CGameObject*[m_nObjects];

	CLoadedModelInfo* pDeerObjectModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/SM_Deer.bin", this, false);
	

	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)pContext;
	m_ppObjects[0] = new CDeerObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);

	m_ppObjects[0]->SetChild(pDeerObjectModel->m_pModelRootObject);
	m_ppObjects[0]->SetPosition(120.0f, pTerrain->GetHeight(120.0f, 620.0f), 620.0f);





	if (pDeerObjectModel)
	{
		delete pDeerObjectModel;
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
CFoliageShader::CFoliageShader()
{

}

CFoliageShader::~CFoliageShader()
{

}

D3D12_SHADER_BYTECODE CFoliageShader::CreatePixelShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSGrssStandard", "ps_5_1", &m_pd3dPixelShaderBlob));
}

void CFoliageShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, void *pContext)
{
	

	
	CLoadedModelInfo* pFoliageModel01 = CFoliageObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Grass_C_01.bin", this, false);
	CLoadedModelInfo* pFoliageModel02 = CFoliageObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Grass_D_01.bin", this, false);
	CLoadedModelInfo* pFoliageModel03 = CFoliageObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Plant_c_01.bin", this, false);


	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)pContext;


	float fxPitch = 3.0f;
	float fzPitch = 3.0f;

	float fTerrainWidth = pTerrain->GetWidth();
	float fTerrainLength = pTerrain->GetLength();

	int xObjects = int(fTerrainWidth* 0.05f /fxPitch);
	int zObjects = int(fTerrainLength * 0.25f / fzPitch);

	m_nObjects = xObjects * zObjects;
	m_ppObjects = new CGameObject*[m_nObjects];

	int xStart = 0.0f;
	int zStart = 0.0f;

	for (int z = zStart,i=0; z < zStart+zObjects; z++)
	{
		for (int x = xStart ; x < xStart+xObjects; x++)
		{
			m_ppObjects[i] = new CFoliageObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	
			float xPosition = x * fxPitch;
			float zPosition = z * fzPitch;

			float fHeight = pTerrain->GetHeight(xPosition, zPosition);
			
			m_ppObjects[i]->SetPosition(xPosition, fHeight,zPosition);

			if (i % 3 == 0) {
				m_ppObjects[i++]->SetChild(pFoliageModel01->m_pModelRootObject, true);
			}
			else if(i %3 ==1){
				m_ppObjects[i++]->SetChild(pFoliageModel02->m_pModelRootObject, true);
			}
			else
			{
				m_ppObjects[i++]->SetChild(pFoliageModel03->m_pModelRootObject, true);
			}

		}
	}
	
	if (pFoliageModel01)
	{
		delete pFoliageModel01;
	}
	if (pFoliageModel02)
	{
		delete pFoliageModel02;
	}
	if (pFoliageModel03)
	{
		delete pFoliageModel03;
	}
}

float CFoliageShader::GetDistanceToCamera(CGameObject* pObject,CCamera *pCamera)
{
	float dist = 0.0;

	XMFLOAT3 cameraPosition = pCamera->GetPosition();
	XMFLOAT3 objectPosition = pObject->GetPosition();


	dist =Vector3::Length(Vector3::Subtract(cameraPosition, objectPosition));

	return dist;
}
void CFoliageShader::AnimateObjects(float fTimeElapsed, CCamera* pCamera, CPlayer *pPlayer)
{
	float distance = 0.0f;
	UINT lodlevel = CGameObject::LOD_LEVEL0;
	for (int i = 0; i < m_nObjects; i++)
	{
		distance = GetDistanceToCamera(m_ppObjects[i],pCamera);
	
		
		//lodlevel= (distance < 20.0f) ? CGameObject::LOD_LEVEL0 : CGameObject::LOD_LEVEL1;

		if (distance < 50.0f)
		{
			lodlevel = CGameObject::LOD_LEVEL0;
		}
		else if (distance >= 50.0f && distance < 200.0f)
		{
			lodlevel = CGameObject::LOD_LEVEL1;
		}
		else if( distance >200 && distance < 300.0f)
		{
			lodlevel = CGameObject::LOD_LEVEL2;
		}
		else
		{
			lodlevel = CGameObject::LOD_BILLBOARD;
		}

		//여기서 자신의 LOD레벨을 구하면 될듯하다.
		m_ppObjects[i]->SetLODlevel(lodlevel);
	}
}

D3D12_BLEND_DESC CFoliageShader::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = TRUE;
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

	return(d3dBlendDesc);
}

void CFoliageShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	
	CStandardShader::Render(pd3dCommandList, pCamera);
	for (int j = 0; j < m_nObjects; j++)
	{

		if (m_ppObjects[j])
		{
			m_ppObjects[j]->Animate(m_fElapsedTime);
			m_ppObjects[j]->UpdateTransform(NULL);
			m_ppObjects[j]->Render(pd3dCommandList, m_ppObjects[j]->GetLodLevel(), pCamera);

		}
			
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CFenceShader::CFenceShader()
{

}

CFenceShader::~CFenceShader()
{

}

void CFenceShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, void *pContext)
{
	CLoadedModelInfo *pFenceModel01 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/LowPoly_Fence_B.bin", this, false);
	CLoadedModelInfo *pFenceModel02 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/LowPoly_Fence_A.bin", this, false);


	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)pContext;


	
	m_nObjects = 20;

	m_ppObjects = new CGameObject*[m_nObjects];


	for (UINT i = 0; i < m_nObjects; i++)
	{
		m_ppObjects[i] = new CFenceObject();
		m_ppObjects[i]->SetPosition(0.0f, pTerrain->GetHeight(0.0f,0.0f+(i*40)), 0.0f+(i*40));
		m_ppObjects[i]->Rotate(0.0f, -90.0f, 0.0f);
		if (i % 2 == 0) {
			m_ppObjects[i]->SetChild(pFenceModel01->m_pModelRootObject, true);
		}
		else {
			m_ppObjects[i]->SetChild(pFenceModel02->m_pModelRootObject, true);
		}

	}
	

	if (pFenceModel01)
	{
		delete pFenceModel01;
	}

	if (pFenceModel02)
	{
		delete pFenceModel02;
	}
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CSkinnedAnimationObjectsShader::CSkinnedAnimationObjectsShader()
{
}

CSkinnedAnimationObjectsShader::~CSkinnedAnimationObjectsShader()
{
}

void CSkinnedAnimationObjectsShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, void *pContext)
{
}

void CSkinnedAnimationObjectsShader::ReleaseObjects()
{
	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++)
		{
			if (m_ppObjects[j]) m_ppObjects[j]->Release();
		}
		delete[] m_ppObjects;
	}
}

void CSkinnedAnimationObjectsShader::AnimateObjects(float fTimeElapsed)
{
	m_fElapsedTime = fTimeElapsed;
}

void CSkinnedAnimationObjectsShader::ReleaseUploadBuffers()
{
	for (int j = 0; j < m_nObjects; j++) if (m_ppObjects[j]) m_ppObjects[j]->ReleaseUploadBuffers();
}

void CSkinnedAnimationObjectsShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CSkinnedAnimationShader::Render(pd3dCommandList, pCamera);

	for (int j = 0; j < m_nObjects; j++)
	{
		if (m_ppObjects[j])
		{
			m_ppObjects[j]->Animate(m_fElapsedTime);
			m_ppObjects[j]->UpdateTransform(NULL);
			m_ppObjects[j]->Render(pd3dCommandList, pCamera);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
//CAngrybotObjectsShader::CAngrybotObjectsShader()
//{
//}
//
//CAngrybotObjectsShader::~CAngrybotObjectsShader()
//{
//}
//
//void CAngrybotObjectsShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, void *pContext)
//{
//	int xObjects = 7, zObjects = 7, i = 0;
//
//	m_nObjects = (xObjects * 2 + 1) * (zObjects * 2 + 1);
//
//	m_ppObjects = new CGameObject*[m_nObjects];
//
//	float fxPitch = 7.0f * 2.5f;
//	float fzPitch = 7.0f * 2.5f;
//
//	CLoadedModelInfo *pAngrybotModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Player.bin", NULL, true);
//
//	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)pContext;
//
//	int nObjects = 0;
//	for (int x = -xObjects; x <= xObjects; x++)
//	{
//		for (int z = -zObjects; z <= zObjects; z++)
//		{
//			m_ppObjects[nObjects] = new CAngrybotObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
//			m_ppObjects[nObjects]->SetChild(pAngrybotModel->m_pModelRootObject, true);
//			m_ppObjects[nObjects]->m_pAnimationController = new CAnimationController(1, pAngrybotModel->m_pAnimationSets);
//			m_ppObjects[nObjects]->m_pAnimationController->SetTrackAnimationSet(0, (nObjects % 2));
//			m_ppObjects[nObjects]->m_pAnimationController->SetTrackSpeed(0, (nObjects % 2) ? 0.25f : 1.0f);
//			m_ppObjects[nObjects]->m_pAnimationController->SetTrackPosition(0, (nObjects % 3) ? 0.85f : 0.0f);
//			m_ppObjects[nObjects]->m_pSkinningBoneTransforms = new CSkinningBoneTransforms(pd3dDevice, pd3dCommandList, pAngrybotModel);
//			XMFLOAT3 xmf3Position = XMFLOAT3(fxPitch*x + 390.0f, 0.0f, 730.0f + fzPitch * z);
//			xmf3Position.y = pTerrain->GetHeight(xmf3Position.x, xmf3Position.z);
//			m_ppObjects[nObjects]->SetPosition(xmf3Position);
//			m_ppObjects[nObjects++]->SetScale(2.0f, 2.0f, 2.0f);
//		}
//    }
//
//	CreateShaderVariables(pd3dDevice, pd3dCommandList);
//
//	if (pAngrybotModel) delete pAngrybotModel;
//}
