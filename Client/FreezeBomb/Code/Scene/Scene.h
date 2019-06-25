#pragma once

#include "../GameObject/GameObject.h"

#define MAX_LIGHTS						16 
#define POINT_LIGHT						1
#define SPOT_LIGHT						2
#define DIRECTIONAL_LIGHT			3


struct FOGFACTORS
{
	float gFogMode;
	float gFogStart;;
	float gFogRange;
	float gFogDensity;
};

struct FOG
{
	XMFLOAT4 FogColor;
	FOGFACTORS FogFactor;
};

struct LIGHT
{
	XMFLOAT4	m_xmf4Ambient;
	XMFLOAT4	m_xmf4Diffuse;
	XMFLOAT4	m_xmf4Specular;
	XMFLOAT3	m_xmf3Position;
	float 				m_fFalloff;
	XMFLOAT3	m_xmf3Direction;
	float 				m_fTheta; //cos(m_fTheta)
	XMFLOAT3	m_xmf3Attenuation;
	float				m_fPhi;		//cos(m_fPhi)
	bool				m_bEnable;
	int					m_nType;
	float				m_fRange;
	float				padding;
};										
										
struct LIGHTS							
{										
	LIGHT				m_pLights[MAX_LIGHTS];
	XMFLOAT4			m_xmf4GlobalAmbient;
	int						m_nLights;
};

class CPlayer;
class CShaderManager;
class CSoundSystem;
class CScene
{
public:
    CScene();
    ~CScene();
	
	void CreateSoundSystem();
	void PlayIceBreakEffect(bool& bBreak);
	void PlayGetItemEffect();

	bool GetBackgroundMusicOn() { return m_musicStart; }
	void SetBackgroundMusicOn(bool bStart);
	void PlayBackgroundMusic();

	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();

	void BuildFog();
	void BuildDefaultLightsAndMaterials();
	void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList,const int& playerCount);
	void ReleaseObjects();

	ID3D12RootSignature *CreateGraphicsRootSignature(ID3D12Device *pd3dDevice);
	ID3D12RootSignature *GetGraphicsRootSignature() { return(m_pd3dGraphicsRootSignature); }

	bool ProcessInput(UCHAR *pKeysBuffer);
    void AnimateObjects(ID3D12GraphicsCommandList *pd3dCommandList,float fTimeElapsed);
    void PreRender(ID3D12GraphicsCommandList *pd3dCommandList,float fTimeElapsed, CCamera *pCamera=NULL);
	void PostRender(ID3D12GraphicsCommandList *pd3dCommandList,float fTimeElapsed, CCamera *pCamera=NULL);

	//네임 카드를 위해서는 월드 좌표계에서 화면좌표계로 대응되는 위치벡터가 필요하다
	//XMFLOAT3& CalculateNDCSpace();
	XMFLOAT2 ProcessNameCard(const int& objNum);

	void ReleaseUploadBuffers();

	void setPlayer(CPlayer* pPlayer) { m_pPlayer = pPlayer; }
	void SetPlayerCount(UINT nPlayerCount) { m_playerCount = nPlayerCount;}

	UINT GetPlayerCount() { return m_playerCount; }

	CShaderManager* getShaderManager()	const { return m_pShaderManager; }
	void CheckObjectByObjectCollisions(float elapsedTime);

	bool DistanceToTarget(XMFLOAT3& pos);
	void CheckWarningTimer();
	void SetWarningTimer();
	void StopWarningTimer();

	XMFLOAT2 ScreenPosition(int x, int y);

	enum MucicEnum
	{
		BACKGROUNDMUSIC = 0,
		TIMERWARNING,
		ICEBREAK,
		ITEMGET,
		MENU_INPUT
	};

protected:
	ID3D12RootSignature*						m_pd3dGraphicsRootSignature = NULL;

	static ID3D12DescriptorHeap*			m_pd3dCbvSrvDescriptorHeap;

	static D3D12_CPU_DESCRIPTOR_HANDLE	m_d3dCbvCPUDescriptorStartHandle;
	static D3D12_GPU_DESCRIPTOR_HANDLE	m_d3dCbvGPUDescriptorStartHandle;
	static D3D12_CPU_DESCRIPTOR_HANDLE	m_d3dSrvCPUDescriptorStartHandle;
	static D3D12_GPU_DESCRIPTOR_HANDLE	m_d3dSrvGPUDescriptorStartHandle;

	static D3D12_CPU_DESCRIPTOR_HANDLE	m_d3dCbvCPUDescriptorNextHandle;
	static D3D12_GPU_DESCRIPTOR_HANDLE	m_d3dCbvGPUDescriptorNextHandle;
	static D3D12_CPU_DESCRIPTOR_HANDLE	m_d3dSrvCPUDescriptorNextHandle;
	static D3D12_GPU_DESCRIPTOR_HANDLE	m_d3dSrvGPUDescriptorNextHandle;

