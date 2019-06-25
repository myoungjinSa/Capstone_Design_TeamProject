#include "../../../../Stdafx/Stdafx.h"
#include "../../../../Texture/Texture.h"
#include "../../../../Mesh/BillboardMesh/BillboardMesh.h"
#include "../../../../Material/Material.h"
#include "../../../../GameObject/Billboard/UI/UI.h"
#include "IDShader.h"
#include "../../../../GameFramework/GameFramework.h"
#ifdef _WITH_SERVER_
CIDShader::CIDShader()
{
}

CIDShader::~CIDShader()
{

}

void CIDShader::CreateShader(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];

	for (int i = 0; i < m_nPipelineStates; ++i)
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

		HRESULT hResult = pd3dDevice->CreateGraphicsPipelineState(&m_d3dPipelineStateDesc, __uuidof(ID3D12PipelineState), (void **)&m_ppd3dPipelineStates[i]);
	}

	if (m_pd3dVertexShaderBlob) m_pd3dVertexShaderBlob->Release();
	if (m_pd3dPixelShaderBlob) m_pd3dPixelShaderBlob->Release();

	if (m_d3dPipelineStateDesc.InputLayout.pInputElementDescs) delete[] m_d3dPipelineStateDesc.InputLayout.pInputElementDescs;
}

void CIDShader::CreateCbvSrvDescriptorHeaps(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, int nConstantBufferViews, int nShaderResourceViews)
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

void CIDShader::CreateConstantBufferViews(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, int nConstantBufferViews, ID3D12Resource *pd3dConstantBuffers, UINT nStride)
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

void CIDShader::CreateShaderResourceViews(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CTexture *pTexture, UINT nRootParameter, bool bAutoIncrement)
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

D3D12_SHADER_BYTECODE CIDShader::CreateVertexShader()
{
	return(CShader::CompileShaderFromFile(L"../Code/Shader/HLSL/UI.hlsl", "VSIDScene", "vs_5_1", &m_pd3dVertexShaderBlob));
}

D3D12_SHADER_BYTECODE CIDShader::CreatePixelShader()
{
	return(CShader::CompileShaderFromFile(L"../Code/Shader/HLSL/UI.hlsl", "PSIDScene", "ps_5_1", &m_pd3dVertexShaderBlob));
}


void CIDShader::BuildObjects(ID3D12Device* pd3dDevice,ID3D12GraphicsCommandList *pd3dCommandList,ID3D12RootSignature *pd3dGraphicsRootSignature,void *pContext)
{
	constexpr int nTextures = 3;
	CreateCbvSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 0, nTextures);


	CTexture** pLoginTextures = new CTexture*[nTextures];
	
	pLoginTextures[NO_SELECT] = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	pLoginTextures[NO_SELECT]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Resource/Textures/Login/ID_1.dds", 0);

	pLoginTextures[ID_SELECT] = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	pLoginTextures[ID_SELECT]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Resource/Textures/Login/ID_2.dds", 0);

	
	pLoginTextures[REQUEST_LOGIN] = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	pLoginTextures[REQUEST_LOGIN]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Resource/Textures/Login/ID_3.dds", 0);


	for (int i = 0; i < nTextures; ++i)
	{
		vTexture.emplace_back(pLoginTextures[i]);
		CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pLoginTextures[i], 0, false);
	}

	CBillboardMesh* pLoginMesh = new CBillboardMesh(pd3dDevice, pd3dCommandList, 20.0f, 20.0f, 0.0f, 0.0f, 0.0f, 0.0f);



	m_nObjects = nTextures;
	m_ppUIMaterial = new CMaterial*[m_nObjects];

	m_ppUIMaterial[0] = new CMaterial(m_nObjects);
	
	
	for (int i = 0; i < m_nObjects; ++i)
	{
		m_ppUIMaterial[0]->SetTexture(vTexture[i], i);
		CUI* pUI = new CUI(1);
		pUI->SetMesh(pLoginMesh);
		pUI->SetMaterial(0, m_ppUIMaterial[0]);
		m_UIMap.emplace(i, pUI);
	}
	
}
void CIDShader::AnimateObjects(float fTimeElapsed)
{
	

}

int CIDShader::DecideTextureByCursor(WPARAM wParam,LONG mouseX,LONG mouseY)
{
	cout << mouseX << "," << mouseY<<endl;

	
	if (339 <= mouseX && mouseX <= 827 && 656 <= mouseY && mouseY <= 692)
	{
		
		m_currentTexture = ID_SELECT;

	}
	else if (887 <= mouseX && mouseX <= 1051 && 652 <= mouseY && mouseY <= 692)
	{
		m_currentTexture = REQUEST_LOGIN;

	}
	else
	{
		m_currentTexture = NO_SELECT;
	}
	return m_currentTexture;

}
void CIDShader::Render(ID3D12GraphicsCommandList *pd3dCommandList,int nPipelineState,bool isInput)
{
	if (m_pd3dCbvSrvDescriptorHeap) 
		pd3dCommandList->SetDescriptorHeaps(1, &m_pd3dCbvSrvDescriptorHeap);

	CUIShader::OnPrepareRender(pd3dCommandList, 0);

	if(isInput)
	{
		auto iter = m_UIMap.find(NO_SELECT);
		if (iter != m_UIMap.end())
		(*iter).second->Render(pd3dCommandList, nPipelineState,NO_SELECT);
	}
	else
	{
		auto iter = m_UIMap.find(m_currentTexture);
		if (iter != m_UIMap.end())
		(*iter).second->Render(pd3dCommandList, nPipelineState,m_currentTexture);
	
	}
	

}

void CIDShader::ReleaseObjects()
{
	if (m_pd3dCbvSrvDescriptorHeap)
		m_pd3dCbvSrvDescriptorHeap->Release();

	CUIShader::ReleaseObjects();

	vTexture.clear();
}
#endif