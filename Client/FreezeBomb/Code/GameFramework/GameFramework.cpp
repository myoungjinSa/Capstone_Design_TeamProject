#include "../Stdafx/Stdafx.h"
#include "GameFramework.h"
#include "../Network/Network.h"

#include "../Scene/Scene.h"
#include "../GameObject/Player/Player.h"
#include "../GameObject/Shadow/Shadow.h"

#include "../Scene/LoadingScene/LoadingScene.h"
#include "../ShaderManager/ShaderManager.h"
#include "../Shader/TerrainShader/TerrainShader.h"
#include "../Shader/BillboardShader/SnowShader/SnowShader.h"
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
#ifdef _WITH_DIRECT2D_
	CreateDirect2DDevice();
#endif
	CreateRtvAndDsvDescriptorHeaps();
	CreateSwapChain();
	CreateDepthStencilView();

	CoInitialize(NULL);

	

	if (!BuildObjects())
		return false;

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

#ifdef _WITH_DIRECT2D_
void CGameFramework::CreateDirect2DDevice()
{
	UINT nD3D11DeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if defined(_DEBUG)
	nD3D11DeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	//ID3D12Device 생성 후 ID3D11On12Device 를 생성한다.
	//ID3D11Device는 D3D11OnCreateDevice API를 통해 ID3D12Device에 Wrapping되어있다.
	//이 api는 11On12 디바이스에 명령들을  ID3D12CommandQueue에 제출하는 역할을 한다.
	//ID3D11Device 가 생성된 후에, ID3D11On12Device 인터페이스를 ID3D11Device로 부터 요청할 수 있다.
	//ID3D11On12Device 가 D2D를 설치하는데 사용하는 주 디바이스가 된다.
	ID3D11Device *pd3d11Device = NULL;
	HRESULT hResult = ::D3D11On12CreateDevice(m_pd3dDevice, nD3D11DeviceFlags, NULL, 0, (IUnknown **)&m_pd3dCommandQueue, 1, 0, &pd3d11Device, &m_pd3d11DeviceContext, NULL);
	hResult = pd3d11Device->QueryInterface(__uuidof(ID3D11On12Device), (void **)&m_pd3d11On12Device);
	if (pd3d11Device) pd3d11Device->Release();


	//11On12 디바이스가 생성된 후, 우리는 이 디바이스를 D2DFactory와 디바이스를 
	//다이렉트 11에서 했던 것 과 동일하게 생성해준다.

	D2D1_FACTORY_OPTIONS nD2DFactoryOptions = { D2D1_DEBUG_LEVEL_NONE };
#if defined(_DEBUG)
	nD2DFactoryOptions.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif
	
	//D2D1_FACTORY_TYPE_SINGLE_THREADED ->팩토리에 접근하거나 쓰는것 또는 객체를 생성하는 것으로부터 동기화를 제공하지 않는다.
	//									만약 팩토리나 객체들이 다중 스레드에서 호출되면, Application은 접근을 lock해준다.
	hResult = ::D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory3), &nD2DFactoryOptions, (void**)&m_pd2dFactory);


	////////////////////////////////////////////////////////////////////////////////////
	IDXGIDevice *pdxgiDevice = NULL;
	hResult = m_pd3d11On12Device->QueryInterface(__uuidof(IDXGIDevice), (void **)&pdxgiDevice);
	hResult = m_pd2dFactory->CreateDevice(pdxgiDevice, &m_pd2dDevice);
	hResult = m_pd2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &m_pd2dDeviceContext);
	hResult = ::DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), (IUnknown **)&m_pdWriteFactory);

	if(pdxgiDevice) 
	{
		pdxgiDevice->Release();
	}

	m_pd2dDeviceContext->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);

	m_pd2dDeviceContext->CreateSolidColorBrush(D2D1::ColorF(0.3f, 0.0f, 0.0f, 0.5f), &m_pd2dbrBackground);
	m_pd2dDeviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF(0x9ACD32, 1.0f)), &m_pd2dbrBorder);

	m_pdwFont = new IDWriteTextFormat*[m_nNameFont];
	m_pd2dbrText = new ID2D1SolidColorBrush*[m_nNameFont];
	for (int i = 0; i < m_nNameFont; ++i) 
	{
		hResult = m_pdWriteFactory->CreateTextFormat(L"고딕", nullptr, DWRITE_FONT_WEIGHT_DEMI_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 30.0f, L"en-US", &m_pdwFont[i]);
		hResult = m_pdwFont[i]->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		hResult = m_pdwFont[i]->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		hResult = m_pdWriteFactory->CreateTextLayout(L"텍스트 레이아웃", 8, m_pdwFont[i], 4096.0f, 4096.0f, &m_pdwTextLayout);
	}
	
	int index = 0;
	m_pd2dDeviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::LightBlue, 1.0f), &m_pd2dbrText[index++]);
	m_pd2dDeviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Orange, 1.0f), &m_pd2dbrText[index++]);
	m_pd2dDeviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Yellow, 1.0f), &m_pd2dbrText[index++]);
	m_pd2dDeviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::LightGreen, 1.0f), &m_pd2dbrText[index++]);
	m_pd2dDeviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::LightCoral, 1.0f), &m_pd2dbrText[index++]);
	m_pd2dDeviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::LavenderBlush, 1.0f), &m_pd2dbrText[index++]);

	//Initializes the COM library on the current thread and identifies the concurrency model as single-thread apartment 
	CoInitialize(NULL);
	hResult = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, __uuidof(IWICImagingFactory), (void**)&m_pwicImagingFactory);

	hResult = m_pd2dFactory->CreateDrawingStateBlock( &m_pd2dsbDrawingState);
	hResult = m_pd2dDeviceContext->CreateEffect(CLSID_D2D1BitmapSource, &m_pd2dfxBitmapSource);


	IWICBitmapDecoder *pwicBitmapDecoder;
	hResult = m_pwicImagingFactory->CreateDecoderFromFilename(L"../Resource/Png/ScoreBoard.png", NULL, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &pwicBitmapDecoder);

	IWICBitmapFrameDecode *pwicFrameDecode;
	pwicBitmapDecoder->GetFrame(0, &pwicFrameDecode);	//GetFrame() : Retrieves the specified frame of the image.


	//CreateFormatConverter::Creates a new instance of the IWICFormatConverter class.
	m_pwicImagingFactory->CreateFormatConverter(&m_pwicFormatConverter);

	//Initializes the format converter.
	//1. WIICBitmapSource* : the input bitmap to convert
	//2. REFWICPixelFormatGUID : The destination pixel format GUID.
	//3. WICBitmapDitherType : The WICBitmapDitherType used for conversation.
	// WICBitmapDitherTypeNone -> A solid color algorithm without dither.
	//							//떨림이 없는 단색 알고리즘	
	//4. IWICPalette*  : The palette to use for conversation.
	//5. double : the alpha threshold to use for conversation.
	//6. WICBitmapPaletteType : the palette translation type to use for conversation.
	//   WICBitmapPaletteTypeCustom -> An arbitrary custom palette provided by caller.
	m_pwicFormatConverter->Initialize(pwicFrameDecode, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.0f, WICBitmapPaletteTypeCustom);
	
	//D2D1_BITMAPSOURCE_PROP_WIC_BITMAP_SOURCE : The IWICBitmapSource containing loaded. The type is IWICBitmapSource.
	m_pd2dfxBitmapSource->SetValue(D2D1_BITMAPSOURCE_PROP_WIC_BITMAP_SOURCE,m_pwicFormatConverter);
	
	D2D1_VECTOR_2F	vec{ 1.1f,1.6f };
	m_pd2dfxBitmapSource->SetValue(D2D1_BITMAPSOURCE_PROP_SCALE, vec);

	//ScoreBoard
	if(pwicBitmapDecoder) 
		pwicBitmapDecoder->Release();

	if (pwicFrameDecode)
		pwicFrameDecode->Release();
		
}

