#include "../../../../Stdafx/Stdafx.h"
#include "ProgressBarUIShader.h"
#include "../../../../GameObject/Billboard/UI/UI.h"
#include "../../../../Mesh/BillboardMesh/BillboardMesh.h"
#include "../../../../Texture/Texture.h"
#include "../../../../Material/Material.h"
#include "../../../../Scene/Scene.h"

extern volatile size_t g_TotalSize;
extern volatile size_t g_FileSize;

CProgressBarUIShader::CProgressBarUIShader()
{
}

CProgressBarUIShader::~CProgressBarUIShader()
{
}

void CProgressBarUIShader::CreateShader(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];

	for (int i = 0; i < m_nPipelineStates; ++i)
	{
		::ZeroMemory(&m_d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
		m_d3dPipelineStateDesc.pRootSignature = pd3dGraphicsRootSignature;
		m_d3dPipelineStateDesc.VS = CreateVertexShader(i);
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

		HRESULT hResult = pd3dDevice->CreateGraphicsPipelineState(&m_d3dPipelineStateDesc, __uuidof(ID3D12PipelineState), (void **)&m_ppd3dPipelineStates[i]);
	}

	if (m_pd3dVertexShaderBlob) m_pd3dVertexShaderBlob->Release();
	if (m_pd3dPixelShaderBlob) m_pd3dPixelShaderBlob->Release();

	if (m_d3dPipelineStateDesc.InputLayout.pInputElementDescs) delete[] m_d3dPipelineStateDesc.InputLayout.pInputElementDescs;
}

D3D12_SHADER_BYTECODE CProgressBarUIShader::CreateVertexShader(int UIType)
{
	switch (UIType)
	{
	case ProgressBar:
		return(CShader::CompileShaderFromFile(L"../Code/Shader/HLSL/UI.hlsl", "VSProgressBarUI", "vs_5_1", &m_pd3dVertexShaderBlob));
		break;
	}
}

D3D12_SHADER_BYTECODE CProgressBarUIShader::CreatePixelShader()
{
	return(CShader::CompileShaderFromFile(L"../Code/Shader/HLSL/UI.hlsl", "PSLoadingScene", "ps_5_1", &m_pd3dVertexShaderBlob));
}

void CProgressBarUIShader::CreateCbvSrvDescriptorHeaps(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, int nConstantBufferViews, int nShaderResourceViews)
{
	//리소스를 사용하려면 뷰를 만들어야한다. 
	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
	d3dDescriptorHeapDesc.NumDescriptors = nConstantBufferViews + nShaderResourceViews; //CBVs + SRVs 
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	d3dDescriptorHeapDesc.NodeMask = 0;

	//서술자 힙: 리소스를 서술하는 서술자들을 저장하는 연속적인 메모리 영역(배열)
	pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void **)&m_pd3dCbvSrvDescriptorHeap);

	//디바이스(어댑터)마다 서술자 유형별 메모리 크기가다름(32~64바이트)
	// 서술자 힙을 사용하려면 서술자 배열의 원소의 크기를 알아야 함.
	m_d3dCbvCPUDescriptorStartHandle = m_pd3dCbvSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_d3dCbvGPUDescriptorStartHandle = m_pd3dCbvSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	m_d3dSrvCPUDescriptorStartHandle.ptr = m_d3dCbvCPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * nConstantBufferViews);
	m_d3dSrvGPUDescriptorStartHandle.ptr = m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * nConstantBufferViews);
}

void CProgressBarUIShader::CreateConstantBufferViews(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, int nConstantBufferViews, ID3D12Resource *pd3dConstantBuffers, UINT nStride)
{
	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = pd3dConstantBuffers->GetGPUVirtualAddress();
	D3D12_CONSTANT_BUFFER_VIEW_DESC d3dCBVDesc;
	d3dCBVDesc.SizeInBytes = nStride;
	for (int j = 0; j < nConstantBufferViews; j++)
	{
		d3dCBVDesc.BufferLocation = d3dGpuVirtualAddress + (nStride * j);
		D3D12_CPU_DESCRIPTOR_HANDLE d3dCbvCPUDescriptorHandle;
		d3dCbvCPUDescriptorHandle.ptr = m_d3dCbvCPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * j);
		pd3dDevice->CreateConstantBufferView(&d3dCBVDesc, d3dCbvCPUDescriptorHandle);
	}
}

