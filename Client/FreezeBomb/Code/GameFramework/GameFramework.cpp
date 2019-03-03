#include "../Stdafx/Stdafx.h"
#include "GameFramework.h"
#include "../Scene/Scene.h"
#include "../GameObject/Player/Player.h"
#include "../GameObject/Shadow/Shadow.h"

#include "../ShaderManager/ShaderManager.h"
#include "../Shader/TerrainShader/TerrainShader.h"
#include "../GameObject/Terrain/Terrain.h"
#include "../ResourceManager/ResourceManager.h"
#include "../Shader/StandardShader/MapToolShader/MapToolShader.h"
#include "../Texture/Texture.h"
#include "../Shader/PostProcessShader/CartoonShader/SobelCartoonShader.h"


CGameFramework::CGameFramework()
{
	m_pdxgiFactory = NULL;
	m_pdxgiSwapChain = NULL;
	m_pd3dDevice = NULL;

	for (int i = 0; i < m_nSwapChainBuffers; i++) m_ppd3dSwapChainBackBuffers[i] = NULL;
	m_nSwapChainBufferIndex = 0;

	m_pd3dCommandAllocator = NULL;
	m_pd3dCommandQueue = NULL;
	m_pd3dCommandList = NULL;

	m_pd3dRtvDescriptorHeap = NULL;
	m_pd3dDsvDescriptorHeap = NULL;

	m_nRtvDescriptorIncrementSize = 0;
	m_nDsvDescriptorIncrementSize = 0;

	m_hFenceEvent = NULL;
	m_pd3dFence = NULL;
	for (int i = 0; i < m_nSwapChainBuffers; i++) m_nFenceValues[i] = 0;

	m_nWndClientWidth = FRAME_BUFFER_WIDTH;
	m_nWndClientHeight = FRAME_BUFFER_HEIGHT;

	m_pScene = NULL;
	m_pPlayer = NULL;

	

	_tcscpy_s(m_pszFrameRate, _T("FreezeBomb ("));
}

CGameFramework::~CGameFramework()
{
	
}

bool CGameFramework::OnCreate(HINSTANCE hInstance, HWND hMainWnd)
{
	m_hInstance = hInstance;
	m_hWnd = hMainWnd;

	

	CreateDirect3DDevice();
	CreateCommandQueueAndList();
	CreateRtvAndDsvDescriptorHeaps();
	CreateSwapChain();

	//CreateDirect11DeviceOn12();
	
	CreateDepthStencilView();

	CoInitialize(NULL);

	//CreateDirectSound();

	BuildObjects();


	CreateOffScreenRenderTargetViews();

	return(true);
}

void CGameFramework::CreateSwapChain()
{
	RECT rcClient;
	::GetClientRect(m_hWnd, &rcClient);
	m_nWndClientWidth = rcClient.right - rcClient.left;
	m_nWndClientHeight = rcClient.bottom - rcClient.top;

#ifdef _WITH_CREATE_SWAPCHAIN_FOR_HWND
	DXGI_SWAP_CHAIN_DESC1 dxgiSwapChainDesc;
	::ZeroMemory(&dxgiSwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC1));
	dxgiSwapChainDesc.Width = m_nWndClientWidth;
	dxgiSwapChainDesc.Height = m_nWndClientHeight;
	dxgiSwapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiSwapChainDesc.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;
	dxgiSwapChainDesc.SampleDesc.Quality = (m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels - 1) : 0;
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgiSwapChainDesc.BufferCount = m_nSwapChainBuffers;
	dxgiSwapChainDesc.Scaling = DXGI_SCALING_NONE;
	dxgiSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	dxgiSwapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	dxgiSwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	DXGI_SWAP_CHAIN_FULLSCREEN_DESC dxgiSwapChainFullScreenDesc;
	::ZeroMemory(&dxgiSwapChainFullScreenDesc, sizeof(DXGI_SWAP_CHAIN_FULLSCREEN_DESC));
	dxgiSwapChainFullScreenDesc.RefreshRate.Numerator = 60;
	dxgiSwapChainFullScreenDesc.RefreshRate.Denominator = 1;
	dxgiSwapChainFullScreenDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	dxgiSwapChainFullScreenDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	dxgiSwapChainFullScreenDesc.Windowed = TRUE;

	HRESULT hResult = m_pdxgiFactory->CreateSwapChainForHwnd(m_pd3dCommandQueue, m_hWnd, &dxgiSwapChainDesc, &dxgiSwapChainFullScreenDesc, NULL, (IDXGISwapChain1 **)&m_pdxgiSwapChain);
