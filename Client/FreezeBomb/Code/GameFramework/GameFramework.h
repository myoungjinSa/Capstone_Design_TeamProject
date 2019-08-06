#pragma once
#include <chrono>
#include "../GameTimer/GameTimer.h"
#include "../Network/Network.h"

enum GAMESTATE { ID_INPUT,LOBBY, INGAME, PAUSE, OPTION, CONNECT, LOGIN, LOADING };

// 전체모드할경우 CharacterSelectUIShader에서도 
// FULLSCREENMODE 여부를 확인해야해서 CPP말고 헤더로 옮김 - 명진
#define FullScreenMode
class CScene;
class CPlayer;
class CCamera;
class CMapToolShader;
class CSobelCartoonShader;
class CLoadingScene;
class CLobbyScene;
class ChattingSystem;
class CIPScene;
class CLoginScene;

struct clientsInfo;

class CGameFramework
{
public:
	CGameFramework();
	~CGameFramework();

	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);
	void OnDestroy();

	void CreateSwapChain();

	void CreateDirect3DDevice();
	void CreateCommandQueueAndList();
	void CreateLoadingCommandList();

	void CreateRtvAndDsvDescriptorHeaps();

	void CreateRenderTargetViews();

#ifdef _WITH_DIRECT2D_
	void CreateDirect2DRenderTargetViews();
	void CreateDirect2DDevice();
	
	static  ID2D1DeviceContext2*	GetDeviceContext() { return m_pd2dDeviceContext; }
	static  IWICImagingFactory* GetWICImagingFactory() { return m_pwicImagingFactory; };
	static IDWriteFactory5* GetWriteFactory() { return m_pdWriteFactory; }
	static const map<int, clientsInfo>& GetClientsInfo() { return m_mapClients; }
#ifdef _WITH_SERVER_
	static char GetHostID() { return m_HostID; }
#endif
	void Initialize_BitmapImage();
	void Initialize_GameFont();

	void SetNamecard();
	
	void ProcessDirect2D();
#endif
	void CreateDepthStencilView();
	
#ifdef _WITH_SERVER_
	//Network*getNetwork() { return &m_Network; }
	CLoginScene* GetLoginScene()const { return m_pLoginScene; }
	void ProcessPacket(char *ptr);
	void ResetAnimationForRoundStart();
	//void InitializeIPSystem();
	void ProcessLogin();
	void ShowCurrentBomber();
#endif
	void ChangeSwapChainState();

	bool BuildObjects();
	void ReleaseObjects();

	void ProcessInput();
	void AnimateObjects();
	void FrameAdvance();
	void ProcessInGame(D3D12_CPU_DESCRIPTOR_HANDLE&);
	void ProcessLobby();
	void WaitForGpuComplete();
	void MoveToNextFrame();
	
	void CreateOffScreenRenderTargetViews();

	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	bool IsHangeul() { return m_bHangeul; }
	void SetHangeul(bool han) { m_bHangeul = han; }

	//얼음으로 변해도 되는 시간인지 검사하는 함수
	void CheckIceChangeOk();

#ifdef _MAPTOOL_MODE_
	void OnMapToolInputMesseage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
#endif	

	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	bool IsInGame(); 
public:
	static enum STAGE {ROUND_1,ROUND_2,ROUND_3};
	std::chrono::steady_clock::time_point m_RoundStartTime;
	std::chrono::steady_clock::time_point m_RoundCountTime;
	
	//ice로 변하는게 가능한지 여부의 bool변수
	static bool m_IceChangeOk;
private:
	HINSTANCE						m_hInstance;
	HWND							m_hWnd;
	
	bool							m_bTearingMode{ false };
	bool							m_WindowMode{ false };
	
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
	ID3D12DescriptorHeap*				m_pd3dRtvDescriptorHeap = nullptr;
	UINT												m_nRtvDescriptorIncrementSize;
	D3D12_CPU_DESCRIPTOR_HANDLE				m_pd3dRtvSwapChainBackBufferCPUHandles[m_nSwapChainBuffers];

	ID3D12Resource*								m_pd3dDepthStencilBuffer = nullptr;
	ID3D12DescriptorHeap*					m_pd3dDsvDescriptorHeap = nullptr;
	UINT													m_nDsvDescriptorIncrementSize;

	ID3D12CommandAllocator*					m_pd3dCommandAllocator = nullptr;
	ID3D12CommandQueue*						m_pd3dCommandQueue = nullptr;
	ID3D12GraphicsCommandList*				m_pd3dCommandList = nullptr;

	ID3D12CommandAllocator*					m_pLoadingCommandAllocator = nullptr;
	ID3D12GraphicsCommandList*				m_pLoadingCommandList = nullptr;

	//DXGI_MODE_DESC *						m_pDisplayMode;
	ID3D12Fence*							m_pd3dFence = nullptr;
	UINT64										m_nFenceValues[m_nSwapChainBuffers];
	HANDLE									m_hFenceEvent;

#if defined(_DEBUG)
	ID3D12Debug*							m_pd3dDebugController;
#endif
	CGameTimer								m_GameTimer;
	float m_elapsedTime = 0.f;

	CLoadingScene*							m_pLoadingScene = nullptr;
	CLobbyScene*							m_pLobbyScene = nullptr;
	CScene*									m_pScene = nullptr;
	CPlayer*								m_pPlayer = nullptr;

	CCamera*								m_pCamera = nullptr;

	POINT									m_ptOldCursorPos;

	_TCHAR									m_pszFrameRate[70];

	static const UINT						m_nCartoonScreenRenderTargetBuffers = 2;
	ID3D12Resource*							m_ppd3dCartoonScreenRenderTargetBuffers[m_nCartoonScreenRenderTargetBuffers];
	D3D12_CPU_DESCRIPTOR_HANDLE				m_pd3dCarttonScreenRenderTargetBufferCPUHandles[m_nCartoonScreenRenderTargetBuffers];

	CSobelCartoonShader*		m_pCartoonShader{ nullptr };
	static map<int, clientsInfo>	m_mapClients;

	

#ifdef _WITH_DIRECT2D_
	ID3D11On12Device				*m_pd3d11On12Device{ nullptr };//
	ID3D11DeviceContext			*m_pd3d11DeviceContext{ nullptr };//

	ID2D1Factory3						*m_pd2dFactory{ nullptr };//
	ID2D1Device2*					m_pd2dDevice{ nullptr };//

	// Direct2D로 이미지를 그리기 위한 팩토리
	static IWICImagingFactory*			m_pwicImagingFactory;
	// Direct2D로 텍스트를 그리기 위한 팩토리
	static IDWriteFactory5*					m_pdWriteFactory;
	static ID2D1DeviceContext2*			m_pd2dDeviceContext;

	ID3D11Resource				*m_ppd3d11WrappedBackBuffers[m_nSwapChainBuffers];
	ID2D1Bitmap1					*m_ppd2dRenderTargets[m_nSwapChainBuffers];

	//한글인지 영어 인지
	bool m_bHangeul{ false };	
#endif

#ifdef _WITH_SERVER_
	//Network m_Network;
	static char m_HostID;
	int clientCount = 0;
	int m_currentBomberID{ 0 };
	CLoginScene*	m_pLoginScene{ nullptr };
	CIPScene*			m_pIPScene{ nullptr };
#endif
	vector<thread> loadingThread;
	void Worker_Thread();

#ifdef _MAPTOOL_MODE_
	CMapToolShader*  m_pMapToolShader = nullptr;
#endif
};

