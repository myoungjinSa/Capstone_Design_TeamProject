#pragma once

#include "../GameObject.h"
#include "../../Camera/Camera.h"

#define KEY_DOWN(VK_CODE) ((GetAsyncKeyState(VK_CODE) & 0x8000 ? 1 : 0))
#define KEY_UP(VK_CODE) ((GetAsyncKeyState(VK_CODE) & 0x8000 ? 0 : 1))
#define VK_A 0x41

enum EVENT_TYPE { EV_GOLDTIMER, EV_GOLDHAMMER, EV_QUIT };
struct Timer_Event
{
	chrono::high_resolution_clock::time_point start_time;
	EVENT_TYPE event_type;

	// 우선순위 큐에 데이터를 넣을 때의 비교연산자
	constexpr bool operator < (const Timer_Event& _Left) const
	{	
		return ((*this).start_time > _Left.start_time);
	};
};

class CItem;
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

	void SetPlayerID(char id) { m_pPlayerID = id; }
	char GetPlayerID() { return m_pPlayerID; }

	void SetMaterialID(char id) { m_matID = id; }
	char GetMaterialID() { return m_matID; }
	
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
	XMFLOAT3& GetScale() { return m_xmf3Scale; }

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

	void UpdateTransform(XMFLOAT4X4 *pxmf4x4Parent=NULL,bool isLocalFrameRotate = false);
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

	void DecideAnimationState(float fLength,const float& fTimeElapsed);

	bool AnimationCollision(byte AnimationType);

	void Add_Inventory(const string& key,int ItemType);
	void Sub_Inventory(int ItemType);
	void ChangeRound();
	void InventoryClear();

	const map<string, CItem*> get_Special_Inventory()	const { return m_Special_Inventory; }
	size_t get_Normal_InventorySize() const { return m_Normal_Inventory.size(); }
	size_t get_Special_InventorySize() const { return m_Special_Inventory.size(); }
	
	void setShaderManager(CShaderManager* pShaderManager) { m_pShaderManager = pShaderManager; }
	
	DWORD				m_dwDirection = 0x00;

	enum MUSIC_ENUM
	{
		FOOTSTEP=1,
		USETIMER,
		DIE,
		ATTACK,
		ELECTRIC
	};
	std::map<MUSIC_ENUM, std::string> m_mapMusicList;

	void SetMoveRotate(bool mv) { m_isMoveRotate = mv; }
	bool GetMoveRotate() { return m_isMoveRotate; }
	bool IsCameraVibe() const { return m_bCameraVibe; }
	void SetCameraVibe(bool vibe) {  m_bCameraVibe = vibe; }
	bool IsSpike() const { return m_bSpike; }
	void SetSpike(bool spike) { m_bSpike = spike; }

	CBomb* GetBombPtr() { return m_BombParticle; }
	bool CheckInventoryToGet(const int& itemType);		//이미 인벤의 존재하는 종류의 아이템인지 검사하고 없다면 true 반환
	void ChangeRole();
	map<string, CItem*>& GetSpecialInventory() { return m_Special_Inventory; }
	map<string, CItem*>& GetNormalInventory() { return m_Normal_Inventory; }
	
	void SetCollision(bool collision) { m_bCollision = collision; }
	bool GetCollision() { return m_bCollision; }

	void ProcessAnimation();
	void SendAnimation(int animation);
	bool CheckAnimation(int animation);
	void SetAnimation(int animation);
	bool CheckNormalItem();
	int CheckSpecialItem();
	bool CheckICE();

	bool CheckMove(int key);
	void RotateAxisY(int key);
	void ProcessMove();
	bool ControlAnimationOnce();
	void ProcessRoundStart();
	void ProcessRoundEnd();

	void ProcessEvent();
	void AddTimer(chrono::high_resolution_clock::time_point start_t, EVENT_TYPE e_type);
	void DeadReckoning(float elapsedTime);
	int GetPrevAnimation()	const { return m_PrevAnimation; }

	const XMFLOAT2& ConvertIceCoolTimeTextToNDCSpace();

	void SetIceCoolTime(int coolTime) { m_IceCooltime = coolTime; }
	const int GetIceCoolTime() const { return m_IceCooltime; }
	void SetIsShowCoolTime(bool isShow) { m_bShowCoolTime = isShow; }
	bool GetIsShowCoolTime() { return m_bShowCoolTime; }

protected:
	XMFLOAT3					m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3					m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	XMFLOAT3					m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMFLOAT3					m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);

	XMFLOAT3					m_xmf3Scale = XMFLOAT3(1.0f, 1.0f, 1.0f);

	float           			m_fPitch = 0.0f;
	float           			m_fYaw = 0.0f;
	float           			m_fRoll = 0.0f;

	bool						m_bCollision{ false };
	bool						m_isMoveRotate{ false };

	XMFLOAT3					m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3     				m_xmf3Gravity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	float           			m_fMaxVelocityXZ = 0.0f;
	float           			m_fMaxVelocityY = 0.0f;
	float           			m_fFriction = 0.0f;

	float						m_fMinDistance = 0.0f;

	LPVOID						m_pPlayerUpdatedContext{ nullptr };
	LPVOID						m_pCameraUpdatedContext{ nullptr };

	CCamera*					m_pCamera{ nullptr };

	map<string, CItem*>			m_Normal_Inventory;
	map<string, CItem*>			m_Special_Inventory;
	list<CItem*>							m_RemovedItemList;

	CShaderManager* m_pShaderManager{ nullptr };

	float				m_Time = 0.f;

	bool			m_bLocalRotation{ false };
	bool			m_bSpike{ false };
	bool			m_bCameraVibe{ false };
	CBomb	*		m_BombParticle{ nullptr };

	//주인공 아이디
	char			m_pPlayerID;

	enum ANIMATION_TYPE
	{
		IDLE										= 0x0000,
		FRONT_RUN							= 0x0001,
		BACK_RUN							= 0x0002,
		SWING									= 0x0004,
		GOLDTIMER							= 0x0008,
		GOLDHAMMER					= 0x0010,
		WIN										= 0x0020,
		LOSE									= 0x0040,
		ICE										= 0x0080,
	
		// 혼합 동작
		FRONT_RUN_AND_SWING	= 0x0005,
		BACK_RUN_AND_SWING		= 0x0006,

		FRONT_RUN_AND_GOLDTIMER = 0x0009,
		BACK_RUN_AND_GOLDTIMER = 0x000A,

		FRONT_RUN_AND_GOLDHAMMER = 0x0011,
		BACK_RUN_AND_GOLDHAMMER = 0x0012,
	};

	// 플레이어의 키입력 정의
	enum KEY_TYPE
	{
		NONE				= 0x0000,
		UP					= 0x0001,
		DOWN				= 0x0002,
		RIGHT				= 0x0004,
		LEFT					= 0x0008,

		UP_RIGHT			= 0x0005,
		UP_LEFT			= 0x0009,
		DOWN_RIGHT	= 0x0006,
		DOWN_LEFT		= 0x000A
	};

	int m_PrevAnimation = ANIMATION_TYPE::IDLE;
	thread m_TimerThread;
	priority_queue<Timer_Event> m_TimerQueue;

	int		m_IceCooltime = 0;
	bool	m_bShowCoolTime = false;
	bool m_IsICEKeyDown = false;
};

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
};