#else
	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	::ZeroMemory(&dxgiSwapChainDesc, sizeof(dxgiSwapChainDesc));
	dxgiSwapChainDesc.BufferCount = m_nSwapChainBuffers;
	dxgiSwapChainDesc.BufferDesc.Width = m_nWndClientWidth;
	dxgiSwapChainDesc.BufferDesc.Height = m_nWndClientHeight;
	dxgiSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgiSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	dxgiSwapChainDesc.OutputWindow = m_hWnd;
	dxgiSwapChainDesc.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;
	dxgiSwapChainDesc.SampleDesc.Quality = (m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels - 1) : 0;
	dxgiSwapChainDesc.Windowed = TRUE;
	dxgiSwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	HRESULT hResult = m_pdxgiFactory->CreateSwapChain(m_pd3dCommandQueue, &dxgiSwapChainDesc, (IDXGISwapChain **)&m_pdxgiSwapChain);
#endif
	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();

	hResult = m_pdxgiFactory->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER);

#ifndef _WITH_SWAPCHAIN_FULLSCREEN_STATE
	CreateRenderTargetViews();
#endif
}


void CGameFramework::CreateDirect3DDevice()
{
	HRESULT hResult;

	UINT nDXGIFactoryFlags = 0;
#if defined(_DEBUG)
	ID3D12Debug *pd3dDebugController = NULL;
	hResult = D3D12GetDebugInterface(__uuidof(ID3D12Debug), (void **)&pd3dDebugController);
	if (pd3dDebugController)
	{
		pd3dDebugController->EnableDebugLayer();
		pd3dDebugController->Release();
	}
	nDXGIFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif

	hResult = ::CreateDXGIFactory2(nDXGIFactoryFlags, __uuidof(IDXGIFactory4), (void **)&m_pdxgiFactory);

	IDXGIAdapter1* pd3dAdapter = NULL;

	for (UINT i = 0; DXGI_ERROR_NOT_FOUND != m_pdxgiFactory->EnumAdapters1(i, &pd3dAdapter); i++)
	{
		DXGI_ADAPTER_DESC1 dxgiAdapterDesc;
		pd3dAdapter->GetDesc1(&dxgiAdapterDesc);
		if (dxgiAdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;
		if (SUCCEEDED(D3D12CreateDevice(pd3dAdapter, D3D_FEATURE_LEVEL_12_0, _uuidof(ID3D12Device), (void **)&m_pd3dDevice))) break;
	}

	// DirectX12를 지원하지 않는 경우
	if (pd3dAdapter == nullptr)
	{
		hResult = m_pdxgiFactory->EnumWarpAdapter(_uuidof(IDXGIAdapter1), (void **)&pd3dAdapter);
		hResult = D3D12CreateDevice(pd3dAdapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), (void **)&m_pd3dDevice);
	}

	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS d3dMsaaQualityLevels;
	d3dMsaaQualityLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dMsaaQualityLevels.SampleCount = 4;
	d3dMsaaQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	d3dMsaaQualityLevels.NumQualityLevels = 0;
	hResult = m_pd3dDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &d3dMsaaQualityLevels, sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS));
	m_nMsaa4xQualityLevels = d3dMsaaQualityLevels.NumQualityLevels;
	m_bMsaa4xEnable = (m_nMsaa4xQualityLevels > 1) ? true : false;

	hResult = m_pd3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), (void **)&m_pd3dFence);
	for (UINT i = 0; i < m_nSwapChainBuffers; i++) m_nFenceValues[i] = 0;

	m_hFenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);

	if (pd3dAdapter) pd3dAdapter->Release();

	// IncrementSize를 컴퓨터에 맞게 설정해줌
	::gnCbvSrvDescriptorIncrementSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}





void CGameFramework::CreateCommandQueueAndList()
{
	HRESULT hResult;

	D3D12_COMMAND_QUEUE_DESC d3dCommandQueueDesc;
	::ZeroMemory(&d3dCommandQueueDesc, sizeof(D3D12_COMMAND_QUEUE_DESC));
	d3dCommandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	d3dCommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	hResult = m_pd3dDevice->CreateCommandQueue(&d3dCommandQueueDesc, _uuidof(ID3D12CommandQueue), (void **)&m_pd3dCommandQueue);

	hResult = m_pd3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator), (void **)&m_pd3dCommandAllocator);

	hResult = m_pd3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pd3dCommandAllocator, NULL, __uuidof(ID3D12GraphicsCommandList), (void **)&m_pd3dCommandList);
	hResult = m_pd3dCommandList->Close();
}

void CGameFramework::CreateRtvAndDsvDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
	::ZeroMemory(&d3dDescriptorHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
	d3dDescriptorHeapDesc.NumDescriptors = m_nSwapChainBuffers;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	d3dDescriptorHeapDesc.NodeMask = 0;
	HRESULT hResult = m_pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void **)&m_pd3dRtvDescriptorHeap);
	m_nRtvDescriptorIncrementSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	d3dDescriptorHeapDesc.NumDescriptors = 1;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	hResult = m_pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void **)&m_pd3dDsvDescriptorHeap);
	m_nDsvDescriptorIncrementSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
}