public:
	static void CreateCbvSrvDescriptorHeaps(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, int nConstantBufferViews, int nShaderResourceViews);

	static D3D12_GPU_DESCRIPTOR_HANDLE CreateConstantBufferViews(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, int nConstantBufferViews, ID3D12Resource *pd3dConstantBuffers, UINT nStride);
	static D3D12_GPU_DESCRIPTOR_HANDLE CreateShaderResourceViews(ID3D12Device *pd3dDevice, CTexture *pTexture, UINT nRootParameter, bool bAutoIncrement);

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUCbvDescriptorStartHandle() { return(m_d3dCbvCPUDescriptorStartHandle); }
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUCbvDescriptorStartHandle() { return(m_d3dCbvGPUDescriptorStartHandle); }
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUSrvDescriptorStartHandle() { return(m_d3dSrvCPUDescriptorStartHandle); }
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUSrvDescriptorStartHandle() { return(m_d3dSrvGPUDescriptorStartHandle); }

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUCbvDescriptorNextHandle() { return(m_d3dCbvCPUDescriptorNextHandle); }
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUCbvDescriptorNextHandle() { return(m_d3dCbvGPUDescriptorNextHandle); }
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUSrvDescriptorNextHandle() { return(m_d3dSrvCPUDescriptorNextHandle); }
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUSrvDescriptorNextHandle() { return(m_d3dSrvGPUDescriptorNextHandle); }

	FOG*						m_pFog = NULL;

	ID3D12Resource				*m_pd3dcbFog = NULL;
	FOG							*m_pcbMappedFog = NULL;

	LIGHT*						m_pLights = NULL;
	int								m_nLights = 0;

	XMFLOAT4					m_xmf4GlobalAmbient;

	ID3D12Resource*				m_pd3dcbLights = NULL;
	LIGHTS*						m_pcbMappedLights = NULL;

	//FMOD 사운드 시스템
	//씬마다 음악이 달라져야 할수 있기 때문에 씬이 사운드를 관리함.
	CSoundSystem*		m_pSound=NULL;

	const char**			m_musicList = NULL;
	int							m_musicCount;

	bool						m_musicStart{ false };
	CPlayer*					m_pPlayer = NULL;

	float						m_bVibeTime{ 0.0f };
	bool						m_bWarningSet{ false };		//경고음 울리는지 여부 

	//게임 플레이어 수
	UINT						m_playerCount;

	CShaderManager*	m_pShaderManager{ nullptr };

	// [ ShaderResourceView 총 개수 ]
	// SkyBox : 1
	// Terrain : 3, 
	// DeadTrees : 15, PineTrees : 34, Rocks : 14, Deer : 2, Pond : 2, FirePit : 3 ,MagicRing=> MapObjects : 71
	// Hammer : 3, Pocket_Watch : 5 => Item = 8
	// EvilBear : 18, LampParticle : 1 => 19
	// Player : 18, LampParticle : 1 => 19
	// BombParticle => 1
	// CubeParticle : 1
	// Snow : 1, 
	// Number : 10, Colon : 1 => TimerUI = 11
	// ItemBox = 1, Hammer_Item : 1, GoldHammer_Item : 1, GoldTimer_Item : 1=> ItemUI : 4
	// MenuUI : 1, Menu_ICON : 1, Option : 1, GameOver : 1, Sound : 1, Cartoon : 1 => 6
	enum nShaderResourceView
	{
		SkyBox = 1, Terrain = 2, MapObjects = 71, Item = 8,
		EvilBear = 19, Player = 19, BombParticle = 1, CubeParticle = 1, Snow = 1, TimerUI = 11, ItemUI = 4, MenuUI = 6,
		Thunder = 2
	};

	// 술래 변경할 수 있는 시간
	static float m_TaggerCoolTime;

	static bool m_IsPlay;
};