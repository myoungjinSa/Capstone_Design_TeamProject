#pragma once

#include "../GameObject/GameObject.h"


#define MAX_LIGHTS						16 

#define POINT_LIGHT						1
#define SPOT_LIGHT						2
#define DIRECTIONAL_LIGHT			3

D3D12_SHADER_RESOURCE_VIEW_DESC GetShaderResourceViewDesc(D3D12_RESOURCE_DESC d3dResourceDesc, UINT nTextureType);

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
	int					m_nLights;
};

class CPlayer;
class CShaderManager;
class CSoundSystem;
class CScene
{
public:
    CScene();
    ~CScene();

	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();

	void BuildDefaultLightsAndMaterials();
	void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList,const int& playerCount);
	void ReleaseObjects();

	ID3D12RootSignature *CreateGraphicsRootSignature(ID3D12Device *pd3dDevice);
	ID3D12RootSignature *GetGraphicsRootSignature() { return(m_pd3dGraphicsRootSignature); }

	bool ProcessInput(UCHAR *pKeysBuffer);
    void AnimateObjects(float fTimeElapsed);
    void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera=NULL);

	void ReleaseUploadBuffers();

	void setPlayer(CPlayer* pPlayer) { m_pPlayer = pPlayer; }
	void SetPlayerCount(UINT nPlayerCount) { m_playerCount = nPlayerCount;}

	UINT GetPlayerCount() { return m_playerCount; }

	CShaderManager* getShaderManager()	const { return m_pShaderManager; }
	void CheckObjectByObjectCollisions();

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

	LIGHT*						m_pLights = NULL;
	int								m_nLights = 0;

	XMFLOAT4					m_xmf4GlobalAmbient;

	ID3D12Resource*				m_pd3dcbLights = NULL;
	LIGHTS*						m_pcbMappedLights = NULL;
	
private:
	CPlayer*					m_pPlayer = NULL;

	//void*						m_SoundBuffer1 = NULL;
	//HANDLE						m_SoundBuffer2 = NULL;
	CSoundSystem*				m_pSound=NULL;
	//FMOD 사운드 시스템
	//씬마다 음악이 달라져야 할수 있기 때문에 씬이 사운드를 관리함.
	
	const char**				m_musicList = NULL;
	int							m_musicCount;

	//게임 플레이어 수
	UINT						m_playerCount;

	CShaderManager*	m_pShaderManager{ nullptr };

	// [ ShaderResourceView 총 개수 ]
	// SkyBox : 1
	// Terrain : 2, 
	// DeadTrees : 23, PineTrees : 35, Rocks : 25, Deer : 2, Pond : 2=> MapObjects : 87
	// Hammer : 3, Pocket_Watch : 2 => Item = 5
	// EvilBear : 15
	// Player : 15 
	// Snow : 1, IceParticle : 1 => Particle = 2 
	// Number : 10, Colon : 1 => TimerUI = 11
	// ItemBox = 1, Hammer_Item : 1, GoldHammer_Item : 1, GoldTimer_Item : 1=> ItemUI : 4

	enum nShaderResourceView
	{
		SkyBox = 1, Terrain = 2, MapObjects = 67, Item = 5,
		EvilBear = 15, Player = 15, Particle = 2, TimerUI = 11, ItemUI = 4
	};
};