void CGameFramework::CreateRenderTargetViews()
{
	D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle = m_pd3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	for (UINT i = 0; i < m_nSwapChainBuffers; i++)
	{
		m_pdxgiSwapChain->GetBuffer(i, __uuidof(ID3D12Resource), (void **)&m_ppd3dSwapChainBackBuffers[i]);
		m_pd3dDevice->CreateRenderTargetView(m_ppd3dSwapChainBackBuffers[i], NULL, d3dRtvCPUDescriptorHandle);
		d3dRtvCPUDescriptorHandle.ptr += m_nRtvDescriptorIncrementSize;
	}
}

void CGameFramework::CreateDepthStencilView()
{
	D3D12_RESOURCE_DESC d3dResourceDesc;
	d3dResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	d3dResourceDesc.Alignment = 0;
	d3dResourceDesc.Width = m_nWndClientWidth;
	d3dResourceDesc.Height = m_nWndClientHeight;
	d3dResourceDesc.DepthOrArraySize = 1;
	d3dResourceDesc.MipLevels = 1;
	d3dResourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dResourceDesc.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;
	d3dResourceDesc.SampleDesc.Quality = (m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels - 1) : 0;
	d3dResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	d3dResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_HEAP_PROPERTIES d3dHeapProperties;
	::ZeroMemory(&d3dHeapProperties, sizeof(D3D12_HEAP_PROPERTIES));
	d3dHeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	d3dHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	d3dHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	d3dHeapProperties.CreationNodeMask = 1;
	d3dHeapProperties.VisibleNodeMask = 1;

	D3D12_CLEAR_VALUE d3dClearValue;
	d3dClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dClearValue.DepthStencil.Depth = 1.0f;
	d3dClearValue.DepthStencil.Stencil = 0;

	m_pd3dDevice->CreateCommittedResource(&d3dHeapProperties, D3D12_HEAP_FLAG_NONE, &d3dResourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &d3dClearValue, __uuidof(ID3D12Resource), (void **)&m_pd3dDepthStencilBuffer);

	D3D12_DEPTH_STENCIL_VIEW_DESC d3dDepthStencilViewDesc;
	::ZeroMemory(&d3dDepthStencilViewDesc, sizeof(D3D12_DEPTH_STENCIL_VIEW_DESC));
	d3dDepthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dDepthStencilViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	d3dDepthStencilViewDesc.Flags = D3D12_DSV_FLAG_NONE;

	D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle = m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_pd3dDevice->CreateDepthStencilView(m_pd3dDepthStencilBuffer, &d3dDepthStencilViewDesc, d3dDsvCPUDescriptorHandle);
}