#endif

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
#ifdef _WITH_DIRECT2D_

	CreateDirect2DRenderTargetViews();
#endif
}

#ifdef _WITH_DIRECT2D_
// D3D12가 스왑체인을 소유한다. 그렇기 때문에 우리가 사용하는 11On12 Device에 백버퍼에 우리가 원하는것을 그리려면 
// 우리는 ID3D12Resource 의 BackBuffer 로 부터 Wrapped Resource of type ID3D11Resource.
// 이것은 ID3D12Resource 와 D3D11 기반의 인터페이스들을 연결해준다.
// 이것은 또한 11On12Device에서도 사용가능하다. 
// 우리는 Wrapped Resource 를 가진 후 부터 우리는 D2D Render Target Surface 를 생성할 수 있다.
// 뿐만 아니라 Load Assets메소드안 에 있는 렌더 타겟도 가능하다.
void CGameFramework::CreateDirect2DRenderTargetViews()
{
	float fxDPI, fyDPI;

	m_pd2dFactory->GetDesktopDpi(&fxDPI, &fyDPI); // 현재 데스크톱에 dots per inch(DPI)를 검색하고 ,각 변수들에 할당해준다.

	//D2D1_BITMAP_PROPERTIES1	 : 
	//1. D2D1_BITMAP_OPTIONS     : 비트맵의 용도 옵션.  EX> D2D1_BITMAP_OPTIONS_TARGET : the bitmap can be used as a device context target.
	//												   EX> D2D1_BITMAP_OPTIONS_CANNOT_DRAW : the bitmap cannot be used an input.
	//2. const D2D1_PIXEL_FORMAT : 비트맵의 픽셀 형식과 알파 모드.  EX> D2D1_ALPHA_MODE_PREMULTIPLIED :  알파값이 미리 곱해진다.
	D2D1_BITMAP_PROPERTIES1 d2dBitmapProperties = D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW, D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED), fxDPI, fxDPI);

	for (UINT i = 0; i < m_nSwapChainBuffers; i++)
	{

		D3D11_RESOURCE_FLAGS d3d11Flags = { D3D11_BIND_RENDER_TARGET };
		//CreateWrappedResource-> 이 함수는 D3D11On12 에서 사용가능한  D3D11 리소스들을 만들어준다.
		m_pd3d11On12Device->CreateWrappedResource(m_ppd3dSwapChainBackBuffers[i], &d3d11Flags, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT, __uuidof(ID3D11Resource), (void**)&m_ppd3d11WrappedBackBuffers[i]);

		IDXGISurface *pdxgiSurface = NULL;
		m_ppd3d11WrappedBackBuffers[i]->QueryInterface(__uuidof(IDXGISurface), (void**)&pdxgiSurface);
		//CreateBitmapFromDxgiSurface() -> DXGI표면에서 대상 표면으로 설정하거나 추가 색상 컨텍스트 정보를 지정할 수 있는 비트맵을 만듭니다. 
		m_pd2dDeviceContext->CreateBitmapFromDxgiSurface(pdxgiSurface, &d2dBitmapProperties, &m_ppd2dRenderTargets[i]);
		if (pdxgiSurface)
		{
			pdxgiSurface->Release();
		}
	}

}