void CProgressBarUIShader::CreateShaderResourceViews(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CTexture *pTexture, UINT nRootParameter, bool bAutoIncrement)
{
	int nTextures = pTexture->GetTextures();
	int nTextureType = pTexture->GetTextureType();
	for (int i = 0; i < nTextures; i++)
	{
		ID3D12Resource *pShaderResource = pTexture->GetTexture(i);				//텍스처 리소스를 하나 얻어온다.
		D3D12_RESOURCE_DESC d3dResourceDesc = pShaderResource->GetDesc();		//그 리소스들에 대한 정보를 얻어온다.
		D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc = GetShaderResourceViewDesc(d3dResourceDesc, nTextureType);  //텍스처에 대한 서술자 뷰를 설정한다.
		pd3dDevice->CreateShaderResourceView(pShaderResource, &d3dShaderResourceViewDesc, m_d3dSrvCPUDescriptorStartHandle);		//cpu가 접근할수있는 srv뷰를 만든다.
		m_d3dSrvCPUDescriptorStartHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;

		//srv뷰를 
		pTexture->SetRootArgument(i, (bAutoIncrement) ? (nRootParameter + i) : nRootParameter, m_d3dSrvGPUDescriptorStartHandle);
		m_d3dSrvGPUDescriptorStartHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
	}
}

void CProgressBarUIShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, void *pContext)
{
	CTexture* pTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Resource/Textures/Loading/Green_ProgressBar.dds", 0);

	UINT ncbElementBytes = ((sizeof(CB_Position) + 255) & ~255); //256의 배수
	CreateCbvSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 1, 1);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 0, false);

	CUI* pUI = new CUI(1);
	CBillboardMesh* pProgressBarMesh = new CBillboardMesh(pd3dDevice, pd3dCommandList, 10.f, 10.f, 0.0f, 0.0f, 0.0f, 0.0f);
	pUI->SetMesh(pProgressBarMesh);
	CMaterial* pUIMaterial = new CMaterial(1);
	pUIMaterial->SetTexture(pTexture, 0);
	pUI->SetMaterial(0, pUIMaterial);
	m_UIMap.emplace(ProgressBar, pUI);

	m_ProgressBarPosition = XMFLOAT4(-1.f, 1.f, -1.f, 0.85f);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CProgressBarUIShader::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature,
	const map<string, CTexture*>& Context, void* pContext)
{
	CBillboardMesh* pProgressBarMesh = new CBillboardMesh(pd3dDevice, pd3dCommandList, 20.f, 20.f, 0.0f, 0.0f, 0.0f, 0.0f);

	auto iter = Context.find("ProgressBar");
	if (iter != Context.end())
	{
		CUI* pUI = new CUI(1);
		pUI->SetMesh(pProgressBarMesh);
		CMaterial* pUIMaterial = new CMaterial(1);
		pUIMaterial->SetTexture((*iter).second, 0);
		pUI->SetMaterial(0, pUIMaterial);
		m_UIMap.emplace(ProgressBar, pUI);

		m_ProgressBarPosition = XMFLOAT4(-1.f, 1.f, -1.f, 0.85f);

		CreateShaderVariables(pd3dDevice, pd3dCommandList);
	}
}

void CProgressBarUIShader::AnimateObjects(float elapsedTime, CCamera* pCamera, CPlayer* pPlayer)
{
	if (g_FileSize > 0)
	{
		double fileSize = g_FileSize;
		double totalSize = g_TotalSize;
		m_ProgressBarPosition.z = -1.f + (fileSize / totalSize) * 2;
	}
}

void CProgressBarUIShader::Render(ID3D12GraphicsCommandList* pd3dCommandList, int nPipelineState)
{
	auto iter = m_UIMap.find(ProgressBar);
	if (iter != m_UIMap.end())
	{
		if (m_pd3dCbvSrvDescriptorHeap)
			pd3dCommandList->SetDescriptorHeaps(1, &m_pd3dCbvSrvDescriptorHeap);

		CUIShader::OnPrepareRender(pd3dCommandList, ProgressBar);
		UpdateShaderVariables(pd3dCommandList);
		(*iter).second->Render(pd3dCommandList, ProgressBar);
	}
}

void CProgressBarUIShader::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_Position) + 255) & ~255); //256의 배수
	m_pd3dPositionData = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dPositionData->Map(0, nullptr, (void**)&m_pMappedPositionData);
}

void CProgressBarUIShader::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (m_pd3dPositionData)
	{
		D3D12_GPU_VIRTUAL_ADDRESS GPUVirtualAddress = m_pd3dPositionData->GetGPUVirtualAddress();
		pd3dCommandList->SetGraphicsRootConstantBufferView(1, GPUVirtualAddress);

		m_pMappedPositionData->m_Position = m_ProgressBarPosition;
	}
}

void CProgressBarUIShader::ReleaseShaderVariables()
{
	if (m_pd3dPositionData)
	{
		m_pd3dPositionData->Unmap(0, nullptr);
		m_pd3dPositionData->Release();
	}

	if (m_pd3dCbvSrvDescriptorHeap)
		m_pd3dCbvSrvDescriptorHeap->Release();
}