void CGameFramework::ChangeSwapChainState()
{
	WaitForGpuComplete();

	BOOL bFullScreenState = FALSE;
	m_pdxgiSwapChain->GetFullscreenState(&bFullScreenState, NULL);
	m_pdxgiSwapChain->SetFullscreenState(!bFullScreenState, NULL);

	DXGI_MODE_DESC dxgiTargetParameters;
	dxgiTargetParameters.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiTargetParameters.Width = m_nWndClientWidth;
	dxgiTargetParameters.Height = m_nWndClientHeight;
	dxgiTargetParameters.RefreshRate.Numerator = 60;
	dxgiTargetParameters.RefreshRate.Denominator = 1;
	dxgiTargetParameters.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	dxgiTargetParameters.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	m_pdxgiSwapChain->ResizeTarget(&dxgiTargetParameters);

	for (int i = 0; i < m_nSwapChainBuffers; i++) if (m_ppd3dSwapChainBackBuffers[i]) m_ppd3dSwapChainBackBuffers[i]->Release();

	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	m_pdxgiSwapChain->GetDesc(&dxgiSwapChainDesc);
	m_pdxgiSwapChain->ResizeBuffers(m_nSwapChainBuffers, m_nWndClientWidth, m_nWndClientHeight, dxgiSwapChainDesc.BufferDesc.Format, dxgiSwapChainDesc.Flags);

	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();

	CreateRenderTargetViews();
}
void CGameFramework::CreateDirect11DeviceOn12()
{
	HRESULT hResult;
	UINT d3d11DeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;		//Note  Required for Direct2D interoperability with Direct3D resources.

	UINT dxgiFactoryFlags = 0;
	D2D1_FACTORY_OPTIONS d2dFactoryOptions = {};

//#if defined(_DEBUG)
//	// Enable the debug layer (requires the Graphics Tools "optional feature").
//	// NOTE: Enabling the debug layer after device creation will invalidate the active device.
//	{
//		ComPtr<ID3D12Debug> debugController;
//		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
//		{
//			debugController->EnableDebugLayer();
//
//			// Enable additional debug layers.
//			dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
//			d3d11DeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
//			d2dFactoryOptions.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
//		}
//	}
//#endif
//
//#if defined(_DEBUG)
//	// Filter a debug error coming from the 11on12 layer.
//	ComPtr<ID3D12InfoQueue> infoQueue;
//	if (SUCCEEDED(m_pd3dDevice->QueryInterface(IID_PPV_ARGS(&infoQueue))))
//	{
//		// Suppress whole categories of messages.
//		//D3D12_MESSAGE_CATEGORY categories[] = {};
//
//		// Suppress messages based on their severity level.
//		D3D12_MESSAGE_SEVERITY severities[] =
//		{
//			D3D12_MESSAGE_SEVERITY_INFO,
//		};
//
//		// Suppress individual messages by their ID.
//		D3D12_MESSAGE_ID denyIds[] =
//		{
//			// This occurs when there are uninitialized descriptors in a descriptor table, even when a
//			// shader does not access the missing descriptors.
//			D3D12_MESSAGE_ID_INVALID_DESCRIPTOR_HANDLE,
//		};
//
//		D3D12_INFO_QUEUE_FILTER filter = {};
//		//filter.DenyList.NumCategories = _countof(categories);
//		//filter.DenyList.pCategoryList = categories;
//		filter.DenyList.NumSeverities = _countof(severities);
//		filter.DenyList.pSeverityList = severities;
//		filter.DenyList.NumIDs = _countof(denyIds);
//		filter.DenyList.pIDList = denyIds;
//
//		HRESULT hr;
//		hr = infoQueue->PushStorageFilter(&filter);
//		if (FAILED(hr))
//		{
//			return;
//		}
//	}
//#endif

	//ComPtr<ID3D11Device> d3d11Device;
	hResult = D3D11On12CreateDevice(m_pd3dDevice, d3d11DeviceFlags, nullptr, 0, reinterpret_cast<IUnknown**>(m_pd3dCommandQueue), 1, 0, &m_d3d11Device, &m_d3d11DeviceContext, nullptr);
	if (FAILED(hResult))
	{
		return;
	}


}


void CGameFramework::CreateOffScreenRenderTargetViews()
{
	CTexture *pTextureForCartoonProcessing = new CTexture(m_nCartoonScreenRenderTargetBuffers, RESOURCE_TEXTURE2D_ARRAY,0);

	D3D12_CLEAR_VALUE d3dClearValue = { DXGI_FORMAT_R8G8B8A8_UNORM, { 0.0f, 0.0f, 0.0f, 1.0f } };

	for (UINT i = 0; i < m_nCartoonScreenRenderTargetBuffers; i++)
	{
		m_ppd3dCartoonScreenRenderTargetBuffers[i] = pTextureForCartoonProcessing->CreateTexture(m_pd3dDevice, m_pd3dCommandList, m_nWndClientWidth, m_nWndClientHeight, DXGI_FORMAT_R8G8B8A8_UNORM, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,i);
		m_ppd3dCartoonScreenRenderTargetBuffers[i]->AddRef();
	}

	 //RtvCPU서술자 증가 크기가 스왑체인의 크기 배수로 증가하는 이유?
	D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle = m_pd3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	d3dRtvCPUDescriptorHandle.ptr += (m_nSwapChainBuffers * m_nRtvDescriptorIncrementSize);

	D3D12_RENDER_TARGET_VIEW_DESC d3dRenderTargetViewDesc;
	d3dRenderTargetViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dRenderTargetViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	d3dRenderTargetViewDesc.Texture2D.MipSlice = 0;
	d3dRenderTargetViewDesc.Texture2D.PlaneSlice = 0;

	m_pCartoonShader = new CSobelCartoonShader();
	m_pCartoonShader->CreateGraphicsRootSignature(m_pd3dDevice);
	m_pCartoonShader->CreateShader(m_pd3dDevice, m_pCartoonShader->GetGraphicsRootSignature(), 1);
	m_pCartoonShader->BuildObjects(m_pd3dDevice, m_pd3dCommandList, pTextureForCartoonProcessing);
}
void CGameFramework::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	if (m_pScene) m_pScene->OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
	switch (nMessageID)
	{
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
			::SetCapture(hWnd);
			::GetCursorPos(&m_ptOldCursorPos);
			break;
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
			::ReleaseCapture();
			break;
		case WM_MOUSEMOVE:
			break;
		default:
			break;
	}
}