#endif
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
	HRESULT hResult = m_pdxgiSwapChain->GetFullscreenState(&bFullScreenState, NULL);
	if (hResult == E_FAIL)
	{
		return;
	}

	hResult = m_pdxgiSwapChain->SetFullscreenState(!bFullScreenState, NULL);
	if (hResult == E_FAIL)
	{
		return;
	}

	DXGI_MODE_DESC dxgiTargetParameters;
	dxgiTargetParameters.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiTargetParameters.Width = m_nWndClientWidth;
	dxgiTargetParameters.Height = m_nWndClientHeight;
	dxgiTargetParameters.RefreshRate.Numerator = 60;
	dxgiTargetParameters.RefreshRate.Denominator = 1;
	dxgiTargetParameters.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	dxgiTargetParameters.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	m_pdxgiSwapChain->ResizeTarget(&dxgiTargetParameters);

	for (int i = 0; i < m_nSwapChainBuffers; i++)
	{
		if (m_ppd3dSwapChainBackBuffers[i]) m_ppd3dSwapChainBackBuffers[i]->Release();
#ifdef _WITH_DIRECT2D_
		if (m_ppd3d11WrappedBackBuffers[i]) m_ppd3d11WrappedBackBuffers[i]->Release();
		if (m_ppd2dRenderTargets[i]) m_ppd2dRenderTargets[i]->Release();
#endif
	}
	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	m_pdxgiSwapChain->GetDesc(&dxgiSwapChainDesc);
	m_pdxgiSwapChain->ResizeBuffers(m_nSwapChainBuffers, m_nWndClientWidth, m_nWndClientHeight, dxgiSwapChainDesc.BufferDesc.Format, dxgiSwapChainDesc.Flags);

	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();

	CreateRenderTargetViews();

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
				case VK_RETURN:
					if (m_bStart == false)
						m_bStart = true;
					break;
				case VK_F1:
				case VK_F2:
				case VK_F3:
					m_pCamera = m_pPlayer->ChangeCamera((DWORD)(wParam - VK_F1 + 1), m_GameTimer.GetTimeElapsed());
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
				m_pMapToolShader->MakeMapBinaryFile(string("../Resource/Position/Surrounding/MapVer1"));
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
#ifdef _WITH_DIRECT2D_
	//Direct2D
	if (m_pd2dbrBackground) m_pd2dbrBackground->Release();
	if (m_pd2dbrBorder) m_pd2dbrBorder->Release();
	for(UINT i=0;i<m_nNameFont;++i)
	{
		if (m_pdwFont[i]) m_pdwFont[i]->Release();
		if (m_pd2dbrText[i]) m_pd2dbrText[i]->Release();
	}
	delete[]m_pdwFont;
	m_pdwFont = nullptr;
	delete[]m_pd2dbrText;
	m_pd2dbrText = nullptr;
	
	if(m_pdwTextLayout) m_pdwTextLayout->Release();
	
	//Direct11
	if (m_pd2dDeviceContext) m_pd2dDeviceContext->Release();
	if (m_pd2dDevice) m_pd2dDevice->Release();
	if (m_pdWriteFactory) m_pdWriteFactory->Release();
	if (m_pd3d11On12Device) m_pd3d11On12Device->Release();
	if (m_pd3d11DeviceContext) m_pd3d11DeviceContext->Release();
	if (m_pd2dFactory) m_pd2dFactory->Release();

	for (int i = 0; i < m_nSwapChainBuffers; i++)
	{
		if (m_ppd3d11WrappedBackBuffers[i]) m_ppd3d11WrappedBackBuffers[i]->Release();
		if (m_ppd2dRenderTargets[i]) m_ppd2dRenderTargets[i]->Release();
	}

	if (m_pd2dfxBitmapSource) m_pd2dfxBitmapSource->Release();
	if (m_pd2dsbDrawingState) m_pd2dsbDrawingState->Release();
	if (m_pwicFormatConverter) m_pwicFormatConverter->Release();
	if (m_pwicImagingFactory) m_pwicImagingFactory->Release();

