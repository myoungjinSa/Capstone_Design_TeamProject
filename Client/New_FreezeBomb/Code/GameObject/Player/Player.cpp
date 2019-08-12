#include "../../Stdafx/Stdafx.h"
#include "Player.h"
#include "../Terrain/Terrain.h"
#include "../../Material/Material.h"
#include "../../Shader/Shader.h"
#include "../Item/Item.h"
#include "../Shadow/Shadow.h"
#include "../../ShaderManager/ShaderManager.h"
#include "../../Shader/BillboardShader/UIShader/TimerUIShader/TimerUIShader.h"
#include "../../Shader/BillboardShader/BombParticleShader/BombParticleShader.h"
#include "../Billboard/Bomb/Bomb.h"
#include "../../FrameTransform/FrameTransform.h"
#include "../../Chatting/Chatting.h"
#include "../../Shader/BillboardShader/UIShader/OutcomeUIShader/OutcomeUIShader.h"
#include "../../Network/Network.h"
#include "../../Shader/CubeParticleShader/ExplosionParticleShader/ExplosionParticleShader.h"
#include "../../GameFramework/GameFramework.h"
#include "../../SoundSystem/SoundSystem.h"
#include "../../Stdafx/KeyDefine.h"

extern byte g_PlayerCharacter;
extern int g_State;
extern bool g_IsRoundEnd;

extern bool g_IsStop;
CPlayer::CPlayer()
{
	m_pCamera = NULL;

	m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);

	m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_xmf3Gravity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_fMaxVelocityXZ = 0.0f;
	m_fMaxVelocityY = 0.0f;
	m_fFriction = 0.0f;

	m_fPitch = 0.0f;
	m_fRoll = 0.0f;
	m_fYaw = 0.0f;

	m_pPlayerUpdatedContext = nullptr;
	m_pCameraUpdatedContext = nullptr;
}

CPlayer::~CPlayer()
{
	AddTimer(chrono::high_resolution_clock::now(), EV_QUIT);
	m_TimerThread.join();

	ReleaseShaderVariables();

	if (m_pCamera)
		delete m_pCamera;

	for (auto iter = m_Normal_Inventory.begin(); iter != m_Normal_Inventory.end();)
	{
		delete (*iter).second;
		iter = m_Normal_Inventory.erase(iter);
	}
	m_Normal_Inventory.clear();

	for (auto iter = m_Special_Inventory.begin(); iter != m_Special_Inventory.end();)
	{
		delete (*iter).second;
		iter = m_Special_Inventory.erase(iter);
	}
	m_Special_Inventory.clear();

	for (auto iter = m_RemovedItemList.begin(); iter != m_RemovedItemList.end(); )
	{
		(*iter)->Release();
		iter = m_RemovedItemList.erase(iter);
	}
	m_RemovedItemList.clear();

	if (m_pShadow)
		delete m_pShadow;
}

void CPlayer::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	if (m_pCamera)
		m_pCamera->CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CPlayer::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
}

void CPlayer::ReleaseShaderVariables()
{
	if (m_pCamera)
		m_pCamera->ReleaseShaderVariables();
}

void CPlayer::Move(DWORD dwDirection, float fDistance, bool bUpdateVelocity)
{
	if (dwDirection)
	{
		XMFLOAT3 xmf3Shift = XMFLOAT3(0, 0, 0);
		if (dwDirection & KEY_TYPE::UP) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look, fDistance);
		if (dwDirection & KEY_TYPE::DOWN) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look, -fDistance);
		if (dwDirection & KEY_TYPE::RIGHT) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right, fDistance);
		if (dwDirection & KEY_TYPE::LEFT) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right, -fDistance);

		Move(xmf3Shift, bUpdateVelocity);
	}
}

void CPlayer::Move(const XMFLOAT3& xmf3Shift, bool bUpdateVelocity)
{
	if (bUpdateVelocity)
	{
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, xmf3Shift);
		//m_xmf3Position = Vector3::Minus
		//m_xmf3Position = Vector3::Add(m_xmf3Position, m_xmf3Velocity);
	}
	else
	{
		// 여기서 플레이어가 정적인 오브젝트와 충돌했을 시에 못움직이게 해야됨.
		// 방법
		// 1. 플레이어가 무슨 키의 입력을 받았을 때, 충돌 되었는지
		// 2. 플레이어가 무슨 애니메이션일 때, 충돌 되었는지
		// 3. 
		m_xmf3Position = Vector3::Add(m_xmf3Position, xmf3Shift);

		m_xmf4x4World._41 = m_xmf3Position.x;
		m_xmf4x4World._42 = m_xmf3Position.y;
		m_xmf4x4World._43 = m_xmf3Position.z;

		m_pCamera->Move(xmf3Shift);
	}
}

void CPlayer::Rotate(float x, float y, float z)
{
	DWORD nCurrentCameraMode = m_pCamera->GetMode();
	if ((nCurrentCameraMode == FIRST_PERSON_CAMERA) || (nCurrentCameraMode == THIRD_PERSON_CAMERA))
	{
		if (x != 0.0f)
		{
			m_fPitch += x;
			if (m_fPitch > +89.0f) { x -= (m_fPitch - 89.0f); m_fPitch = +89.0f; }
			if (m_fPitch < -89.0f) { x -= (m_fPitch + 89.0f); m_fPitch = -89.0f; }
		}
		if (y != 0.0f)
		{
			m_fYaw += y;
			if (m_fYaw > 360.0f) m_fYaw -= 360.0f;
			if (m_fYaw < 0.0f) m_fYaw += 360.0f;
		}
		if (z != 0.0f)
		{
			m_fRoll += z;
			if (m_fRoll > +20.0f) { z -= (m_fRoll - 20.0f); m_fRoll = +20.0f; }
			if (m_fRoll < -20.0f) { z -= (m_fRoll + 20.0f); m_fRoll = -20.0f; }
		}
		m_pCamera->Rotate(x, y, z);
		if (y != 0.0f)
		{
			XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up), XMConvertToRadians(y));
			m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
			m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
		}
	}
	else if (nCurrentCameraMode == SPACESHIP_CAMERA)
	{
		m_pCamera->Rotate(x, y, z);
		if (x != 0.0f)
		{
			XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Right), XMConvertToRadians(x));
			m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
			m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, xmmtxRotate);
		}
		if (y != 0.0f)
		{
			XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up), XMConvertToRadians(y));
			m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
			m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
		}
		if (z != 0.0f)
		{
			XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Look), XMConvertToRadians(z));
			m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, xmmtxRotate);
			m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
		}
	}

	m_xmf3Look = Vector3::Normalize(m_xmf3Look);
	m_xmf3Right = Vector3::CrossProduct(m_xmf3Up, m_xmf3Look, true);
	m_xmf3Up = Vector3::CrossProduct(m_xmf3Look, m_xmf3Right, true);
}

void CPlayer::Update(float elapsedTime)
{
#ifndef _WITH_SERVER_
	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, m_xmf3Gravity); // 중력과 속도와 합
	float fLength = sqrtf(m_xmf3Velocity.x * m_xmf3Velocity.x + m_xmf3Velocity.z * m_xmf3Velocity.z);
	float fMaxVelocityXZ = m_fMaxVelocityXZ;
	if (fLength > m_fMaxVelocityXZ)
	{
		m_xmf3Velocity.x *= (fMaxVelocityXZ / fLength);
		m_xmf3Velocity.z *= (fMaxVelocityXZ / fLength);
	}
	
	
	float fMaxVelocityY = m_fMaxVelocityY;
	float fLengthY = sqrtf(m_xmf3Velocity.y * m_xmf3Velocity.y);
	if (fLengthY > m_fMaxVelocityY) m_xmf3Velocity.y *= (fMaxVelocityY / fLengthY);


	XMFLOAT3 xmf3Velocity = Vector3::ScalarProduct(m_xmf3Velocity, fTimeElapsed, false);
	Move(xmf3Velocity, false);

	if (m_pPlayerUpdatedContext) 
		OnPlayerUpdateCallback(fTimeElapsed);

	DWORD nCurrentCameraMode = m_pCamera->GetMode();
	if (nCurrentCameraMode == THIRD_PERSON_CAMERA) 
		m_pCamera->Update(m_xmf3Position, fTimeElapsed,IsCameraVibe());
	if (m_pCameraUpdatedContext) 
		OnCameraUpdateCallback(fTimeElapsed);
	if (nCurrentCameraMode == THIRD_PERSON_CAMERA) 
		m_pCamera->SetLookAt(m_xmf3Position);

	m_pCamera->RegenerateViewMatrix();

	fLength = Vector3::Length(m_xmf3Velocity);
	float fDeceleration = (m_fFriction * fTimeElapsed);
	if (fDeceleration > fLength) fDeceleration = fLength;
	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::ScalarProduct(m_xmf3Velocity, -fDeceleration, true));
	
	DecideAnimationState(fLength,fTimeElapsed);