void CGameFramework::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{

	if (m_pScene) m_pScene->OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
	switch (nMessageID)
	{
		case WM_KEYUP:
			switch (wParam)
			{
				case VK_ESCAPE:
					::PostQuitMessage(0);
					break;
			//	case VK_RETURN:

				//	break;
				case VK_F1:
				case VK_F2:
				case VK_F3:
					m_pCamera = m_pPlayer->ChangeCamera((DWORD)(wParam - VK_F1 + 1), m_GameTimer.GetTimeElapsed());
					break;
				case VK_F4:
					m_bCartoon = !m_bCartoon;
					break;
				case VK_F9:
					ChangeSwapChainState();
					break;
				//case '1':
				//	//AddFileData(m_pPlayer, wParam);
				//	
				//	break;
				//case '2':
				//	//AddFileData(m_pPlayer, wParam);
				//	break;
				//case '3':
				//	break;
				default:
					break;
			}
			break;
		default:
			break;
	}
}
#ifdef _MAPTOOL_MODE_
void CGameFramework::OnMapToolInputMesseage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYUP:
		switch (wParam)
		{
		case '1':
			if (m_pMapToolShader)
			{
				if (m_pPlayer)
				{
					int modelIndex = m_pMapToolShader->GetCurrPineTreeIndex();
					m_pMapToolShader->SetCurrPineTreeIndex((modelIndex + 1) % m_pMapToolShader->GetPineTreeModelCount()+1);
					m_pMapToolShader->InsertObject(m_pd3dDevice, m_pd3dCommandList, m_pPlayer, string("SM_PineTree_Snow_0")+to_string(m_pMapToolShader->GetCurrPineTreeIndex()));
				}
			}
			break;
		case '2':
			if (m_pMapToolShader)
			{
				if (m_pPlayer)
				{
					int modelIndex = m_pMapToolShader->GetCurrDeadTreeIndex();
					m_pMapToolShader->SetCurrDeadTreeIndex((modelIndex + 1) % m_pMapToolShader->GetDeadTreeModelCount() + 1);
					m_pMapToolShader->InsertObject(m_pd3dDevice, m_pd3dCommandList, m_pPlayer, string("SM_DeadTrunk_0")+to_string(m_pMapToolShader->GetCurrDeadTreeIndex()));
				}
			}
			break;
		case '3':
			if (m_pMapToolShader)
			{
				if (m_pPlayer)
				{
					int modelIndex = m_pMapToolShader->GetCurrBigRockIndex();
					m_pMapToolShader->SetCurrBigRockIndex((modelIndex + 1) % m_pMapToolShader->GetBigRockModelCount() + 1);
					m_pMapToolShader->InsertObject(m_pd3dDevice, m_pd3dCommandList, m_pPlayer, string("SM_BigPlainRock_Snow_0") + to_string(m_pMapToolShader->GetCurrBigRockIndex()));
				}
			}
			break;
		case '4':
			if (m_pMapToolShader)
			{
				if (m_pPlayer)
				{
					int modelIndex = m_pMapToolShader->GetCurrPondModelIndex();
					m_pMapToolShader->SetCurrPondIndex((modelIndex + 1) % m_pMapToolShader->GetPondModelCount() + 1);
					m_pMapToolShader->InsertObject(m_pd3dDevice, m_pd3dCommandList, m_pPlayer, string("PondSquare"));
				}
			}
			break;
		case '5':
			if (m_pMapToolShader)
			{
				if (m_pPlayer)
				{
					int modelIndex = m_pMapToolShader->GetCurrDeerIndex();
					m_pMapToolShader->SetCurrDeerIndex((modelIndex + 1)% m_pMapToolShader->GetDeerModelCount() + 1);
					m_pMapToolShader->InsertObject(m_pd3dDevice, m_pd3dCommandList, m_pPlayer, string("SM_Deer"));
				}
			}
			break;
		case '6':
			if (m_pMapToolShader)
			{
				if (m_pPlayer)
				{
					string s;
					int modelIndex = m_pMapToolShader->GetCurrFenceIndex();
					m_pMapToolShader->SetCurrFenceIndex((modelIndex + 1)% m_pMapToolShader->GetFenceModelCount());
					if (m_pMapToolShader->GetCurrFenceIndex() % 2 == 0)
					{
						s = 'A';
					}
					else
					{
						s = 'B';
					}
					m_pMapToolShader->InsertObject(m_pd3dDevice, m_pd3dCommandList, m_pPlayer, string("LowPoly_-_Fence_")+s);
				}
			}

			break;
		case 'S':
			if (m_pMapToolShader)
			{
				m_pMapToolShader->MakeMapFile();
			}
			break;
		case 'B':
			if (m_pMapToolShader)
			{
				m_pMapToolShader->MakeMapBinaryFile(string("MapVer1"));
			}
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}
#endif

