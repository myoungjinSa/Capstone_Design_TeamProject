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

extern byte g_PlayerCharacter;
extern int g_State;

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
		if (dwDirection & DIR_FORWARD) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look, fDistance);
		if (dwDirection & DIR_BACKWARD) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look, -fDistance);
		//if (dwDirection & DIR_RIGHT) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right, fDistance);
		//if (dwDirection & DIR_LEFT) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right, -fDistance);
		if (dwDirection & DIR_UP) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Up, fDistance);
		if (dwDirection & DIR_DOWN) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Up, -fDistance);

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

void CPlayer::Update(float fTimeElapsed)
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
	if (m_dwDirection == DIR_FORWARD )
	{
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, m_xmf3Look, m_fVelocityFromServer);

	}
	if (m_dwDirection == DIR_BACKWARD)
	{
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, m_xmf3Look, -m_fVelocityFromServer);
	}


	Move(m_xmf3Velocity, false);
	
	m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_dwDirection = 0;
	//cout << m_xmf3Velocity.x << "," << m_xmf3Velocity.y << "," << m_xmf3Velocity.z << endl;

	//cout << m_xmf3Position.x << "," << m_xmf3Position.y << "," << m_xmf3Position.z << endl;

	DWORD nCurrentCameraMode = m_pCamera->GetMode();
	if (nCurrentCameraMode == THIRD_PERSON_CAMERA) 
		m_pCamera->Update(m_xmf3Position, fTimeElapsed, IsCameraVibe());
	if (m_pCameraUpdatedContext) 
		OnCameraUpdateCallback(fTimeElapsed);
	if (nCurrentCameraMode == THIRD_PERSON_CAMERA) 
		m_pCamera->SetLookAt(m_xmf3Position);

	m_pCamera->RegenerateViewMatrix();

	//std::cout <<"서버에서 받은 속도: "<< m_fVelocityFromServer << "\n";
	if (Network::GetInstance()->GetConnectState() == Network::CONNECT_STATE::OK) 
	{
		DecideAnimationState(m_fVelocityFromServer, fTimeElapsed);
	}
#endif
	m_Time += fTimeElapsed;
	if (m_Time > 1.f)
	{
		m_Time = 0.f;
	}
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
	static UINT idleCount = 0;
	static bool bRun = false;

		//cout << fLength << "\n";
	if (fLength == 0.0f 
		&& m_isMoveRotate == false
		&& bRun == true
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
		
		if (idleCount < 1) 
		{
			
			bRun = false;
			Network::GetInstance()->SendAnimationState(CAnimationController::IDLE);
		//	m_pAnimationController->SetAnimationState(CAnimationController::IDLE);
			idleCount++;
			SetTrackAnimationSet(0, CAnimationController::IDLE);
			m_pAnimationController->SetAnimationState(CAnimationController::IDLE);
		}
		
		
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
	else {
		if (GetAsyncKeyState(VK_UP) & 0x8000
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
				//idleCount = 0;
				//idleCount = 0;
				SetTrackAnimationPosition(0, 0.0f);
				SetTrackAnimationSet(0, CAnimationController::RUNFAST);
				m_pAnimationController->SetAnimationState(CAnimationController::RUNFAST);

				Network::GetInstance()->SendAnimationState(CAnimationController::RUNFAST);
				bRun = true;
				
			}
#else
			SetTrackAnimationSet(0, CAnimationController::RUNFAST);
			m_pAnimationController->SetAnimationState(CAnimationController::RUNFAST);
#endif

			//m_pAnimationController->SetTrackSpeed(0, 1.3f);
			//m_pAnimationController->SetTrackPosition(0, 0.0f);
		}
		else if (GetAsyncKeyState(VK_DOWN) & 0x8000
			&& pController->GetAnimationState() != CAnimationController::ATTACK
			&& pController->GetAnimationState() != CAnimationController::JUMP
			&& pController->GetAnimationState() != CAnimationController::ICE
			&& pController->GetAnimationState() != CAnimationController::RAISEHAND
			&& pController->GetAnimationState() != CAnimationController::DIE
			&& pController->GetAnimationState() != CAnimationController::VICTORY
			)
		{

#ifdef _WITH_SERVER_
			if (pController->GetAnimationState() != CAnimationController::RUNBACKWARD)
			{
				
				bRun = true;
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
			idleCount = 0;
		}

	}
	
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

	


	////얼음으로 변신
	if (GetAsyncKeyState(VK_A) & 0x0001 && ChattingSystem::GetInstance()->IsChattingActive() ==false && m_bBomb == false
		&& g_State == GAMESTATE::INGAME)
	{
		
#ifdef _WITH_SERVER_
		if (m_bIce == false)
		{
			//m_bIce = true;
			cout << "SendFreezeState()\n";
			Network::GetInstance()->SendFreezeState();
		}
		else
		{
			//m_bIce = false;
			Network::GetInstance()->SendReleaseFreezeState();
		}
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
		idleCount = 0;
		bRun = true;
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
			
			
			if (!m_bBomb ) {
#ifndef _WITH_SERVER_
				Sub_Inventory(GOLD_HAMMER);
			
#else
				idleCount = 0;
				Network::GetInstance()->SendUseItem(ITEM::GOLD_HAMMER, GetPlayerID());
				eraseTime = 0.0f;
#endif
			}
				//}
			m_bLocalRotation = false;
			m_bLightening = false;
			countCoolTime = false;

		}
	}
	
	// 특수 아이템 사용 버튼(ALT)
	if (GetAsyncKeyState(VK_MENU) & 0x0001
		&& pController->GetAnimationState() != CAnimationController::ATTACK
		&& pController->GetAnimationState() != CAnimationController::ICE
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
				//m_pAnimationController->SetTrackSpeed(0, 2.0f);
				pController->SetAnimationState(CAnimationController::USEGOLDHAMMER);
				m_bLocalRotation = true;
				
#ifdef _WITH_SERVER_
				idleCount = 0;
				bRun = true;
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
					bRun = true;
					Network::GetInstance()->SendUseItem(ITEM::GOLD_TIMER, GetPlayerID());
					idleCount = 0;
#else
					// 30초 증가
					CTimerUIShader::setTimer(30.f);
					Sub_Inventory(GOLD_TIMER);
#endif					
				}
			}	
		}
	}