#else	

	// 채팅이 활성화 되어있으면 애니메이션과 움직이는 처리 비활성화
	if (ChattingSystem::GetInstance()->IsChattingActive() == false)
	{	
		// 애니메이션 처리
		ProcessAnimation();
		// 이동처리
		ProcessMove();
	}

	//얼음 쿨타임 텍스트 렌더링 여부 결정
	if (m_bShowCoolTime)
	{
		//0초 가 되면 쿨타임 렌더링을 하지 않도록 한다.
		if (m_IceCooltime <= 0)
			m_bShowCoolTime = false;
	}

	DWORD nCurrentCameraMode = m_pCamera->GetMode();
	if (nCurrentCameraMode == THIRD_PERSON_CAMERA) 
		m_pCamera->Update(m_xmf3Position, elapsedTime, IsCameraVibe());
	if (m_pCameraUpdatedContext) 
		OnCameraUpdateCallback(elapsedTime);
	if (nCurrentCameraMode == THIRD_PERSON_CAMERA) 
		m_pCamera->SetLookAt(m_xmf3Position);

	m_pCamera->RegenerateViewMatrix();
#endif
}

CCamera *CPlayer::OnChangeCamera(DWORD nNewCameraMode, DWORD nCurrentCameraMode)
{
	CCamera *pNewCamera = NULL;
	switch (nNewCameraMode)
	{
	case FIRST_PERSON_CAMERA:
		pNewCamera = new CFirstPersonCamera(m_pCamera);
		break;
	case THIRD_PERSON_CAMERA:
		pNewCamera = new CThirdPersonCamera(m_pCamera);
		break;
	case SPACESHIP_CAMERA:
		pNewCamera = new CSpaceShipCamera(m_pCamera);
		break;
	}
	if (nCurrentCameraMode == SPACESHIP_CAMERA)
	{
		m_xmf3Right = Vector3::Normalize(XMFLOAT3(m_xmf3Right.x, 0.0f, m_xmf3Right.z));
		m_xmf3Up = Vector3::Normalize(XMFLOAT3(0.0f, 1.0f, 0.0f));
		m_xmf3Look = Vector3::Normalize(XMFLOAT3(m_xmf3Look.x, 0.0f, m_xmf3Look.z));

		m_fPitch = 0.0f;
		m_fRoll = 0.0f;
		m_fYaw = Vector3::Angle(XMFLOAT3(0.0f, 0.0f, 1.0f), m_xmf3Look);
		if (m_xmf3Look.x < 0.0f) m_fYaw = -m_fYaw;
	}
	else if ((nNewCameraMode == SPACESHIP_CAMERA) && m_pCamera)
	{
		m_xmf3Right = m_pCamera->GetRightVector();
		m_xmf3Up = m_pCamera->GetUpVector();
		m_xmf3Look = m_pCamera->GetLookVector();
	}

	if (pNewCamera)
	{
		pNewCamera->SetMode(nNewCameraMode);
		pNewCamera->SetPlayer(this);
	}

	if (m_pCamera) delete m_pCamera;

	return(pNewCamera);
}

void CPlayer::OnPrepareRender()
{
	m_xmf4x4ToParent._11 = m_xmf3Right.x; m_xmf4x4ToParent._12 = m_xmf3Right.y; m_xmf4x4ToParent._13 = m_xmf3Right.z;
	m_xmf4x4ToParent._21 = m_xmf3Up.x; m_xmf4x4ToParent._22 = m_xmf3Up.y; m_xmf4x4ToParent._23 = m_xmf3Up.z;
	m_xmf4x4ToParent._31 = m_xmf3Look.x; m_xmf4x4ToParent._32 = m_xmf3Look.y; m_xmf4x4ToParent._33 = m_xmf3Look.z;
	m_xmf4x4ToParent._41 = m_xmf3Position.x; m_xmf4x4ToParent._42 = m_xmf3Position.y; m_xmf4x4ToParent._43 = m_xmf3Position.z;

	m_xmf4x4ToParent = Matrix4x4::Multiply(XMMatrixScaling(m_xmf3Scale.x, m_xmf3Scale.y, m_xmf3Scale.z), m_xmf4x4ToParent);

	UpdateTransform(NULL);
}

void CPlayer::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera, int nPipelineState)
{
	DWORD nCameraMode = (pCamera) ? pCamera->GetMode() : 0x00;

	if (nCameraMode == THIRD_PERSON_CAMERA)
	{
#ifdef _WITH_SERVER_
		if (m_bBomb == true)
		{
			CGameObject::Tagger_Render(pd3dCommandList, pCamera, m_matID, m_bGoldTimer, GameObject);
			if (m_pShadow)
				m_pShadow->Tagger_Render(pd3dCommandList, pCamera, m_matID, m_bGoldTimer, GameObject_Shadow);
		}
		else
		{
			CGameObject::RunAway_Render(pd3dCommandList, pCamera, m_matID,m_bIce, m_bHammer, m_bGoldHammer,m_bLightening, GameObject);
			if (m_pShadow)
				m_pShadow->RunAway_Render(pd3dCommandList, pCamera, m_matID,m_bIce ,m_bHammer, m_bGoldHammer, GameObject_Shadow);
		}
#else
		if (m_bBomb == true)
		{
			CGameObject::Tagger_Render(pd3dCommandList, pCamera, g_PlayerCharacter, m_bGoldTimer, GameObject);
			if (m_pShadow)
				m_pShadow->Tagger_Render(pd3dCommandList, pCamera, g_PlayerCharacter, m_bGoldTimer, GameObject_Shadow);
		}
		else
		{
			CGameObject::RunAway_Render(pd3dCommandList, pCamera, g_PlayerCharacter, m_bIce, m_bHammer, m_bGoldHammer,m_bLightening, GameObject);
			if (m_pShadow)
				m_pShadow->RunAway_Render(pd3dCommandList, pCamera, g_PlayerCharacter, m_bIce, m_bHammer, m_bGoldHammer,GameObject_Shadow);
		}

#endif
	}
}

void CPlayer::Add_Inventory(const string& key, int ItemType)
{
	if (ItemType == CItem::NormalHammer)
	{
		CItem* pItem = new CItem;
		pItem->setItemType(ItemType);
#ifndef _WITH_SERVER_
		m_bHammer = true;
#endif
		m_Normal_Inventory.emplace(key, pItem);
	}
	else
	{
			if(m_Special_Inventory.size() == 0)
			{
				CItem* pItem = new CItem;
				pItem->setItemType(ItemType);
#ifndef _WITH_SERVER_
				if (ItemType == CItem::GoldHammer)
						m_bGoldHammer = true;
				else
					m_bGoldTimer = true;
#endif
				m_Special_Inventory.emplace(key, pItem);
			}
	}
}