LRESULT CALLBACK CGameFramework::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
		case WM_ACTIVATE:
		{
			if (LOWORD(wParam) == WA_INACTIVE)
				m_GameTimer.Stop();
			else
				m_GameTimer.Start();
			break;
		}
		case WM_SIZE:
			break;
		case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        case WM_MOUSEMOVE:
			OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
            break;
        case WM_KEYDOWN:
        case WM_KEYUP:
			OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);

#ifdef _MAPTOOL_MODE_
			OnMapToolInputMesseage(hWnd, nMessageID, wParam, lParam);
#endif
			break;
	}
	return(0);
}

void CGameFramework::OnDestroy()
{
    ReleaseObjects();

	::CloseHandle(m_hFenceEvent);

	if (m_pd3dDepthStencilBuffer) m_pd3dDepthStencilBuffer->Release();
	if (m_pd3dDsvDescriptorHeap) m_pd3dDsvDescriptorHeap->Release();

	for (int i = 0; i < m_nSwapChainBuffers; i++) if (m_ppd3dSwapChainBackBuffers[i]) m_ppd3dSwapChainBackBuffers[i]->Release();
	if (m_pd3dRtvDescriptorHeap) m_pd3dRtvDescriptorHeap->Release();

	if (m_pd3dCommandAllocator) m_pd3dCommandAllocator->Release();
	if (m_pd3dCommandQueue) m_pd3dCommandQueue->Release();
	if (m_pd3dCommandList) m_pd3dCommandList->Release();

	if (m_pd3dFence) m_pd3dFence->Release();

	m_pdxgiSwapChain->SetFullscreenState(FALSE, NULL);
	if (m_pdxgiSwapChain) m_pdxgiSwapChain->Release();
    if (m_pd3dDevice) m_pd3dDevice->Release();
	if (m_pdxgiFactory) m_pdxgiFactory->Release();

#if defined(_DEBUG)
	IDXGIDebug1	*pdxgiDebug = NULL;
	DXGIGetDebugInterface1(0, __uuidof(IDXGIDebug1), (void **)&pdxgiDebug);
	HRESULT hResult = pdxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_DETAIL);
	pdxgiDebug->Release();
#endif
}

void CGameFramework::BuildObjects()
{
	m_pd3dCommandList->Reset(m_pd3dCommandAllocator, NULL);

	m_pScene = new CScene();
	if (m_pScene) 
		m_pScene->BuildObjects(m_pd3dDevice, m_pd3dCommandList);

	CTerrainPlayer* pPlayer{ nullptr };

	if (m_pScene->getShaderManager())
	{

		map<string, CShader*> m = m_pScene->getShaderManager()->getShaderMap();
		auto iter = m.find("Terrain");
		if(iter != m.end())
		{
			CTerrain* pTerrain = dynamic_cast<CTerrainShader*>((*iter).second)->getTerrain();
			pPlayer = new CTerrainPlayer(m_pd3dDevice, m_pd3dCommandList, m_pScene->GetGraphicsRootSignature(),CGameObject::MATERIALTYPE::PANDA,pTerrain);		
			pPlayer->SetPosition(XMFLOAT3(0.f, pTerrain->GetHeight(0.f, 0.f), 0.f));
			pPlayer->SetScale(XMFLOAT3(10.0f, 10.0f, 10.0f));	

			map<string, Bounds*> BoundMap = m_pScene->getShaderManager()->getResourceManager()->getBoundMap();
			auto iter2 = BoundMap.find(pPlayer->getID());
			if (iter2 != BoundMap.end())
				pPlayer->SetOOBB((*iter2).second->m_xmf3Center, (*iter2).second->m_xmf3Extent, XMFLOAT4(0, 0, 0, 1));

#ifdef _MAPTOOL_MODE_
			m_pMapToolShader = new CMapToolShader();
			m_pMapToolShader->CreateShader(m_pd3dDevice, m_pd3dCommandList, m_pScene->GetGraphicsRootSignature());
			m_pMapToolShader->BuildObjects(m_pd3dDevice, m_pd3dCommandList, m_pScene->GetGraphicsRootSignature(), pTerrain);
#endif
		}
	}

	m_pPlayer = pPlayer;
	m_pScene->setPlayer(m_pPlayer);
	m_pCamera = m_pPlayer->GetCamera();

	m_pd3dCommandList->Close();
	ID3D12CommandList *ppd3dCommandLists[] = { m_pd3dCommandList };
	m_pd3dCommandQueue->ExecuteCommandLists(1, ppd3dCommandLists);

	WaitForGpuComplete();

	if (m_pScene) 
		m_pScene->ReleaseUploadBuffers();
	if (m_pPlayer) 
		m_pPlayer->ReleaseUploadBuffers();

	m_GameTimer.Reset();
}

