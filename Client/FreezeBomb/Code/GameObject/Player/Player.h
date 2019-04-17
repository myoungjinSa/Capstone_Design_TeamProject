#pragma once

#define DIR_FORWARD		0x01
#define DIR_BACKWARD		0x02
#define DIR_LEFT					0x04
#define DIR_RIGHT				0x08
#define DIR_UP					0x10
#define DIR_DOWN				0x20

#include "../GameObject.h"
#include "../../Camera/Camera.h"

class CItem;
class CShadow;
class CShaderManager;
class CBomb;
class CPlayer : public CGameObject
{
public:
	CPlayer();
	virtual ~CPlayer();

	XMFLOAT3 GetPosition() { return(m_xmf3Position); }
	XMFLOAT3 GetLookVector() { return(m_xmf3Look); }
	XMFLOAT3 GetUpVector() { return(m_xmf3Up); }
	XMFLOAT3 GetRightVector() { return(m_xmf3Right); }
	DWORD    GetDirection() { return m_dwDirection; }

	_TCHAR* GetPlayerName() { return m_playerName; }

	void SetPlayerName(const _TCHAR* nPlayerName) { wcscpy_s(m_playerName, nPlayerName); }

	void SetDirection(DWORD direction) { m_dwDirection = direction; }
	void SetFriction(float fFriction) { m_fFriction = fFriction; }
	void SetGravity(const XMFLOAT3& xmf3Gravity) { m_xmf3Gravity = xmf3Gravity; }
	void SetMaxVelocityXZ(float fMaxVelocity) { m_fMaxVelocityXZ = fMaxVelocity; }
	void SetMaxVelocityY(float fMaxVelocity) { m_fMaxVelocityY = fMaxVelocity; }

	float GetVelocityX() { return m_xmf3Velocity.x; }
	float GetVelocityZ() { return m_xmf3Velocity.z; }

	void SetVelocity(const XMFLOAT3& xmf3Velocity) { m_xmf3Velocity = xmf3Velocity; }
	void SetVelocity(const float& velX, const float& velY, const float& velZ) { m_xmf3Velocity.x = velX; m_xmf3Velocity.y = velY, m_xmf3Velocity.z = velZ; }
	void SetPosition(const XMFLOAT3& xmf3Position) { Move(XMFLOAT3(xmf3Position.x - m_xmf3Position.x, xmf3Position.y - m_xmf3Position.y, xmf3Position.z - m_xmf3Position.z), false); }

	//사운드 볼륨 조절을 위해 가장 가까운 적과의 거리를 멤버 변수로 저장한다.
	void SetMinDistanceWithEnemy(float fDistance) { m_fMinDistance = fDistance; }
	void SetScale(XMFLOAT3& xmf3Scale) { m_xmf3Scale = xmf3Scale; }

	const XMFLOAT3& GetVelocity() const { return(m_xmf3Velocity); }
	float GetYaw() const { return(m_fYaw); }
	float GetPitch() const { return(m_fPitch); }
	float GetRoll() const { return(m_fRoll); }
	float GetMaxVelocity() const { return m_fMaxVelocityXZ; }

	CCamera *GetCamera() { return(m_pCamera); }
	void SetCamera(CCamera *pCamera) { m_pCamera = pCamera; }

	void Move(ULONG nDirection, float fDistance, bool bVelocity = false);
	void Move(const XMFLOAT3& xmf3Shift, bool bVelocity = false);
	void Move(float fxOffset = 0.0f, float fyOffset = 0.0f, float fzOffset = 0.0f);
	void Rotate(float x, float y, float z);

	void Update(float fTimeElapsed);

	virtual void OnPlayerUpdateCallback(float fTimeElapsed) { }
	void SetPlayerUpdatedContext(LPVOID pContext) { m_pPlayerUpdatedContext = pContext; }

	virtual void OnCameraUpdateCallback(float fTimeElapsed) { }
	void SetCameraUpdatedContext(LPVOID pContext) { m_pCameraUpdatedContext = pContext; }

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);

	CCamera *OnChangeCamera(DWORD nNewCameraMode, DWORD nCurrentCameraMode);

	virtual CCamera *ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed) { return nullptr; }
	virtual void OnPrepareRender();
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState);

	void Add_Inventory(string key, int ItemType);
	void Refresh_Inventory(int ItemType);
	void DecideAnimationState(float fLength);
	bool AnimationCollision(byte AnimationType);

	CItem* getItem()	const { return m_pItem; }
	const map<string, CItem*> get_Special_Inventory()	const { return m_Special_Inventory; }
	size_t get_Normal_InventorySize() const { return m_Normal_Inventory.size(); }
	size_t get_Special_InventorySize() const { return m_Special_Inventory.size(); }

	CShadow*	getShadow()		const { return m_pShadow; }
	
	void setShaderManager(CShaderManager* pShaderManager) { m_pShaderManager = pShaderManager; }

	void InitializeSound();
	void ReleaseSound();

	void* GetSoundData() const { return static_cast<void*>(m_pSound); }
	
	DWORD				m_dwDirection = 0x00;

	enum MUSIC_ENUM
	{
		FOOTSTEP=1,
		USETIMER,
		DIE,
		ATTACK
	};
	std::map<MUSIC_ENUM, std::string> m_mapMusicList;

	void setScore(short score) { m_Score = score; }
	short getScore()		const { return m_Score; }

protected:

	_TCHAR				m_playerName[256];

	XMFLOAT3			m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3			m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	XMFLOAT3			m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMFLOAT3			m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);

	XMFLOAT3			m_xmf3Scale = XMFLOAT3(1.0f, 1.0f, 1.0f);

	float           			m_fPitch = 0.0f;
	float           			m_fYaw = 0.0f;
	float           			m_fRoll = 0.0f;

	XMFLOAT3			m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3     		m_xmf3Gravity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	float           			m_fMaxVelocityXZ = 0.0f;
	float           			m_fMaxVelocityY = 0.0f;
	float           			m_fFriction = 0.0f;

	float						m_fMinDistance = 0.0f;

	LPVOID				m_pPlayerUpdatedContext{ nullptr };
	LPVOID				m_pCameraUpdatedContext{ nullptr };

	CCamera*				m_pCamera{ nullptr };

	CItem*							m_pItem{ nullptr };
	map<string, CItem*>	m_Normal_Inventory;
	map<string, CItem*>	m_Special_Inventory;

	CShadow*	m_pShadow{ nullptr };

	CShaderManager* m_pShaderManager{ nullptr };

	CSoundSystem*	m_pSound{ nullptr };
	const char**	m_SoundList;
	int					m_SoundCount;

	short				m_Score = 0;
	float				m_Time = 0.f;

	CBomb	*		m_BombParticle{ nullptr };
};

class CSoundSystem;

class CTerrainPlayer : public CPlayer
{
	//카메라 무빙
public:
	CTerrainPlayer(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature,int matID, void *pContext=NULL);
	virtual ~CTerrainPlayer();

	virtual void Animate(float fTimeElapsed);

	virtual CCamera *ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed);
	virtual void OnPlayerUpdateCallback(float fTimeElapsed);
	virtual void OnCameraUpdateCallback(float fTimeElapsed);


	void RotateAxisY(float fTimeElapsed);


};