void CPlayer::Sub_Inventory(int ItemType)
{
	if (ItemType == CItem::NormalHammer)
	{
#ifndef _WITH_SERVER_
		if(m_bHammer)
			m_bHammer = false;
#endif
		for (auto iter = m_Normal_Inventory.begin(); iter != m_Normal_Inventory.end();)
		{
			m_RemovedItemList.emplace_back((*iter).second);
			iter = m_Normal_Inventory.erase(iter);
		}
	}
	// 특수 아이템
	else
	{
		if (m_Special_Inventory.size() > 0)
		{
			for (auto iter = m_Special_Inventory.begin(); iter != m_Special_Inventory.end();)
			{
#ifndef _WITH_SERVER_
				if (ItemType == CItem::GoldHammer)
					m_bGoldHammer = false;
				else
					m_bGoldTimer = false;
#endif
				if (ItemType == (*iter).second->getItemType())
				{
					m_RemovedItemList.emplace_back((*iter).second);
					iter = m_Special_Inventory.erase(iter);
				}
				else
				{
					++iter;
				}
			}
		}
	}
}

void CPlayer::InventoryClear()
{
	for (auto iter = m_Normal_Inventory.begin(); iter != m_Normal_Inventory.end(); )
	{
		m_RemovedItemList.emplace_back((*iter).second);
		iter = m_Normal_Inventory.erase(iter);
	}
	m_Normal_Inventory.clear();

	for (auto iter = m_Special_Inventory.begin(); iter != m_Special_Inventory.end(); )
	{
		m_RemovedItemList.emplace_back((*iter).second);
		iter = m_Special_Inventory.erase(iter);
	}
	m_Special_Inventory.clear();
}

////이미 인벤의 존재하는 종류의 아이템인지 검사하고 없다면 true 반환
bool CPlayer::CheckInventoryToGet(const int& itemType)
{
	bool ret = false; 
	switch (itemType)
	{
	case CItem::ItemType::NormalHammer:
		if (m_Normal_Inventory.size() <= 0)
			ret = true;
		break;
	case CItem::ItemType::GoldHammer:

		if (m_Special_Inventory.size() <= 0)
		{
			ret = true;
		}
		else if(m_Special_Inventory.size() < 1)
		{
			for (auto iter : m_Special_Inventory)
			{
				if (iter.second->getItemType() != CItem::ItemType::GoldHammer)
				{
					ret = true;
				}
			}
		}
		break;
	case CItem::ItemType::GoldTimer:
		if (m_Special_Inventory.size() <= 0) 
		{
			ret = true;
		}
		else if(m_Special_Inventory.size() < 1)
		{
			for (auto iter : m_Special_Inventory)
			{
				if (iter.second->getItemType() != CItem::ItemType::GoldTimer)
				{
					ret = true;
				}
			}
		}
		break;
	}
	return ret;
}

void CPlayer::ChangeRound()
{
	// 라운드 바꼈으면, 인벤토리 비우고, 초기화
	for (auto iter = m_Normal_Inventory.begin(); iter != m_Normal_Inventory.end();)
	{
		m_RemovedItemList.emplace_back((*iter).second);
		iter = m_Normal_Inventory.erase(iter);
	}
	m_Normal_Inventory.clear();

	for (auto iter = m_Special_Inventory.begin(); iter != m_Special_Inventory.end();)
	{
		m_RemovedItemList.emplace_back((*iter).second);
		iter = m_Special_Inventory.erase(iter);
	}
	m_Special_Inventory.clear();

	m_bBomb = false;
	m_bIce = false;
	m_bHammer = false;
	m_bGoldHammer = false;
	m_bGoldTimer = false;
}