void CGameFramework::ReleaseObjects()
{
	if (m_pPlayer) 
		m_pPlayer->Release();

#ifdef _MAPTOOL_MODE_
	if (m_pMapToolShader)
	{
		m_pMapToolShader->ReleaseShaderVariables();
		m_pMapToolShader->ReleaseObjects();
		delete m_pMapToolShader;
	}
#endif
	
	//ReleaseDirectSound();

	if (m_pScene)
	{
		m_pScene->ReleaseObjects();
		delete m_pScene;
	}

	if (m_pCartoonShader)
	{
		m_pCartoonShader->ReleaseShaderVariables();
		m_pCartoonShader->ReleaseObjects();
		delete m_pCartoonShader;
	}
}

void CGameFramework::ProcessInput()
{
	static UCHAR pKeysBuffer[256];
	bool bProcessedByScene = false;
	if (GetKeyboardState(pKeysBuffer) && m_pScene) bProcessedByScene = m_pScene->ProcessInput(pKeysBuffer);
	if (!bProcessedByScene)
	{
		DWORD dwDirection = 0;
		if (pKeysBuffer[VK_UP] & 0xF0)
		{
			dwDirection |= DIR_FORWARD;
			m_pPlayer->SetDirection(dwDirection);
		}
		if (pKeysBuffer[VK_DOWN] & 0xF0)
		{
			dwDirection |= DIR_BACKWARD;
			m_pPlayer->SetDirection(dwDirection);
		}
		if (pKeysBuffer[VK_LEFT] & 0xF0)
		{
			dwDirection |= DIR_LEFT;
			m_pPlayer->SetDirection(dwDirection);
		}
		if (pKeysBuffer[VK_RIGHT] & 0xF0)
		{
			dwDirection |= DIR_RIGHT;
			m_pPlayer->SetDirection(dwDirection);
		}
		if (pKeysBuffer[VK_PRIOR] & 0xF0) dwDirection |= DIR_UP;
		if (pKeysBuffer[VK_NEXT] & 0xF0) dwDirection |= DIR_DOWN;

		float cxDelta = 0.0f, cyDelta = 0.0f;
		POINT ptCursorPos;
		if (GetCapture() == m_hWnd)
		{
			SetCursor(NULL);
			GetCursorPos(&ptCursorPos);
			cxDelta = (float)(ptCursorPos.x - m_ptOldCursorPos.x) / 3.0f;
			cyDelta = (float)(ptCursorPos.y - m_ptOldCursorPos.y) / 3.0f;
			SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y);
		}

		if ((dwDirection != 0) || (cxDelta != 0.0f) || (cyDelta != 0.0f))
		{
			if (cxDelta || cyDelta)
			{
				if (pKeysBuffer[VK_RBUTTON] & 0xF0)
					m_pPlayer->Rotate(cyDelta, 0.0f, -cxDelta);
				else
					m_pPlayer->Rotate(cyDelta, cxDelta, 0.0f);
			}
			if (dwDirection) m_pPlayer->Move(dwDirection, 12.25f, true);
		}
	}
	m_pPlayer->Update(m_GameTimer.GetTimeElapsed());
}

void CGameFramework::AnimateObjects()
{
	float fTimeElapsed = m_GameTimer.GetTimeElapsed();

	if (m_pScene) 
		m_pScene->AnimateObjects(fTimeElapsed);

	
	m_pPlayer->Animate(fTimeElapsed);
	m_pPlayer->UpdateTransform(NULL);

	if(m_pScene)
		m_pScene->CheckObjectByObjectCollisions();
}

void CGameFramework::WaitForGpuComplete()
{
	const UINT64 nFenceValue = ++m_nFenceValues[m_nSwapChainBufferIndex];
	HRESULT hResult = m_pd3dCommandQueue->Signal(m_pd3dFence, nFenceValue);

	if (m_pd3dFence->GetCompletedValue() < nFenceValue)
	{
		hResult = m_pd3dFence->SetEventOnCompletion(nFenceValue, m_hFenceEvent);
		::WaitForSingleObject(m_hFenceEvent, INFINITE);
	}
}

void CGameFramework::MoveToNextFrame()
{
	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();
	//m_nSwapChainBufferIndex = (m_nSwapChainBufferIndex + 1) % m_nSwapChainBuffers;

	UINT64 nFenceValue = ++m_nFenceValues[m_nSwapChainBufferIndex];
	HRESULT hResult = m_pd3dCommandQueue->Signal(m_pd3dFence, nFenceValue);

	if (m_pd3dFence->GetCompletedValue() < nFenceValue)
	{
		hResult = m_pd3dFence->SetEventOnCompletion(nFenceValue, m_hFenceEvent);
		::WaitForSingleObject(m_hFenceEvent, INFINITE);
	}
}

