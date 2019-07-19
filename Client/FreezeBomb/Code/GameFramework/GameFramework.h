#pragma once

#include "../GameTimer/GameTimer.h"
#include "../Network/Network.h"

enum GAMESTATE { CHARACTER_SELECT, INGAME, PAUSE, OPTION, CONNECT, LOGIN, LOADING };

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

	void Initialize_BitmapImage();
	void Initialize_GameFont();

	void SetNamecard();
	void ShowScoreboard();
	//HRESULT BindDC();
	//HRESULT CreateDCRenderTarget();
	void ShowReadyText();
	void DrawStageInfo();
	void ProcessDirect2D();
#endif
	void CreateDepthStencilView();
	
#ifdef _WITH_SERVER_
	//Network*getNetwork() { return &m_Network; }
	CLoginScene* GetLoginScene()const { return m_pLoginScene; }
	void ProcessPacket(char *ptr);
	void ResetAnimationForRoundStart();

	//void CreateLoginCommandList();
	//void InitializeIPSystem();
	void ProcessLogin();
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

public:

	static enum STAGE {ROUND_1,ROUND_2,ROUND_3};
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

	CSobelCartoonShader*					m_pCartoonShader{ nullptr };
	map<int, clientsInfo>						m_mapClients;

#ifdef _WITH_DIRECT2D_
	ID3D11On12Device				*m_pd3d11On12Device{ nullptr };//
	ID3D11DeviceContext			*m_pd3d11DeviceContext{ nullptr };//
	ID2D1Factory3						*m_pd2dFactory{ nullptr };//

	ID2D1Device2*					m_pd2dDevice{ nullptr };//
	static ID2D1DeviceContext2*		m_pd2dDeviceContext;

	ID3D11Resource				*m_ppd3d11WrappedBackBuffers[m_nSwapChainBuffers];
	ID2D1Bitmap1					*m_ppd2dRenderTargets[m_nSwapChainBuffers];
	
	enum FONT_TYPE { PIOP_FONT, MAPLE_FONT };
	IDWriteFactory5*					m_pdWriteFactory{ nullptr };
	IDWriteFontCollection1*		m_pFontCollection{ nullptr };

	const int								m_FontNum = 2;
	// 폰트 객체
	IDWriteTextFormat**			m_ppFont{ nullptr };
	IDWriteTextLayout**				m_ppTextLayout{ nullptr };

	// 폰트 색상 개수
	enum COLOR_TYPE { PINK, BROWN, WHITE, BLACK, SKYBLUE, PANDA, RED, ORANGE };
	const int								m_FontColorNum = 8;

	// 폰트 색상
	ID2D1SolidColorBrush**		m_ppFontColor{ nullptr };	
	static IWICImagingFactory* m_pwicImagingFactory;

	//한글인지 영어 인지
	bool m_bHangeul{ false };	
#endif

#ifdef _WITH_SERVER_
	//클라이언트 정보
	
	//Network m_Network;
	int hostId;
	int clientCount = 0;

	SC_PACKET_ACCESS_COMPLETE *pAC = NULL;
	SC_PACKET_ACCESS_PLAYER *pAP = NULL;
	SC_PACKET_PUT_PLAYER *pPP = NULL;
	SC_PACKET_MOVE_PLAYER *pMP = NULL;
	SC_PACKET_USE_ITEM *pUI = NULL;
	SC_PACKET_ROUND_END *pRE = NULL;
	SC_PACKET_REMOVE_PLAYER *pRP = NULL;
	SC_PACKET_COMPARE_TIME *pCT = NULL;
	SC_PACKET_ROUND_START *pRS = NULL;
	SC_PACKET_STOP_RUN_ANIM *pSTA = NULL;
	SC_PACKET_PLAYER_ANIMATION* pPA = NULL;
	SC_PACKET_LOBBY_IN *pLI = NULL;
	SC_PACKET_LOBBY_OUT *pLO = NULL;
	SC_PACKET_CHATTING * pCh = NULL;
	SC_PACKET_READY_STATE* pReady = NULL;
	SC_PACKET_UNREADY_STATE* pNotReady = NULL;
	SC_PACKET_COLLIDED* pCollided = NULL;
	SC_PACKET_FREEZE* pFR = NULL;
	SC_PACKET_RELEASE_FREEZE* pRF = NULL;
	SC_PACKET_BOMB_EXPLOSION* pBE = NULL;
	SC_PACKET_ROLE_CHANGE* pRC = NULL;
	SC_PACKET_CHANGE_HOST* pCH = NULL;

	bool isCharacterSelectDone = false;

	CLoginScene*			m_pLoginScene{ nullptr };
	CIPScene*				m_pIPScene{ nullptr };
	vector<thread> loginThread;

#endif
	//사운드 쓰레드 풀
	vector<thread> soundThreads;
	vector<thread> loadingThread;

	void Worker_Thread();

#ifdef _MAPTOOL_MODE_
	CMapToolShader*  m_pMapToolShader = nullptr;
#endif
};