//플레이어의 애니메이션을 결정
void CPlayer::DecideAnimationState(float fLength,const float& fTimeElapsed)
{
	CAnimationController* pController = m_pAnimationController;

	if (fLength == 0.0f 
		&& m_isMoveRotate == false
		&&( pController->GetAnimationState() != CAnimationController::ATTACK
			&& pController->GetAnimationState() != CAnimationController::DIGGING
			&& pController->GetAnimationState() != CAnimationController::JUMP
			&& pController->GetAnimationState() != CAnimationController::RAISEHAND
			&& pController->GetAnimationState() != CAnimationController::DIE
			&& pController->GetAnimationState() != CAnimationController::ICE
			&& pController->GetAnimationState() != CAnimationController::SLIDE
			&& pController->GetAnimationState() != CAnimationController::USEGOLDHAMMER
			&& pController->GetAnimationState() != CAnimationController::VICTORY
			))
	{
	
#ifdef _WITH_SERVER_	
		//if (m_IdleCount < 1) 
		//{
		//	
		//	m_IsRun = false;
		//	

		//	m_IdleCount++;
		//	SetTrackAnimationSet(0, CAnimationController::IDLE);
		//	m_pAnimationController->SetAnimationState(CAnimationController::IDLE);
		//}
		
		
#else
		if (pController->GetAnimationState() == CAnimationController::RUNFAST)
		{
			m_pAnimationController->SetTrackPosition(0, 0.0f);
		}

		SetTrackAnimationSet(0, CAnimationController::IDLE);
		m_pAnimationController->SetAnimationState(CAnimationController::IDLE);
#endif
		m_bLocalRotation = false;

	}

	else 
	{
		if (GetAsyncKeyState(VK_UP) & 0x8000
			&& m_bIce == false
			&& pController->GetAnimationState() != CAnimationController::ATTACK
			&& pController->GetAnimationState() != CAnimationController::JUMP
			&& pController->GetAnimationState() != CAnimationController::ICE
			&& pController->GetAnimationState() != CAnimationController::RAISEHAND
			&& pController->GetAnimationState() != CAnimationController::DIE
			&& pController->GetAnimationState() != CAnimationController::USEGOLDHAMMER
			&& pController->GetAnimationState() != CAnimationController::VICTORY
			)
		{
#ifdef _WITH_SERVER_
			if (pController->GetAnimationState() != CAnimationController::RUNFAST)
			{
				SetTrackAnimationPosition(0, 0.0f);
				SetTrackAnimationSet(0, CAnimationController::RUNFAST);
				m_pAnimationController->SetAnimationState(CAnimationController::RUNFAST);

				Network::GetInstance()->SendAnimationState(CAnimationController::RUNFAST);
			}
#else
			SetTrackAnimationSet(0, CAnimationController::RUNFAST);
			m_pAnimationController->SetAnimationState(CAnimationController::RUNFAST);
#endif
		}

		else if (GetAsyncKeyState(VK_DOWN) & 0x8000
			&& m_bIce == false
			&& pController->GetAnimationState() != CAnimationController::ATTACK
			&& pController->GetAnimationState() != CAnimationController::JUMP
			&& pController->GetAnimationState() != CAnimationController::ICE
			&& pController->GetAnimationState() != CAnimationController::RAISEHAND
			&& pController->GetAnimationState() != CAnimationController::DIE
			&& pController->GetAnimationState() != CAnimationController::VICTORY
			&& pController->GetAnimationState() != CAnimationController::USEGOLDHAMMER
			)
		{
#ifdef _WITH_SERVER_
			if (pController->GetAnimationState() != CAnimationController::RUNBACKWARD)
			{
				Network::GetInstance()->SendAnimationState(CAnimationController::RUNBACKWARD);
				m_pAnimationController->SetAnimationState(CAnimationController::RUNBACKWARD);
				SetTrackAnimationSet(0, CAnimationController::RUNBACKWARD);
			}
#else
			m_pAnimationController->SetAnimationState(CAnimationController::RUNBACKWARD);
			SetTrackAnimationSet(0, CAnimationController::RUNBACKWARD);
#endif
		}

		else
		{

		}
	}
	
#ifdef _WITH_SERVER_
		
	if ((GetAsyncKeyState(VK_RIGHT) &0x8000
		|| GetAsyncKeyState(VK_LEFT) &0x8000)
		&& !(GetAsyncKeyState(VK_UP) & 0x8000)
		&& !(GetAsyncKeyState(VK_DOWN) & 0x8000)
		&&( pController->GetAnimationState() == CAnimationController::RUNFAST
		|| pController->GetAnimationState() == CAnimationController::RUNBACKWARD)
		)
	{
		SetTrackAnimationSet(0, CAnimationController::IDLE);
		m_pAnimationController->SetAnimationState(CAnimationController::IDLE);
		Network::GetInstance()->SendAnimationState(CAnimationController::IDLE);
		
	}
		
#endif

	if (m_isMoveRotate)
	{
		m_isMoveRotate = false;
	}

	//점프 주석
//	if (GetAsyncKeyState(VK_SPACE) & 0x8000
//		&& pController->GetAnimationState() != CAnimationController::JUMP
//		&& pController->GetAnimationState() != CAnimationController::ICE
//		&& pController->GetAnimationState() != CAnimationController::VICTORY
//		&& ChattingSystem::GetInstance()->IsChattingActive() ==false
//		)
//	{
//		SetTrackAnimationSet(0, CAnimationController::JUMP);
//		SetTrackAnimationPosition(0, 0);
//		pController->SetAnimationState(CAnimationController::JUMP);
//#ifdef _WITH_SERVER_
//			Network::GetInstance()->SendAnimationState(CAnimationController::JUMP);
//#endif
//		//pController->SetTrackSpeed(0, 1.5f);
//	}

#ifndef _WITH_SERVER_			//서버와의 연동시에는 적용 x
	// 치트키
	//추후에 아이템과 충돌여부 및 아이템 획득 여부로 변경해서 하면 될듯
	// 술래일때랑, 도망자일때로 각각 다르게 작동
	if (GetAsyncKeyState(VK_C) & 0x0001 && ChattingSystem::GetInstance()->IsChattingActive() == false)
	{
		if (pController->GetAnimationState() == CAnimationController::ICE)
		{
			pController->SetAnimationState(CAnimationController::IDLE);
			SetTrackAnimationSet(0, CAnimationController::IDLE);
			SetTrackAnimationPosition(0, 0);
			m_bIce = false;
		}

		ChangeRole();

		// 술래일때,
		if (m_bBomb == true)
		{
			// 만약 술래가 되기전 얼음이었다면,
			Add_Inventory("치트_황금시계", CItem::GoldTimer);
		}
		// 도망자일때,
		else
		{
			Add_Inventory("치트_망치", CItem::NormalHammer);
			//Add_Inventory("치트_황금망치", CItem::GoldHammer);
		}
	}

	// 시간증가 치트키
	if (GetAsyncKeyState(VK_T) & 0x0001)
	{
		if (m_pShaderManager)
			CTimerUIShader::setTimer(10.f);
	}

	if (GetAsyncKeyState(VK_R) & 0x0001)
	{
		if (m_pShaderManager)
			CTimerUIShader::setReduceTimer(10.f);
	}

	if(GetAsyncKeyState(VK_X) & 0x0001
		&& pController->GetAnimationState() != CAnimationController::ICE
		&& ChattingSystem::GetInstance()->IsChattingActive() ==false)
	{
		pController->SetTrackAnimationSet(0, CAnimationController::SLIDE);
		pController->SetAnimationState(CAnimationController::SLIDE);
		//pController->SetTrackSpeed(0, 10.0f);
	}
#endif

	float gameTime = CTimerUIShader::getTimer();

	////얼음으로 변신
	if (GetAsyncKeyState(VK_A) & 0x0001
		&& ChattingSystem::GetInstance()->IsChattingActive() ==false
		&& m_bBomb == false
		&& g_State == GAMESTATE::INGAME
		&& gameTime < MAX_ROUND_TIME - 5)
	{	
#ifdef _WITH_SERVER_
		cout << "SendFreezeState()\n";
		Network::GetInstance()->SendFreezeState();
#else
		m_bIce = !m_bIce;
		pController->SetTrackAnimationSet(0, CAnimationController::IDLE);
		if (m_bIce == true)
		{
			pController->SetAnimationState(CAnimationController::ICE);
		}
		else
		{
			pController->SetAnimationState(CAnimationController::IDLE);
		}
#endif
	}

	// 망치로 때리기 애니메이션
	if (GetAsyncKeyState(VK_CONTROL) & 0x0001
		&& pController->GetAnimationState() != CAnimationController::ATTACK
		&& pController->GetAnimationState() != CAnimationController::ICE
		&& pController->GetAnimationState() != CAnimationController::USEGOLDHAMMER
		&& pController->GetAnimationState() != CAnimationController::DIE
		&& pController->GetAnimationState() != CAnimationController::RAISEHAND
		&& pController->GetAnimationState() != CAnimationController::VICTORY
		&& ChattingSystem::GetInstance()->IsChattingActive() ==false
		)
	{
		
		SetTrackAnimationSet(0, CAnimationController::ATTACK);
		SetTrackAnimationPosition(0, 0.0f);
		pController->SetAnimationState(CAnimationController::ATTACK);
		
#ifdef _WITH_SERVER_
		Network::GetInstance()->SendAnimationState(CAnimationController::ATTACK);
#endif
		//if (m_Normal_Inventory.size() != 0)
			//Sub_Inventory(CItem::NormalHammer);
	}


	// 특수 아이템을 사용하는 동작에 경우 일정 시간동안은 황금 망치를 이용해 특수 아이템 모션을 렌더링하고 Sub_Inventory를 해줘야하기 때문에
	// 쿨타임을 세어 줘야한다.
	static float eraseTime = 0.0f;
	static bool countCoolTime = false;
	if (countCoolTime)
	{
		eraseTime += fTimeElapsed;
		if(eraseTime > 1.85f)		//1.75는 USEGOLDHAMMER 애니메이션에 길이 - 0.8f 해준 값
		{
			if (!m_bBomb ) 
			{
#ifndef _WITH_SERVER_
				Sub_Inventory(GOLD_HAMMER);	
#else
				Network::GetInstance()->SendUseItem(ITEM::GOLD_HAMMER, GetPlayerID());
				eraseTime = 0.0f;
#endif
			}
			m_bLocalRotation = false;
			m_bLightening = false;
			countCoolTime = false;
		}
	}
	
	// 특수 아이템 사용 버튼(ALT)
	if (GetAsyncKeyState(VK_MENU) & 0x0001
		&& pController->GetAnimationState() != CAnimationController::ATTACK
		&& pController->GetAnimationState() != CAnimationController::ICE
		&& pController->GetAnimationState() != CAnimationController::DIE
		&& pController->GetAnimationState() != CAnimationController::VICTORY
		&& ChattingSystem::GetInstance()->IsChattingActive() ==false
		)
	{
		if (m_Special_Inventory.size() > 0 )
		{
			bool isGoldHammer = false;
			bool isGoldTimer = false;
			for(const pair<string,CItem*>& iter : m_Special_Inventory){
				if(iter.second->getItemType() == GOLD_HAMMER)
				{
					isGoldHammer = true;
				}
				if(iter.second->getItemType() == GOLD_HAMMER)
				{
					isGoldTimer = true;
				}
			}
			if (!m_bBomb && isGoldHammer == true)
			{	
				m_bLightening = true;
				SetTrackAnimationSet(0, CAnimationController::USEGOLDHAMMER);
				SetTrackAnimationPosition(0, 0.0f);
				pController->SetAnimationState(CAnimationController::USEGOLDHAMMER);
				m_bLocalRotation = true;
				
#ifdef _WITH_SERVER_
				Network::GetInstance()->SendAnimationState(CAnimationController::USEGOLDHAMMER);
#endif
				//쿨타임 체크 set
				countCoolTime = true;
			}

			else if(m_bBomb && m_bGoldTimer == true)
			{
				if (m_pShaderManager)
				{
					SetTrackAnimationSet(0, CAnimationController::RAISEHAND);
					SetTrackAnimationPosition(0, 0.0f);
					pController->SetAnimationState(CAnimationController::RAISEHAND);
#ifdef _WITH_SERVER_
					Network::GetInstance()->SendAnimationState(CAnimationController::RAISEHAND);
					Network::GetInstance()->SendUseItem(ITEM::GOLD_TIMER, GetPlayerID());
#else
					// 30초 증가
					CTimerUIShader::setTimer(30.f);
					Sub_Inventory(GOLD_TIMER);
#endif					
				}
			}	
		}
	}
#ifdef _WITH_SERVER_
	//// 폭탄이 있을 때,
	if (m_bBomb == true)
	{
		if (m_pShaderManager)
		{
			if (pController->GetAnimationState() != CAnimationController::DIE)
			{
				auto iter = m_pShaderManager->getShaderMap().find("TimerUI");
				if (iter != m_pShaderManager->getShaderMap().end())
				{
					if (CTimerUIShader::getTimer() <= 0.f)
					{
						auto iter2 = m_pShaderManager->getShaderMap().find("Bomb");
						auto explosionIter = m_pShaderManager->getShaderMap().find("ExplosionParticle");

						if (iter2 != m_pShaderManager->getShaderMap().end()
							&& explosionIter != m_pShaderManager->getShaderMap().end())
						{
							// 폭탄 터지는 애니메이션
							m_BombParticle = reinterpret_cast<CBombParticleShader*>((*iter2).second)->getBomb();
							m_BombParticle->SetPosition(m_xmf3Position.x, m_xmf3Position.y, m_xmf3Position.z);
							m_BombParticle->setIsBlowing(true);

							// 폭탄 파티클이 터지는 애니메이션
							reinterpret_cast<CExplosionParticleShader*>((*explosionIter).second)->SetParticleBlowUp(m_xmf3Position);

							// DIE 애니메이션으로 설정
							pController->SetTrackPosition(0, 0.0f);
							pController->SetTrackAnimationSet(0, CAnimationController::DIE);
							pController->SetAnimationState(CAnimationController::DIE);

							m_bBomb = false;
						}
					}
				}
			}
		}
	}			
#endif
}

