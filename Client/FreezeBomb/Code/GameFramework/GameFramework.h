#pragma once

#include "../GameTimer/GameTimer.h"
#include "../Network/Network.h"

// 전체모드할경우 CharacterSelectUIShader에서도 
// FULLSCREENMODE 여부를 확인해야해서 CPP말고 헤더로 옮김 - 명진
//#define FullScreenMode
class CScene;
class CPlayer;
class CCamera;
class CMapToolShader;
class CSobelCartoonShader;
class CLoadingScene;
class CLobbyScene;
class ChattingSystem;


struct clientsInfo
{
	int		id;
	bool	isReady;
	_TCHAR	name[256];
};

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
	void SetNamecard();
	void ShowScoreboard();
	//HRESULT BindDC();
	//HRESULT CreateDCRenderTarget();
	void ShowReadyText();

	void ProcessDirect2D();
#endif
	void CreateDepthStencilView();
	
#ifdef _WITH_SERVER_
	//Network*getNetwork() { return &m_Network; }
	void ProcessPacket(char *ptr);
#endif
	void ChangeSwapChainState();

	bool BuildObjects();
	void ReleaseObjects();

	void ProcessInput();
	void AnimateObjects();
	void FrameAdvance();
	void ProcessInGame(D3D12_CPU_DESCRIPTOR_HANDLE&);
	void ProcessLobby();
	void ShowPlayers();
	void WaitForGpuComplete();
	void MoveToNextFrame();

	void CreateOffScreenRenderTargetViews();

	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	bool IsHangeul() { return m_bHangeul; }
	void SetHangeul(bool han) { m_bHangeul = han; }

#ifdef _MAPTOOL_MODE_
	void OnMapToolInputMesseage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
#endif	

	
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool IsInGame(); 
	enum GAMESTATE {CHARACTER_SELECT=0, INGAME,PAUSE,OPTION};
private:
	HINSTANCE						m_hInstance;
	HWND							m_hWnd;
	

	int 							m_nState{ GAMESTATE::INGAME };
	//게임 상태 
	
	//클라이언트 정보
	clientsInfo						m_PlayerInfo[MAX_USER];
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

	ID3D12CommandAllocator*					m_pLoadingCommandAllocator = nullptr;
	ID3D12GraphicsCommandList*				m_pLoadingCommandList = nullptr;

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

	CSobelCartoonShader*					m_pCartoonShader{ nullptr };


#ifdef _WITH_DIRECT2D_
	ID3D11On12Device				*m_pd3d11On12Device{ nullptr };//
	ID3D11DeviceContext				*m_pd3d11DeviceContext{ nullptr };//
	ID2D1Factory3					*m_pd2dFactory{ nullptr };//
	IDWriteFactory					*m_pdWriteFactory{ nullptr };//
	ID2D1Device2					*m_pd2dDevice{ nullptr };//
	ID2D1DeviceContext2				*m_pd2dDeviceContext{ nullptr };//


	ID3D11Resource					*m_ppd3d11WrappedBackBuffers[m_nSwapChainBuffers];//
	ID2D1Bitmap1					*m_ppd2dRenderTargets[m_nSwapChainBuffers];//

	ID2D1SolidColorBrush			*m_pd2dbrBackground{ nullptr };//
	ID2D1SolidColorBrush			*m_pd2dbrBorder{ nullptr };//
	ID2D1SolidColorBrush			*m_pBlackBrush{ nullptr };
	
	IDWriteTextFormat				**m_pdwFont{ nullptr };//
	const int						m_nNameFont=6;
	//const int								m_nNameFont = 7;
	IDWriteTextLayout				*m_pdwTextLayout{ nullptr };//
	ID2D1SolidColorBrush			**m_pd2dbrText{ nullptr };//
	
	
	IWICImagingFactory				*m_pwicImagingFactory{ nullptr };
	ID2D1Effect						*m_pd2dfxBitmapSource{ nullptr };
	ID2D1DrawingStateBlock1			*m_pd2dsbDrawingState{ nullptr };
	IWICFormatConverter				*m_pwicFormatConverter{ nullptr };

	
	//한글인지 영어 인지
	bool m_bHangeul{ false };
#endif

#ifdef _WITH_SERVER_
	//Network m_Network;
	int hostId;
	int clientCount = 0;
	SC_PACKET_ACCESS_COMPLETE *pAC = NULL;
	SC_PACKET_ACCESS_PLAYER *pAP = NULL;
	SC_PACKET_PUT_PLAYER *pPP = NULL;
	SC_PACKET_MOVE_PLAYER *pMP = NULL;
	SC_PACKET_USE_ITEM *pUI = NULL;
	SC_PACKET_ROLL_CHANGE *pRC = NULL;
	SC_PACKET_ROUND_END *pRE = NULL;
	SC_PACKET_REMOVE_PLAYER *pRP = NULL;
	SC_PACKET_COMPARE_TIME *pCT = NULL;
	SC_PACKET_ROUND_START *pRS = NULL;
	SC_PACKET_STOP_RUN_ANIM *pSTA = NULL;
	SC_PACKET_PLAYER_ANIMATION* pPA = NULL;
	bool isCharacterSelectDone = false;
#endif
	//사운드 쓰레드 풀
	vector<thread> soundThreads;
	vector<thread> loadingThread;

	void Worker_Thread();

#ifdef _MAPTOOL_MODE_
	CMapToolShader*  m_pMapToolShader = nullptr;
#endif
};