#endif
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

bool CGameFramework::BuildObjects()
{
	m_pd3dCommandList->Reset(m_pd3dCommandAllocator, NULL);

	//m_pNetwork = new Network;
	//if (!m_pNetwork->Initialize())
	//{
	//	OnDestroy();
	//	::PostQuitMessage(0);
	//	return false;
	//}
	m_pLoadingScene = new CLoadingScene();
	if (m_pLoadingScene)
		m_pLoadingScene->BuildObjects(m_pd3dDevice, m_pd3dCommandList);

	////////////////////////////////////////////////////////////////////////////
	const int nPlayerCount = 6;		//임시로 플레이어 개수 지정. 
	//추후에 서버에서 접속 인원을 씬 BuildObject 호출 전에 받아와서 세팅하면 될듯함.
	m_pScene = new CScene();
	if (m_pScene) 
		m_pScene->BuildObjects(m_pd3dDevice, m_pd3dCommandList,nPlayerCount-1); //GameFramework에서 관리하는 CPlayer를 제외한 나머지 넘겨준다.

	CTerrainPlayer* pPlayer{ nullptr };

	if (m_pScene->getShaderManager())
	{
		map<string, CShader*> m = m_pScene->getShaderManager()->getShaderMap();
		auto iter = m.find("Terrain");
		if(iter != m.end())
		{
			CTerrain* pTerrain = dynamic_cast<CTerrainShader*>((*iter).second)->getTerrain();
			pPlayer = new CTerrainPlayer(m_pd3dDevice, m_pd3dCommandList, m_pScene->GetGraphicsRootSignature(),CGameObject::MATERIALTYPE::PANDA,pTerrain);		
			pPlayer->SetPosition(XMFLOAT3(40.f, pTerrain->GetHeight(0.f, 0.f), 40.f));
			pPlayer->SetScale(XMFLOAT3(10.0f, 10.0f, 10.0f));
			pPlayer->SetPlayerName(L"사명진");

			pPlayer->setScore(100);

			map<string, Bounds*> BoundMap = m_pScene->getShaderManager()->getResourceManager()->getBoundMap();
			auto iter2 = BoundMap.find("EvilBear");
			if (iter2 != BoundMap.end())
				pPlayer->SetOOBB((*iter2).second->m_xmf3Center, (*iter2).second->m_xmf3Extent, XMFLOAT4(0, 0, 0, 1));
			//pPlayer->SetOOBB(XMFLOAT3(-0.1304445, 0.003544204, -7.450581E-09), XMFLOAT3(0.2756854, 0.1529771, 0.2030513), XMFLOAT4(0, 0, 0, 1));
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

	return true;
}

void CGameFramework::ReleaseObjects()
{
	if (m_pNetwork)
		delete m_pNetwork;

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
			if (m_pPlayer->m_pAnimationController->GetAnimationState() != CAnimationController::ICE)
			{	
				dwDirection |= DIR_FORWARD;
				m_pPlayer->SetDirection(dwDirection);
			}
			
		}
		if (pKeysBuffer[VK_DOWN] & 0xF0)
		{
			if (m_pPlayer->m_pAnimationController->GetAnimationState() != CAnimationController::ICE)
			{
				dwDirection |= DIR_BACKWARD;
				m_pPlayer->SetDirection(dwDirection);
			}
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
		m_pScene->AnimateObjects(m_pd3dCommandList,fTimeElapsed);


	
	m_pPlayer->Animate(fTimeElapsed);
	m_pPlayer->UpdateTransform(NULL);
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


	//카툰 렌더링 해야할 쉐이더들은 PreRender에서 그린다.
	if (m_pScene)
	{
		m_pScene->PreRender(m_pd3dCommandList, m_GameTimer.GetTimeElapsed(),m_pCamera);
	}

#ifdef _MAPTOOL_MODE_
	if (m_pMapToolShader)
	{
		m_pMapToolShader->Render(m_pd3dCommandList, m_pCamera,0);
	}
		
#endif

#ifdef _WITH_PLAYER_TOP
	m_pd3dCommandList->ClearDepthStencilView(d3dDsvCPUDescriptorHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);
#endif

	if (m_pPlayer)
		m_pPlayer->Render(m_pd3dCommandList, m_pCamera, GameObject);

	if (m_pScene)
		m_pScene->CheckObjectByObjectCollisions();

	if (m_pCartoonShader)
	{
		m_pCartoonShader->SobelFilter(m_pd3dCommandList, m_ppd3dSwapChainBackBuffers[m_nSwapChainBufferIndex], m_ppd3dCartoonScreenRenderTargetBuffers, m_pCamera);
		m_pCartoonShader->Render(m_pd3dCommandList, m_ppd3dSwapChainBackBuffers[m_nSwapChainBufferIndex], m_ppd3dCartoonScreenRenderTargetBuffers, m_pCamera);
	}

	//카툰 렌더링 하지 않고 그려야할 쉐이더는 PostRender에서 그린다.
	if(m_pScene->getShaderManager())
	{
		m_pd3dCommandList->ClearDepthStencilView(d3dDsvDepthStencilBufferCPUHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);
		m_pd3dCommandList->OMSetRenderTargets(1, &m_pd3dRtvSwapChainBackBufferCPUHandles[m_nSwapChainBufferIndex], TRUE, &d3dDsvDepthStencilBufferCPUHandle);
		m_pScene->PostRender(m_pd3dCommandList, m_GameTimer.GetTimeElapsed(), m_pCamera);
	}
	
	if(m_pLoadingScene && m_bStart == false)
	{
		m_pLoadingScene->Render(m_pd3dCommandList);
	}

	//Direct2D를 사용하면 스왑체인 버퍼 리소스 전이를 Present로 바꿔주면 안된다. 
#ifndef _WITH_DIRECT2D_
	::SynchronizeResourceTransition(m_pd3dCommandList, m_ppd3dSwapChainBackBuffers[m_nSwapChainBufferIndex], D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
#endif

	hResult = m_pd3dCommandList->Close();
	
	ID3D12CommandList *ppd3dCommandLists[] = { m_pd3dCommandList };
	m_pd3dCommandQueue->ExecuteCommandLists(1, ppd3dCommandLists);
	
	WaitForGpuComplete();

#ifdef _WITH_DIRECT2D_
	//AcquireWrappedResources() D3D11On12 디바이스에서 사용될 수 있는 D3D11 리소스들을 얻게해준다.
	//이 함수는 렌더링 할 Wrapped Resource 들을 다시 사용할 수 있다고 암시해준다. 
	m_pd3d11On12Device->AcquireWrappedResources(&m_ppd3d11WrappedBackBuffers[m_nSwapChainBufferIndex], 1);
	
	//Direct2D 디바이스가 렌더 할 비트맵이나 커맨드 리스트를 설정한다.
	m_pd2dDeviceContext->SetTarget(m_ppd2dRenderTargets[m_nSwapChainBufferIndex]);

	m_pd2dDeviceContext->BeginDraw();

	// Apply the rotation transform to the render target
	m_pd2dDeviceContext->SetTransform(D2D1::Matrix3x2F::Identity());

	//D2D1_SIZE_F : float형태의 가로 세로 쌍을 저장한 구조체
	D2D1_SIZE_F szRenderTarget = m_ppd2dRenderTargets[m_nSwapChainBufferIndex]->GetSize();
	
	D2D1_RECT_F rcText{0,0,0,0};
	if (m_pd2dfxBitmapSource && GetAsyncKeyState(VK_TAB) & 0x8000)
	{
		m_pd2dDeviceContext->DrawImage(m_pd2dfxBitmapSource);
		D2D1_RECT_F rcText = D2D1::RectF(0, 0, 240.f, 360.f);
		
		// 이름
		rcText = D2D1::RectF(0, 0, /*szRenderTarget.width * 0.2f*/ 1150.0f,/* szRenderTarget.height * 0.45f*/360.0f);		
		m_pd2dDeviceContext->DrawTextW(m_pPlayer->GetPlayerName(), (UINT32)wcslen(m_pPlayer->GetPlayerName()), m_pdwFont[0], &rcText, m_pd2dbrText[0]);

		// Score
		rcText = D2D1::RectF(0, 0, 1900.0f, 360.0f);
		m_pd2dDeviceContext->DrawTextW((to_wstring(m_pPlayer->getScore())).c_str(), (UINT32)(to_wstring(m_pPlayer->getScore())).length(), m_pdwFont[0], &rcText, m_pd2dbrText[0]);

		rcText = D2D1::RectF(0, 0, /*szRenderTarget.width * 0.2f*/ 1150.0f,/* szRenderTarget.height * 0.45f*/515.0f);
		m_pd2dDeviceContext->DrawTextW(L"이우상", (UINT32)wcslen(L"이우상"), m_pdwFont[1], &rcText, m_pd2dbrText[1]);
	
		rcText = D2D1::RectF(0, 0, /*szRenderTarget.width * 0.2f*/ 1150.0f,/* szRenderTarget.height * 0.45f*/670.0f);
		m_pd2dDeviceContext->DrawTextW(L"가나다", (UINT32)wcslen(L"가나다"), m_pdwFont[2], &rcText, m_pd2dbrText[2]);
	
		rcText = D2D1::RectF(0, 0, /*szRenderTarget.width * 0.2f*/ 1150.0f,/* szRenderTarget.height * 0.45f*/825.0f);
		m_pd2dDeviceContext->DrawTextW(L"라마바", (UINT32)wcslen(L"라마바"), m_pdwFont[3], &rcText, m_pd2dbrText[3]);

		rcText = D2D1::RectF(0, 0, /*szRenderTarget.width * 0.2f*/ 1150.0f,/* szRenderTarget.height * 0.45f*/980.0f);
		m_pd2dDeviceContext->DrawTextW(L"사아자", (UINT32)wcslen(L"사아자"), m_pdwFont[4], &rcText, m_pd2dbrText[4]);
	
		//cout << index << endl;
		rcText = D2D1::RectF(0, 0, /*szRenderTarget.width * 0.2f*/ 1150.0f,/* szRenderTarget.height * 0.45f*/1135.0f);
		m_pd2dDeviceContext->DrawTextW(L"타파하", (UINT32)wcslen(L"타파하"), m_pdwFont[5], &rcText, m_pd2dbrText[5]);
	}
	////////////////////////////////////////////////////////////////////////
	//WITH_DIRECT2D_IMAGE_EFFECT

	////////////////////////////////////////////////////////////////////////

	m_GameTimer.GetFrameRate(m_pszFrameRate + 12, 37);
	D2D1_RECT_F rcUpperText = D2D1::RectF(0, 0, 200.0f, 1400.0f);
	
	//IDWriteTextFormat (interface) : 텍스트 형식에 사용되는 폰트를 서술함. 
	m_pd2dDeviceContext->DrawTextW(m_pszFrameRate, (UINT32)wcslen(m_pszFrameRate), m_pdwFont[0], &rcUpperText, m_pd2dbrText[0]);
	m_pd2dDeviceContext->EndDraw();

	//Releases D3D11 resources that were wrapped for D3D 11on12
	m_pd3d11On12Device->ReleaseWrappedResources(&m_ppd3d11WrappedBackBuffers[m_nSwapChainBufferIndex], 1);

	//,커맨드 버퍼에 대기중인 커맨드를 gpu로 전송
	m_pd3d11DeviceContext->Flush();

#endif

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