bool CPlayer::AnimationCollision(byte AnimationType)
{
	switch (AnimationType)
	{
		// 망치로 때릴 때 충돌체크
	case CAnimationController::ATTACK:
		if (m_bHammer)
		{
			if (m_pAnimationController->GetAnimationState() == AnimationType)
			{
				// 망치로 내려 찍는 애니메이션의 위치 근사 값
				if (0.5 <= GetTrackAnimationPosition(0) && GetTrackAnimationPosition(0) <= 0.65) 
				{
					return true;
				}
			}
		}
		break;

	default:
		break;
	}
	return false;
}

void CPlayer::ChangeRole()
{
	if (m_Normal_Inventory.size() > 0)
		Sub_Inventory(CItem::NormalHammer);
	if (m_Special_Inventory.size() > 0)
		Sub_Inventory(CItem::GoldHammer);
	if (m_Special_Inventory.size() > 0)
		Sub_Inventory(CItem::GoldTimer);

	m_bBomb = !m_bBomb;
}

void CPlayer::ProcessAnimation()
{
	int animation = ANIMATION_TYPE::IDLE;

	//if (m_IsICEKeyDown == false)
	//{
		if (GetAsyncKeyState(VK_UP) & 0x8000)
			animation = animation | ANIMATION_TYPE::FRONT_RUN;

		else if (GetAsyncKeyState(VK_DOWN) & 0x8000)
			animation = animation | ANIMATION_TYPE::BACK_RUN;

		if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
			animation = animation | ANIMATION_TYPE::SWING;

		else if (GetAsyncKeyState(VK_MENU) & 0x8000)
		{
			int result = CheckSpecialItem();
			if (result == 1)
			{
				animation = animation | ANIMATION_TYPE::GOLDTIMER;
				cout << "황금시계 애니메이션 보냄" << endl;
			}
			else if (result == -1)
			{
				animation = animation | ANIMATION_TYPE::GOLDHAMMER;
				cout << "황금망치 애니메이션 보냄" << endl;
			}
		}
	//}

	// A키
	if (KEY_DOWN(VK_A))
	{
		if (m_IsICEKeyDown == false)
		{
			m_IsICEKeyDown = true;
			cout << "얼음 키 처음 누름" << endl;
		}
	}
	else if (KEY_UP(VK_A))
	{
		if (m_IsICEKeyDown == true)
		{
			if (CheckICE() == true)
			{
				animation = ANIMATION_TYPE::IDLE;
				Network::GetInstance()->SendFreezeState();
				cout << "얼음 키 떼고, 얼음 보냄" << endl;
			}
			m_IsICEKeyDown = false;
		}
	}

	if (m_bIce == true)
		if (m_PrevAnimation != ANIMATION_TYPE::IDLE)
			animation = animation & ANIMATION_TYPE::IDLE;

	// &연산을 해서 키가 안눌렸으면, 0x0000이 되므로 IDLE이 됨
	// 아니면, 나의 동작을 처리함
	animation = animation & INT_MAX;

	if(m_PrevAnimation != animation)
		SendAnimation(animation);
}

void CPlayer::SendAnimation(int animation)
{
	switch (animation)
	{
	case ANIMATION_TYPE::IDLE:
		animation = CAnimationController::ANIMATIONTYPE::IDLE;
		break;

	case ANIMATION_TYPE::FRONT_RUN:
		animation = CAnimationController::ANIMATIONTYPE::RUNFAST;
		break;

	case ANIMATION_TYPE::BACK_RUN:
		animation = CAnimationController::ANIMATIONTYPE::RUNBACKWARD;
		break;

	case ANIMATION_TYPE::SWING:
	case ANIMATION_TYPE::FRONT_RUN_AND_SWING:
	case ANIMATION_TYPE::BACK_RUN_AND_SWING:
		animation = CAnimationController::ANIMATIONTYPE::ATTACK;
		break;

	case ANIMATION_TYPE::GOLDTIMER:
	case ANIMATION_TYPE::FRONT_RUN_AND_GOLDTIMER:
	case ANIMATION_TYPE::BACK_RUN_AND_GOLDTIMER:
		animation = CAnimationController::ANIMATIONTYPE::RAISEHAND;
		break;

	case ANIMATION_TYPE::GOLDHAMMER:
	case ANIMATION_TYPE::FRONT_RUN_AND_GOLDHAMMER:
	case ANIMATION_TYPE::BACK_RUN_AND_GOLDHAMMER:
		animation = CAnimationController::ANIMATIONTYPE::USEGOLDHAMMER;
		break;

	case ANIMATION_TYPE::WIN:
		animation = CAnimationController::ANIMATIONTYPE::VICTORY;
		break;

	case ANIMATION_TYPE::LOSE:
		animation = CAnimationController::ANIMATIONTYPE::DIE;
		break;

	default:
		break;
	}

	// 애니메이션 설정
	SetAnimation(animation);
}

bool CPlayer::CheckAnimation(int animation)
{
	if (animation == CAnimationController::ANIMATIONTYPE::DIE || animation == CAnimationController::ANIMATIONTYPE::VICTORY)
		return true;

	// 라운드가 끝났을 때, 다른 애니메이션이 되는 것을 방지하기 위해
	if (g_IsRoundEnd == true)
		return false;

	// 얼음 상태면, 동작을 바꾸면 안댐 
	if (m_bIce == true)
	{
		if (m_PrevAnimation == ANIMATION_TYPE::IDLE)
			return false;
	}

	// 현재 셋된 애니메이션이 ONCE일 때, 그 동작이 진행 중이라면,
	if (m_pAnimationController->m_pAnimationTracks->m_pAnimationSet->m_nType == ANIMATION_TYPE_ONCE)
	{
		int state = m_pAnimationController->GetAnimationState();

		// 죽고나서 라운드 시작할 때
		if (state == CAnimationController::ANIMATIONTYPE::DIE)
			return true;

		else if (state != CAnimationController::ANIMATIONTYPE::IDLE)
			return false;
	}
	return true;
}