//#define _WITH_PLAYER_TOP

void CGameFramework::FrameAdvance()
{    
	m_GameTimer.Tick(60.0f);
	
	ProcessInput();

    AnimateObjects();

	float pfClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	
	HRESULT hResult = m_pd3dCommandAllocator->Reset();
	hResult = m_pd3dCommandList->Reset(m_pd3dCommandAllocator, NULL);

	::SynchronizeResourceTransition(m_pd3dCommandList, m_ppd3dSwapChainBackBuffers[m_nSwapChainBufferIndex], D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	m_pd3dRtvSwapChainBackBufferCPUHandles[m_nSwapChainBufferIndex] = m_pd3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_pd3dRtvSwapChainBackBufferCPUHandles[m_nSwapChainBufferIndex].ptr += (m_nSwapChainBufferIndex * m_nRtvDescriptorIncrementSize);

	
	m_pd3dCommandList->ClearRenderTargetView(m_pd3dRtvSwapChainBackBufferCPUHandles[m_nSwapChainBufferIndex], pfClearColor, 0, NULL);
	
	D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvDepthStencilBufferCPUHandle = m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_pd3dCommandList->ClearDepthStencilView(d3dDsvDepthStencilBufferCPUHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);

	m_pd3dCommandList->OMSetRenderTargets(1, &m_pd3dRtvSwapChainBackBufferCPUHandles[m_nSwapChainBufferIndex], TRUE, &d3dDsvDepthStencilBufferCPUHandle);

	if (m_pScene)
	{
		m_pScene->Render(m_pd3dCommandList, m_pCamera);
	}

#ifdef _MAPTOOL_MODE_
	if (m_pMapToolShader)
	{
		m_pMapToolShader->Render(m_pd3dCommandList, m_pCamera);
	}
		
#endif

#ifdef _WITH_PLAYER_TOP
	m_pd3dCommandList->ClearDepthStencilView(d3dDsvCPUDescriptorHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);
#endif

	if (m_pPlayer)
		m_pPlayer->Render(m_pd3dCommandList, m_pCamera, GameObject);

	if (m_pCartoonShader)
	{
		if (m_bCartoon)
		{
			m_pCartoonShader->SobelFilter(m_pd3dCommandList, m_ppd3dSwapChainBackBuffers[m_nSwapChainBufferIndex], m_ppd3dCartoonScreenRenderTargetBuffers, m_pCamera);
			m_pCartoonShader->Render(m_pd3dCommandList, m_ppd3dSwapChainBackBuffers[m_nSwapChainBufferIndex], m_ppd3dCartoonScreenRenderTargetBuffers, m_pCamera);
		}
	}
	



	::SynchronizeResourceTransition(m_pd3dCommandList, m_ppd3dSwapChainBackBuffers[m_nSwapChainBufferIndex], D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

	
	//////////////////////////////////////////////////////////////////////////////////
	//D2D1_SIZE_F renderTargetSize = m_pRenderTarget->GetSize();

	//m_pRenderTarget->BeginDraw();

	//m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());


	//m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

	//m_pRenderTarget->EndDraw();

	////////////////////////////////////////////////////////////////////////////////

	hResult = m_pd3dCommandList->Close();
	
	ID3D12CommandList *ppd3dCommandLists[] = { m_pd3dCommandList };
	m_pd3dCommandQueue->ExecuteCommandLists(1, ppd3dCommandLists);

	WaitForGpuComplete();

#ifdef _WITH_PRESENT_PARAMETERS
	DXGI_PRESENT_PARAMETERS dxgiPresentParameters;
	dxgiPresentParameters.DirtyRectsCount = 0;
	dxgiPresentParameters.pDirtyRects = NULL;
	dxgiPresentParameters.pScrollRect = NULL;
	dxgiPresentParameters.pScrollOffset = NULL;
	m_pdxgiSwapChain->Present1(1, 0, &dxgiPresentParameters);
#else
#ifdef _WITH_SYNCH_SWAPCHAIN
	m_pdxgiSwapChain->Present(1, 0);
#else
	m_pdxgiSwapChain->Present(0, 0);
#endif
#endif

//	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();
	MoveToNextFrame();

	m_GameTimer.GetFrameRate(m_pszFrameRate + 12, 37);
	size_t nLength = _tcslen(m_pszFrameRate);
	XMFLOAT3 xmf3Position = m_pPlayer->GetPosition();
	_stprintf_s(m_pszFrameRate + nLength, 70 - nLength, _T(" (%4f, %4f, %4f)"), xmf3Position.x, xmf3Position.y, xmf3Position.z);
	::SetWindowText(m_hWnd, m_pszFrameRate);
}