//#ifndef _WITH_SERVER_
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
							pController->SetTrackPosition(0, 0.0f);
							pController->SetTrackAnimationSet(0, CAnimationController::DIE);
							pController->SetAnimationState(CAnimationController::DIE);
#ifdef _WITH_SERVER_
							Network::GetInstance()->SendAnimationState(CAnimationController::DIE);
							bRun = true;
							Network::GetInstance()->SendBombExplosion();
							idleCount = 0;
#endif		
							m_BombParticle = ((CBombParticleShader*)(*iter2).second)->getBomb();
							m_BombParticle->SetPosition(m_xmf3Position.x, m_xmf3Position.y, m_xmf3Position.z);
							m_BombParticle->setIsBlowing(true);

							//dynamic_cast<CExplosionParticleShader*>((*explosionIter).second)->SetBlowingUp(true);
							dynamic_cast<CExplosionParticleShader*>((*explosionIter).second)->SetParticleBlowUp(m_xmf3Position);
							m_bBomb = false;
						}
					}
				}
			}
		}
	}

			
//#endif
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

	CAnimationCallbackHandler* pRunAnimationCallbackHandler = new CSoundCallbackHandler;
	pRunAnimationCallbackHandler->SetAdditianalData(5);
	m_pAnimationController->SetAnimationCallbackHandler(m_pAnimationController->RUNFAST, pRunAnimationCallbackHandler);

	CAnimationCallbackHandler* pBackRunAnimationCallbackHandler = new CSoundCallbackHandler;
	pBackRunAnimationCallbackHandler->SetAdditianalData(5);
	m_pAnimationController->SetAnimationCallbackHandler(m_pAnimationController->RUNBACKWARD, pBackRunAnimationCallbackHandler);

	CAnimationCallbackHandler* pRaiseHandAnimationCallbackHandler = new CSoundCallbackHandler;
	pRaiseHandAnimationCallbackHandler->SetAdditianalData(5);
	m_pAnimationController->SetAnimationCallbackHandler(m_pAnimationController->RAISEHAND, pRaiseHandAnimationCallbackHandler);
	
	CAnimationCallbackHandler* pDieAnimationCallbackHandler = new CSoundCallbackHandler;
	pDieAnimationCallbackHandler->SetAdditianalData(5);
	m_pAnimationController->SetAnimationCallbackHandler(m_pAnimationController->DIE, pDieAnimationCallbackHandler);

	CAnimationCallbackHandler* pAttackAnimationCallbackHandler = new CSoundCallbackHandler;
	pAttackAnimationCallbackHandler->SetAdditianalData(5);
	m_pAnimationController->SetAnimationCallbackHandler(m_pAnimationController->ATTACK, pAttackAnimationCallbackHandler);

	CAnimationCallbackHandler* pUseGoldHammerCallbackHandler = new CSoundCallbackHandler;
	pUseGoldHammerCallbackHandler->SetAdditianalData(5);
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
}

CTerrainPlayer::~CTerrainPlayer()
{
}

void CTerrainPlayer::RotateAxisY(float fTimeElapsed)
{
	XMFLOAT3& xmf3Look = m_xmf3Look;
	XMFLOAT3& xmf3Right = m_xmf3Right;
	XMFLOAT3& xmf3Up = m_xmf3Up;
	if (m_dwDirection & DIR_RIGHT)
	{
		float fDotProduct = Vector3::DotProduct(xmf3Look, xmf3Right);

		float fAngle = ::IsEqual(fDotProduct, 1.0f) ? 0.0f : ((fDotProduct > 1.0f) ? XMConvertToDegrees(acos(fDotProduct)) : 90.0f);


		XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up), XMConvertToRadians(fAngle*fTimeElapsed));
		m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
		m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);


		SetDirection(0x00);
	}
	else if (m_dwDirection & DIR_LEFT)
	{
		float fDotProduct = Vector3::DotProduct(xmf3Look, xmf3Right);

		float fAngle = ::IsEqual(fDotProduct, 1.0f) ? 0.0f : ((fDotProduct > 1.0f) ? XMConvertToDegrees(acos(fDotProduct)) : 90.0f);

		XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up), XMConvertToRadians(-(fAngle*fTimeElapsed)));
		m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
		m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);

		SetDirection(0x00);
	}
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