void CPlayer::SetAnimation(int animation)
{
	// 애니메이션이 서버와 같은 상태인지 체크
	if (CheckAnimation(animation) == false)
		return;

	Network::GetInstance()->SendAnimationState(animation);

	m_pAnimationController->SetTrackAnimationSet(0, (CAnimationController::ANIMATIONTYPE)animation);
	m_pAnimationController->SetAnimationState((CAnimationController::ANIMATIONTYPE)animation);
	m_pAnimationController->SetTrackPosition(0, 0.0f);

	switch (animation)
	{
	case CAnimationController::ANIMATIONTYPE::IDLE:
		m_PrevAnimation = ANIMATION_TYPE::IDLE;
		break;

	case CAnimationController::ANIMATIONTYPE::RUNFAST:
		m_PrevAnimation = ANIMATION_TYPE::FRONT_RUN;
		break;

	case CAnimationController::ANIMATIONTYPE::RUNBACKWARD:
		m_PrevAnimation = ANIMATION_TYPE::BACK_RUN;
		break;

	case CAnimationController::ANIMATIONTYPE::ATTACK:
		m_PrevAnimation = ANIMATION_TYPE::SWING;
		break;

	case CAnimationController::ANIMATIONTYPE::RAISEHAND:
		m_PrevAnimation = ANIMATION_TYPE::GOLDTIMER;
		ControlAnimationOnce();
		break;

	case CAnimationController::ANIMATIONTYPE::USEGOLDHAMMER:
		m_PrevAnimation = ANIMATION_TYPE::GOLDHAMMER;
		ControlAnimationOnce();
		break;

	case CAnimationController::ANIMATIONTYPE::VICTORY:
		m_PrevAnimation = ANIMATION_TYPE::WIN;
		break;

	case CAnimationController::ANIMATIONTYPE::DIE:
		m_PrevAnimation = ANIMATION_TYPE::LOSE;
		break;

	default:
		return;
	}
}

bool CPlayer::CheckNormalItem()
{
	if (m_bBomb == true || m_bIce == true || m_bHammer == false)
		return false;

	return true;
}

int CPlayer::CheckSpecialItem()
{
	// 술래일 때,
	if (m_bBomb == true)
	{
		if (m_bIce == true || m_bGoldTimer == false)
			return 0;

		return 1;
	}
	// 술래가 아닐 때,
	else
	{
		if (m_bIce == true || m_bGoldHammer == false)
			return 0;

		return -1;
	}

	return 0;
}

bool CPlayer::CheckICE()
{
	if (g_IsRoundEnd == true)
		return false;

	if(m_bBomb == true)
		return false;

	if (CGameFramework::GetIceChangeOK() == false)
		return false;

	if (m_IceCooltime > 0)
		return false;

	return true;
}

bool CPlayer::CheckMove(int key)
{
	if (g_IsRoundEnd == true)
		return false;

	// 얼음 상태일 때,
	if (m_bIce == true)
	{
		if (key & KEY_TYPE::UP || key & KEY_TYPE::DOWN)
			return false;
	}

	if (m_PrevAnimation & ANIMATION_TYPE::GOLDHAMMER)
		return false;

	if (m_PrevAnimation & ANIMATION_TYPE::GOLDTIMER)
		return false;

	return true;
}

void CPlayer::ProcessMove()
{
	int key = KEY_TYPE::NONE;

	if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
		key = key | KEY_TYPE::RIGHT;

	else if (GetAsyncKeyState(VK_LEFT) & 0x8000)
		key = key | KEY_TYPE::LEFT;

	XMFLOAT3 velocity = XMFLOAT3(0.f, 0.f, 0.f);
	if (GetAsyncKeyState(VK_UP) & 0x8000)
	{
		if (m_bBomb)
			velocity = Vector3::Add(velocity, m_xmf3Look, VELOCITY * 1.2f);
		else
			velocity = Vector3::Add(velocity, m_xmf3Look, VELOCITY);
		key = key | KEY_TYPE::UP;
	}

	else if (GetAsyncKeyState(VK_DOWN) & 0x8000)
	{
		if (m_bBomb)
			velocity = Vector3::Add(velocity, m_xmf3Look, -VELOCITY * 1.2f);
		else
			velocity = Vector3::Add(velocity, m_xmf3Look, -VELOCITY);
		key = key | KEY_TYPE::DOWN;
	}

	// 1로 다채워진 비트로 비교하면 검사를 할 수 있다.
	key = key & INT_MAX;

	// 이동해도 되는지 검사 먼저
	if (CheckMove(key) == false)
		return;

	if (key & KEY_TYPE::RIGHT || key & KEY_TYPE::LEFT)
		RotateAxisY(key);

	Move(velocity, false);

	switch (key)
	{
	case KEY_TYPE::UP:					Network::GetInstance()->SendUpKey();					break;
	case KEY_TYPE::DOWN:			Network::GetInstance()->SendDownKey();			break;
	case KEY_TYPE::RIGHT:				Network::GetInstance()->SendRightKey();				break;
	case KEY_TYPE::LEFT:				Network::GetInstance()->SendLeftKey();				break;
	case KEY_TYPE::UP_RIGHT:		Network::GetInstance()->SendUpRightKey();			break;
	case KEY_TYPE::UP_LEFT:			Network::GetInstance()->SendUpLeftKey();			break;
	case KEY_TYPE::DOWN_RIGHT:	Network::GetInstance()->SendDownRightKey();	break;
	case KEY_TYPE::DOWN_LEFT:	Network::GetInstance()->SendDownLeftKey();		break;

		// 키 안눌렀을 때
	default:
		break;
	}
}

void CPlayer::RotateAxisY(int key)
{
	XMFLOAT3 xmf3Look = m_xmf3Look;
	XMFLOAT3 xmf3Right = m_xmf3Right;
	XMFLOAT3 xmf3Up = m_xmf3Up;
	XMMATRIX xmmtxRotate;

	float dotProduct = Vector3::DotProduct(xmf3Look, xmf3Right);
	float 	angle = ::IsEqual(dotProduct, 1.0f) ? 0.0f : ((dotProduct > 1.0f) ? XMConvertToDegrees(acos(dotProduct)) : 90.0f);

	if(key & KEY_TYPE::RIGHT)
		xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up), XMConvertToRadians(angle * ROTATE_RATE));

	else if(key & KEY_TYPE::LEFT)
		xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up), XMConvertToRadians(-(angle * ROTATE_RATE)));
	
	m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
	m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);

	m_xmf3Look = Vector3::Normalize(m_xmf3Look);
	m_xmf3Right = Vector3::CrossProduct(m_xmf3Up, m_xmf3Look, true);
	m_xmf3Up = Vector3::CrossProduct(m_xmf3Look, m_xmf3Right, true);
}

bool CPlayer::ControlAnimationOnce()
{
	if (m_PrevAnimation & ANIMATION_TYPE::SWING)
		return true;

	// 현재 셋팅 되어있는 애니메이션 길이를 얻어옴
	float animationLength = m_pAnimationController->m_pAnimationTracks->m_pAnimationSet->m_fLength;
	if (m_PrevAnimation & ANIMATION_TYPE::GOLDTIMER)
	{
		animationLength -= 1.0f;
		// float로 되어있는 sec단위라서 * 1000을 하여 millisec 단위로 바꾼다.
		int sce_To_millisec = animationLength * 1000;
		chrono::milliseconds sec = static_cast<chrono::milliseconds>(sce_To_millisec);
		AddTimer(chrono::high_resolution_clock::now() + sec, EV_GOLDTIMER);
		
		return true;
	}

	else if (m_PrevAnimation & ANIMATION_TYPE::GOLDHAMMER)
	{
		m_bLightening = true;
		m_bLocalRotation = true;

		animationLength -= 0.1f;
		int sce_To_millisec = animationLength * 1000;
		chrono::milliseconds sec = static_cast<chrono::milliseconds>(sce_To_millisec);
		AddTimer(chrono::high_resolution_clock::now() + sec, EV_GOLDHAMMER);

		return true;
	}

	return false;
}

void CPlayer::ProcessRoundStart()
{
	// 라운드가 시작하면, IDLE 애니메이션을 서버에게 보냄
	Network::GetInstance()->SendAnimationState(CAnimationController::ANIMATIONTYPE::IDLE);
	SendAnimation(ANIMATION_TYPE::IDLE);
}

