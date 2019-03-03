#pragma once

#include "../GameTimer/GameTimer.h"


class CScene;
class CPlayer;
class CPlayerShadowShader;

class CCamera;
class CMapToolShader;
class CSobelCartoonShader;
class CDirectSound;

class CGameFramework
{
public:
	CGameFramework();
	~CGameFramework();

	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);
	void OnDestroy();

	void CreateSwapChain();
	void CreateDirect11DeviceOn12();
	void CreateDirect3DDevice();
	void CreateCommandQueueAndList();

	void CreateRtvAndDsvDescriptorHeaps();

	void Create2DRenderTarget();
	void CreateRenderTargetViews();
	void CreateDepthStencilView();
	

	void ChangeSwapChainState();

	void BuildObjects();
	void ReleaseObjects();

	void ProcessInput();
	void AnimateObjects();
	void FrameAdvance();

	void WaitForGpuComplete();
	void MoveToNextFrame();

	void CreateOffScreenRenderTargetViews();


	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

#ifdef _MAPTOOL_MODE_
	void OnMapToolInputMesseage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
#endif	

	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

private:


	

	HINSTANCE						m_hInstance;
	HWND							m_hWnd;

	int								m_nWndClientWidth;
	int								m_nWndClientHeight;

	IDXGIFactory4*					m_pdxgiFactory = nullptr;
	IDXGISwapChain3*				m_pdxgiSwapChain = nullptr;
	ID3D12Device*					m_pd3dDevice = nullptr;


	bool							m_bMsaa4xEnable = false;
	UINT							m_nMsaa4xQualityLevels = 0;

	static const UINT				m_nSwapChainBuffers = 2;
	UINT							m_nSwapChainBufferIndex;

	ID3D12Resource*							m_ppd3dSwapChainBackBuffers[m_nSwapChainBuffers];
	ID3D12DescriptorHeap*					m_pd3dRtvDescriptorHeap = nullptr;
	UINT									m_nRtvDescriptorIncrementSize;
	D3D12_CPU_DESCRIPTOR_HANDLE				m_pd3dRtvSwapChainBackBufferCPUHandles[m_nSwapChainBuffers];

	ID3D12Resource*								m_pd3dDepthStencilBuffer = nullptr;
	ID3D12DescriptorHeap*					m_pd3dDsvDescriptorHeap = nullptr;
	UINT													m_nDsvDescriptorIncrementSize;

	ID3D12CommandAllocator*					m_pd3dCommandAllocator = nullptr;
	ID3D12CommandQueue*						m_pd3dCommandQueue = nullptr;
	ID3D12GraphicsCommandList*				m_pd3dCommandList = nullptr;

	ID3D12Fence*							m_pd3dFence = nullptr;
	UINT64										m_nFenceValues[m_nSwapChainBuffers];
	HANDLE									m_hFenceEvent;

#if defined(_DEBUG)
	ID3D12Debug*							m_pd3dDebugController;
#endif

	CGameTimer								m_GameTimer;

	CScene*									m_pScene = nullptr;
	CPlayer*								m_pPlayer = nullptr;
	CPlayerShadowShader*		m_pPlayerShadowShader = nullptr;

	CCamera*								m_pCamera = nullptr;

	POINT									m_ptOldCursorPos;

	_TCHAR									m_pszFrameRate[70];


	static const UINT				m_nCartoonScreenRenderTargetBuffers = 2;
	ID3D12Resource*				m_ppd3dCartoonScreenRenderTargetBuffers[m_nCartoonScreenRenderTargetBuffers];
	D3D12_CPU_DESCRIPTOR_HANDLE m_pd3dCarttonScreenRenderTargetBufferCPUHandles[m_nCartoonScreenRenderTargetBuffers];


	CSobelCartoonShader*		m_pCartoonShader =nullptr;
	bool									m_bCartoon =true;
#ifdef _MAPTOOL_MODE_
	CMapToolShader*  m_pMapToolShader = nullptr;
#endif


	/////////////////////////////////////////////////////////////////////////
	//DirectX 11 
	static const UINT				FrameCount = 3;
	ID3D11Device*					m_d3d11Device = nullptr;
	ID3D11DeviceContext*			m_d3d11DeviceContext=nullptr;
	ID3D11On12Device*				m_d3d11On12Device=nullptr;
	IDWriteFactory*					m_dWriteFactory=nullptr;
	ID2D1Factory3*					m_d2dFactory=nullptr;
	ID2D1Device2*					m_d2dDevice=nullptr;
	ID2D1DeviceContext2*			m_d2dDeviceContext=nullptr;
	ID3D12Resource*					m_renderTargets[FrameCount];
	ID3D11Resource*					m_wrappedBackBuffers[FrameCount];
	ID2D1Bitmap1*					m_d2dRenderTargets[FrameCount];

	ID2D1SolidColorBrush*			m_textBrush=nullptr;
	IDWriteTextFormat*				m_textFormat=nullptr;


};