void CPlayer::ProcessRoundEnd()
{
	// 술래였을 때,
	if (m_bBomb == true && m_pShaderManager != nullptr)
	{
		auto bomb = m_pShaderManager->getShaderMap().find("Bomb");
		if (bomb == m_pShaderManager->getShaderMap().end())
			return;

		auto particle = m_pShaderManager->getShaderMap().find("ExplosionParticle");
		if (particle == m_pShaderManager->getShaderMap().end())
			return;

		// 폭탄 터지는 애니메이션
		m_BombParticle = reinterpret_cast<CBombParticleShader*>((*bomb).second)->getBomb();
		m_BombParticle->SetPosition(m_xmf3Position.x, m_xmf3Position.y, m_xmf3Position.z);
		m_BombParticle->setIsBlowing(true);

		// 폭탄 파티클이 터지는 애니메이션
		reinterpret_cast<CExplosionParticleShader*>((*particle).second)->SetParticleBlowUp(m_xmf3Position);

		// 서버에게 폭탄이터졌다고 전송
		Network::GetInstance()->SendBombExplosion();

		// 서버에게 DIE 애니메이션 전송
		Network::GetInstance()->SendAnimationState(CAnimationController::ANIMATIONTYPE::DIE);
		SendAnimation(ANIMATION_TYPE::LOSE);
		// 폭탄을 없앤다.
		m_bBomb = false;
	}

	// 술래가 아닐 때,
	else if (m_bBomb == false && m_pShaderManager != nullptr)
	{
		Network::GetInstance()->SendAnimationState(CAnimationController::ANIMATIONTYPE::VICTORY);
		SendAnimation(ANIMATION_TYPE::WIN);
	}
}

void CPlayer::ProcessEvent()
{
	while (true)
	{
		// CPU 부하를 덜기위해 잠깐 멈춤
		this_thread::sleep_for(10ms);
		while (true)
		{
			//timer_queue_mtx.lock();
			if (m_TimerQueue.empty() == true)
			{
				//timer_queue_mtx.unlock();
				break;
			}
			else
			{
				//timer_queue_mtx.unlock();
			}


			// 타이머 큐에서 해당 이벤트를 얻어옴
			//timer_queue_mtx.lock();
			Timer_Event ev = m_TimerQueue.top();
			//timer_queue_mtx.unlock();

			if (ev.start_time > chrono::high_resolution_clock::now())
				break;

			//timer_queue_mtx.lock();
			m_TimerQueue.pop();
			//timer_queue_mtx.unlock();

			switch (ev.event_type)
			{
			case EVENT_TYPE::EV_GOLDTIMER:
				Network::GetInstance()->SendUseItem(ITEM::GOLD_TIMER, GetPlayerID());
				m_bGoldTimer = false;
				break;

			case EVENT_TYPE::EV_GOLDHAMMER:
				Network::GetInstance()->SendUseItem(ITEM::GOLD_HAMMER, GetPlayerID());
				m_bLightening = false;
				m_bLocalRotation = false;
				m_bGoldHammer = false;
				break;

				// 클라 종료
			case EVENT_TYPE::EV_QUIT:
				// 큐에 있는 이벤트를 모두 비운다
				while (m_TimerQueue.empty() == false)
					m_TimerQueue.pop();
				return;

			default:
				break;
			}
		}
	}
}

void CPlayer::AddTimer(chrono::high_resolution_clock::time_point start_t, EVENT_TYPE e_type)
{
	//timer_queue_mtx.lock();
	// 타이머 큐에 해당 이벤트를 넣음
	m_TimerQueue.push(Timer_Event{ start_t, e_type });
	//timer_queue_mtx.unlock();
}

void CPlayer::DeadReckoning(float elapsedTime)
{
	static double time = 0;
	time = elapsedTime;

	if (m_bIce == true)
		return;

	// 이전 위치
	XMFLOAT3 prevPos = XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43);
	XMFLOAT3 prevCameraPos = XMFLOAT3(m_pCamera->GetPosition().x, m_pCamera->GetPosition().y, m_pCamera->GetPosition().z);
	XMFLOAT3 distance = XMFLOAT3(0.f, 0.f, 0.f);

	// 가속도
	float acceleration = 1;
	float speed = 0.0001f;
	double temp1[2] = { prevPos.x, prevPos.z };
	double temp4[2] = { prevCameraPos.x, prevCameraPos.z };

	// 속도 * 시간
	double temp2 = speed * time;
	double temp3 = (0.5 * acceleration * time * time);
	double movePos[2] = { 0 };
	double moveCameraPos[2] = { 0 };

}

const XMFLOAT2& CPlayer::ConvertIceCoolTimeTextToNDCSpace()
{
	XMFLOAT4X4 viewProj = Matrix4x4::Multiply(m_pCamera->GetViewMatrix(), m_pCamera->GetProjectionMatrix());
	XMFLOAT2 res{ NAN, NAN };
	XMFLOAT3 s = XMFLOAT3(m_xmf4x4ToParent._41, m_xmf4x4ToParent._42, m_xmf4x4ToParent._43);
	XMFLOAT3 pos = Vector3::Add(s, XMFLOAT3(0.0f, 6.0f, 0.0f));

	float viewX = pos.x * viewProj._11 + pos.y * viewProj._21 + pos.z * viewProj._31 + viewProj._41;
	float viewY = pos.x * viewProj._12 + pos.y * viewProj._22 + pos.z * viewProj._32 + viewProj._42;
	float viewZ = pos.x * viewProj._13 + pos.y * viewProj._23 + pos.z * viewProj._33 + viewProj._43;

	res.x = (float)(viewX / viewZ + 1.0f) * (m_pCamera->GetViewport().Width * 0.5f);
	res.y = (float)(-viewY / viewZ + 1.0f) * (m_pCamera->GetViewport().Height * 0.5f);

	return res;
}

CTerrainPlayer::CTerrainPlayer(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, int matID, void *pContext)
{
	CLoadedModelInfo* pEvilBearModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature,
		"../Resource/Models/EvilBear.bin", NULL, true);
	
	SetChild(pEvilBearModel->m_pModelRootObject, true);
	m_pSkinningBoneTransforms = new CSkinningBoneTransforms(pd3dDevice, pd3dCommandList, pEvilBearModel);
	m_pFrameTransform = new CFrameTransform(pd3dDevice, pd3dCommandList, pEvilBearModel);

	m_pAnimationController = new CAnimationController(1, pEvilBearModel->m_pAnimationSets);
	m_pAnimationController->SetTrackAnimationSet(0, m_pAnimationController->IDLE);

	// RUNFAST번 애니메이션 동작에 사운드 2개를 Set해준다.
	m_pAnimationController->SetCallbackKeys(m_pAnimationController->RUNFAST, 2);
	// 애니메이션 1번동작 0.1초일때 Footstep01 소리를 재생, 1번동작 0.5초일때 Footstep02 소리를 재생, 1번동작 0.9초일때 Footstep03 소리를 재생
	m_pAnimationController->SetCallbackKey(m_pAnimationController->RUNFAST, 0, 0.1f, (void*)CSoundSystem::SOUND_TYPE::RUN1);
	m_pAnimationController->SetCallbackKey(m_pAnimationController->RUNFAST, 1, 0.4f, (void*)CSoundSystem::SOUND_TYPE::RUN2);

	m_pAnimationController->SetCallbackKeys(m_pAnimationController->RUNBACKWARD, 2);
	m_pAnimationController->SetCallbackKey(m_pAnimationController->RUNBACKWARD, 0, 0.1f, (void*)CSoundSystem::SOUND_TYPE::RUN1);
	m_pAnimationController->SetCallbackKey(m_pAnimationController->RUNBACKWARD, 1, 0.25f, (void*)CSoundSystem::SOUND_TYPE::RUN2);

	m_pAnimationController->SetCallbackKeys(m_pAnimationController->RAISEHAND, 1);
	m_pAnimationController->SetCallbackKey(m_pAnimationController->RAISEHAND, 0, 0.3f, (void*)CSoundSystem::SOUND_TYPE::GOLDTIMER_EFFECT);

	m_pAnimationController->SetCallbackKeys(m_pAnimationController->DIE, 1);
	m_pAnimationController->SetCallbackKey(m_pAnimationController->DIE, 0, 0.1f, (void*)CSoundSystem::SOUND_TYPE::BOMBEXPLOSION_EFFECT);

	m_pAnimationController->SetCallbackKeys(m_pAnimationController->ATTACK, 1);
	m_pAnimationController->SetCallbackKey(m_pAnimationController->ATTACK, 0, 0.2f, (void*)CSoundSystem::SOUND_TYPE::HAMMERSWING_EFFECT);

	m_pAnimationController->SetCallbackKeys(m_pAnimationController->USEGOLDHAMMER, 1);
	m_pAnimationController->SetCallbackKey(m_pAnimationController->USEGOLDHAMMER, 0, 0.3f, (void*)CSoundSystem::SOUND_TYPE::GOLDHAMMER_EFFECT);

	m_SoundChannel = 6;

	CAnimationCallbackHandler* pRunAnimationCallbackHandler = new CSoundCallbackHandler;
	m_pAnimationController->SetAnimationCallbackHandler(m_pAnimationController->RUNFAST, pRunAnimationCallbackHandler);

	CAnimationCallbackHandler* pBackRunAnimationCallbackHandler = new CSoundCallbackHandler;
	m_pAnimationController->SetAnimationCallbackHandler(m_pAnimationController->RUNBACKWARD, pBackRunAnimationCallbackHandler);

	CAnimationCallbackHandler* pRaiseHandAnimationCallbackHandler = new CSoundCallbackHandler;
	m_pAnimationController->SetAnimationCallbackHandler(m_pAnimationController->RAISEHAND, pRaiseHandAnimationCallbackHandler);
	
	CAnimationCallbackHandler* pDieAnimationCallbackHandler = new CSoundCallbackHandler;
	m_pAnimationController->SetAnimationCallbackHandler(m_pAnimationController->DIE, pDieAnimationCallbackHandler);

	CAnimationCallbackHandler* pAttackAnimationCallbackHandler = new CSoundCallbackHandler;
	m_pAnimationController->SetAnimationCallbackHandler(m_pAnimationController->ATTACK, pAttackAnimationCallbackHandler);

	CAnimationCallbackHandler* pUseGoldHammerCallbackHandler = new CSoundCallbackHandler;
	m_pAnimationController->SetAnimationCallbackHandler(m_pAnimationController->USEGOLDHAMMER, pUseGoldHammerCallbackHandler);

	m_pCamera = ChangeCamera(THIRD_PERSON_CAMERA, 0.0f);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	SetPlayerUpdatedContext(pContext);
	SetCameraUpdatedContext(pContext);

	m_matID = matID;

	m_ID = "<EvilBear>";

	m_pShadow = new CShadow(pEvilBearModel, this);

	if (pEvilBearModel)
		delete pEvilBearModel;

	m_TimerThread = thread{ &CPlayer::ProcessEvent, this };
}

CTerrainPlayer::~CTerrainPlayer()
{
}

void CTerrainPlayer::Animate(float fTimeElapsed)
{
#ifndef _WITH_SERVER_
	RotateAxisY(fTimeElapsed);
#endif
	CGameObject::Animate(fTimeElapsed);
}

void CPlayer::UpdateTransform(XMFLOAT4X4 *pxmf4x4Parent,bool isLocalFrameRotate)
{
	m_xmf4x4World = (pxmf4x4Parent) ? Matrix4x4::Multiply(m_xmf4x4ToParent, *pxmf4x4Parent) : m_xmf4x4ToParent;

	if (m_pSibling)
		m_pSibling->UpdateTransform(pxmf4x4Parent,m_bLocalRotation);
	if (m_pChild)
		m_pChild->UpdateTransform(&m_xmf4x4World,m_bLocalRotation);
}

CCamera *CTerrainPlayer::ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed)
{
	DWORD nCurrentCameraMode = (m_pCamera) ? m_pCamera->GetMode() : 0x00;
	if (nCurrentCameraMode == nNewCameraMode) return(m_pCamera);
	switch (nNewCameraMode)
	{
	case FIRST_PERSON_CAMERA:
		SetFriction(250.0f);
		SetGravity(XMFLOAT3(0.0f, -400.0f, 0.0f));
		SetMaxVelocityXZ(300.0f);
		SetMaxVelocityY(400.0f);
		m_pCamera = OnChangeCamera(FIRST_PERSON_CAMERA, nCurrentCameraMode);
		m_pCamera->SetTimeLag(0.0f);
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 20.0f, 0.0f));
		m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
		m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
		m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
		break;
	case SPACESHIP_CAMERA:
		SetFriction(125.0f);
		SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));
		SetMaxVelocityXZ(300.0f);
		SetMaxVelocityY(400.0f);
		m_pCamera = OnChangeCamera(SPACESHIP_CAMERA, nCurrentCameraMode);
		m_pCamera->SetTimeLag(0.0f);
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 0.0f, 0.0f));
		m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
		m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
		m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
		break;
	case THIRD_PERSON_CAMERA:
		SetFriction(250.0f);
		SetGravity(XMFLOAT3(0.0f, -250.0f, 0.0f));
		SetMaxVelocityXZ(40.0f);
		SetMaxVelocityY(400.0f);
		m_pCamera = OnChangeCamera(THIRD_PERSON_CAMERA, nCurrentCameraMode);
		m_pCamera->SetTimeLag(0.3f);
		// 카메라 위치
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 10.0f, -20.0f));
		m_pCamera->SetPosition(Vector3::Add(m_xmf3Position, m_pCamera->GetOffset()));
		m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
		m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
		m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
		break;
	default:
		break;
	}
	Update(fTimeElapsed);

	return(m_pCamera);
}

void CTerrainPlayer::OnPlayerUpdateCallback(float fTimeElapsed)
{
	CTerrain *pTerrain = (CTerrain *)m_pPlayerUpdatedContext;
	XMFLOAT3 xmf3Scale = pTerrain->GetScale();
	XMFLOAT3 xmf3PlayerPosition = GetPosition();
	int z = (int)(xmf3PlayerPosition.z / xmf3Scale.z);
	bool bReverseQuad = ((z % 2) != 0);
	// 플레이어 올라가는거 주석처리
	//float fHeight = pTerrain->GetHeight(xmf3PlayerPosition.x, xmf3PlayerPosition.z, bReverseQuad) + 0.0f;
	float fHeight = 0.f;
	if (xmf3PlayerPosition.y < fHeight)
	{
		XMFLOAT3 xmf3PlayerVelocity = GetVelocity();
		xmf3PlayerVelocity.y = 0.0f;
		SetVelocity(xmf3PlayerVelocity);
		xmf3PlayerPosition.y = fHeight;
		SetPosition(xmf3PlayerPosition);
	}
}

void CTerrainPlayer::OnCameraUpdateCallback(float fTimeElapsed)
{
	CTerrain *pTerrain = (CTerrain *)m_pCameraUpdatedContext;
	XMFLOAT3 xmf3Scale = pTerrain->GetScale();
	XMFLOAT3 xmf3CameraPosition = m_pCamera->GetPosition();
	int z = (int)(xmf3CameraPosition.z / xmf3Scale.z);
	bool bReverseQuad = ((z % 2) != 0);
	float fHeight = pTerrain->GetHeight(xmf3CameraPosition.x, xmf3CameraPosition.z, bReverseQuad) + 5.0f;
	if (xmf3CameraPosition.y <= fHeight)
	{
		// 카메라 올라가는거 주석처리
		//xmf3CameraPosition.y = fHeight;
		m_pCamera->SetPosition(xmf3CameraPosition);
		if (m_pCamera->GetMode() == THIRD_PERSON_CAMERA)
		{
			CThirdPersonCamera *p3rdPersonCamera = (CThirdPersonCamera *)m_pCamera;
			p3rdPersonCamera->SetLookAt(GetPosition());
		}
		
	}
}

