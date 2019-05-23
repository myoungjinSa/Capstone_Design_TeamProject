#include "../../Stdafx/Stdafx.h"
#include "Player.h"
#include "../Terrain/Terrain.h"
#include "../../Material/Material.h"
#include "../../Shader/Shader.h"
#include "../Item/Item.h"
#include "../Shadow/Shadow.h"

#include "../../SoundSystem/SoundSystem.h"
#include "../../ShaderManager/ShaderManager.h"
#include "../../Shader/BillboardShader/UIShader/TimerUIShader/TimerUIShader.h"
#include "../../Shader/BillboardShader/BombParticleShader/BombParticleShader.h"
#include "../Billboard/Bomb/Bomb.h"
#include "../../FrameTransform/FrameTransform.h"
#include "../../Chatting/Chatting.h"
#include "../../Shader/BillboardShader/UIShader/TextUIShader/OutcomeUIShader/OutcomeUIShader.h"
#include "../../Network/Network.h"

extern byte g_PlayerCharacter;

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

	InitializeSound();
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

	ReleaseSound();
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
	
	DecideAnimationState(fLength);
#else

	DWORD nCurrentCameraMode = m_pCamera->GetMode();
	if (nCurrentCameraMode == THIRD_PERSON_CAMERA) m_pCamera->Update(m_xmf3Position, fTimeElapsed,IsCameraVibe());
	if (m_pCameraUpdatedContext) OnCameraUpdateCallback(fTimeElapsed);
	if (nCurrentCameraMode == THIRD_PERSON_CAMERA) m_pCamera->SetLookAt(m_xmf3Position);

	m_pCamera->RegenerateViewMatrix();

	
	//std::cout <<"서버에서 받은 속도: "<< m_fVelocityFromServer << "\n";
	DecideAnimationState(m_fVelocityFromServer);

#endif
	m_Time += fTimeElapsed;
	if (m_Time > 1.f)
	{
		m_Score += 100;
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
			CGameObject::RunAway_Render(pd3dCommandList, pCamera, m_matID,m_bIce, m_bHammer, m_bGoldHammer, GameObject);
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
			CGameObject::RunAway_Render(pd3dCommandList, pCamera, g_PlayerCharacter, m_bIce, m_bHammer, m_bGoldHammer, GameObject);
			if (m_pShadow)
				m_pShadow->RunAway_Render(pd3dCommandList, pCamera, g_PlayerCharacter, m_bIce, m_bHammer, m_bGoldHammer, GameObject_Shadow);
		}

#endif
	}
}

void CPlayer::Add_Inventory(string key, int ItemType)
{
	if (ItemType == CItem::NormalHammer)
	{
		if (m_Normal_Inventory.size() > 0)
			Refresh_Inventory(ItemType);
		CItem* pItem = new CItem;
		pItem->setItemType(ItemType);

		m_bHammer = true;

		m_Normal_Inventory.emplace(key, pItem);
	}
	else
	{
		if (m_Special_Inventory.size() > 0)
			Refresh_Inventory(ItemType);
		CItem* pItem = new CItem;
		pItem->setItemType(ItemType);

		if (ItemType == CItem::GoldHammer)
			m_bGoldHammer = true;
		else
			m_bGoldTimer = true;
		m_Special_Inventory.emplace(key, pItem);
	}
}

void CPlayer::Refresh_Inventory(int ItemType)
{
	if (ItemType == CItem::NormalHammer)
	{
		if(m_bHammer)
			m_bHammer = false;
		for (auto iter = m_Normal_Inventory.begin(); iter != m_Normal_Inventory.end();)
		{
			m_RemovedItemList.emplace_back((*iter).second);
			iter = m_Normal_Inventory.erase(iter);
		}
	}
	// 특수 아이템
	else
	{
		for (auto iter = m_Special_Inventory.begin(); iter != m_Special_Inventory.end();)
		{
			if (ItemType == CItem::GoldHammer)
				m_bGoldHammer = false;
			else
				m_bGoldTimer = false;

			m_RemovedItemList.emplace_back((*iter).second);
			iter = m_Special_Inventory.erase(iter);
		}
	}
}

//플레이어의 애니메이션을 결정
void CPlayer::DecideAnimationState(float fLength)
{
	CAnimationController* pController = m_pAnimationController;

	
	if (fLength == 0.0f
		&& (pController->GetAnimationState() != CAnimationController::ATTACK
			&& pController->GetAnimationState() != CAnimationController::DIGGING
			&& pController->GetAnimationState() != CAnimationController::JUMP
			&& pController->GetAnimationState() != CAnimationController::RAISEHAND
			&& pController->GetAnimationState() != CAnimationController::DIE
			&& pController->GetAnimationState() != CAnimationController::ICE
			&& pController->GetAnimationState() != CAnimationController::SLIDE
			&& pController->GetAnimationState() != CAnimationController::USEGOLDHAMMER))
	{
		if (pController->GetAnimationState() == CAnimationController::RUNFAST)
		{
			m_pAnimationController->SetTrackPosition(0, 0.0f);

		}


		SetTrackAnimationSet(0, CAnimationController::IDLE);
		m_pAnimationController->SetAnimationState(CAnimationController::IDLE);
#ifdef _WITH_SERVER_
		Network::GetInstance()->SendAnimationState(CAnimationController::IDLE);
#endif
	}
	else
	{
		if (GetAsyncKeyState(VK_UP) & 0x8000
			&& pController->GetAnimationState() != CAnimationController::ATTACK
			&& pController->GetAnimationState() != CAnimationController::JUMP
			&& pController->GetAnimationState() != CAnimationController::ICE
			&& pController->GetAnimationState() != CAnimationController::RAISEHAND
			&& pController->GetAnimationState() != CAnimationController::DIE
			)
		{
			
			SetTrackAnimationSet(0, CAnimationController::RUNFAST);
			m_pAnimationController->SetAnimationState(CAnimationController::RUNFAST);
#ifdef _WITH_SERVER_
		Network::GetInstance()->SendAnimationState(CAnimationController::RUNFAST);
#endif

			//m_pAnimationController->SetTrackSpeed(0, 1.3f);
			//m_pAnimationController->SetTrackPosition(0, 0.0f);
		}
#ifdef _WITH_SERVER_
		
		//서버와 연동시 이동과 회전을 동시에 처리가 안되서 
		// 서버에서 fLength = 0.001의 속도를 부여하고 그속도로 
		//이동과 회전을 동시에 처리함. 
		// 그러다보니 제자리에서 회전 했을때도 달리는 애니메이션이 발생해서
		// 아래와 같은 처리를 하였음 - 명진
		if ((GetAsyncKeyState(VK_RIGHT) &0x8000
			|| GetAsyncKeyState(VK_LEFT) &0x8000)
			&& !(GetAsyncKeyState(VK_UP) & 0x8000)
			&&( pController->GetAnimationState() == CAnimationController::RUNFAST
			|| pController->GetAnimationState() == CAnimationController::RUNBACKWARD)
			)
		{
			SetTrackAnimationSet(0, CAnimationController::IDLE);
			m_pAnimationController->SetAnimationState(CAnimationController::IDLE);
			Network::GetInstance()->SendAnimationState(CAnimationController::IDLE);

		}

#endif
		if (GetAsyncKeyState(VK_DOWN) & 0x8000
			&& pController->GetAnimationState() != CAnimationController::ATTACK
			&& pController->GetAnimationState() != CAnimationController::JUMP
			&& pController->GetAnimationState() != CAnimationController::ICE
			&& pController->GetAnimationState() != CAnimationController::RAISEHAND
			&& pController->GetAnimationState() != CAnimationController::DIE
			)
		{
			m_pAnimationController->SetAnimationState(CAnimationController::RUNBACKWARD);
			SetTrackAnimationSet(0, CAnimationController::RUNBACKWARD);
#ifdef _WITH_SERVER_
			Network::GetInstance()->SendAnimationState(CAnimationController::RUNBACKWARD);
#endif
		}


	}
	if (GetAsyncKeyState(VK_SPACE) & 0x8000
		&& pController->GetAnimationState() != CAnimationController::JUMP
		&& pController->GetAnimationState() != CAnimationController::ICE
		&& ChattingSystem::GetInstance()->IsChattingActive() ==false
		)
	{
		SetTrackAnimationSet(0, CAnimationController::JUMP);
		SetTrackAnimationPosition(0, 0);
		pController->SetAnimationState(CAnimationController::JUMP);
#ifdef _WITH_SERVER_
			Network::GetInstance()->SendAnimationState(CAnimationController::JUMP);
#endif
		//pController->SetTrackSpeed(0, 1.5f);
	}

	//if (GetAsyncKeyState(VK_Z) & 0x8000
	//	&& pController->GetAnimationState() != CAnimationController::DIGGING
	//	&& pController->GetAnimationState() != CAnimationController::ICE
	//	&& ChattingSystem::GetInstance()->IsChattingActive() ==false
	//	)
	//{
	//	SetTrackAnimationSet(0, CAnimationController::DIGGING);
	//	SetTrackAnimationPosition(0, 0);

	//	pController->SetAnimationState(CAnimationController::DIGGING);
	//}

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
		{
			auto iter = m_pShaderManager->getShaderMap().find("TimerUI");
			if (iter != m_pShaderManager->getShaderMap().end())
			{
				((CTimerUIShader*)(*iter).second)->setTimer(10.f);
			}
		}
	}
	if (GetAsyncKeyState(VK_R) & 0x0001)
	{
		if (m_pShaderManager)
		{
			auto iter = m_pShaderManager->getShaderMap().find("TimerUI");
			if (iter != m_pShaderManager->getShaderMap().end())
			{
				((CTimerUIShader*)(*iter).second)->setReduceTimer(10.f);
			}
		}
	}

	if(GetAsyncKeyState(VK_X) & 0x0001
		&& pController->GetAnimationState() != CAnimationController::ICE
		&& ChattingSystem::GetInstance()->IsChattingActive() ==false)
	{
		pController->SetTrackAnimationSet(0, CAnimationController::SLIDE);
		pController->SetAnimationState(CAnimationController::SLIDE);
		//pController->SetTrackSpeed(0, 10.0f);
	}

	//if (GetAsyncKeyState(VK_RSHIFT) & 0x0001 
	//	&& pController->GetAnimationState() != CAnimationController::DIE
	//	&& ChattingSystem::GetInstance()->IsChattingActive() ==false
	//	)
	//{
	//	pController->SetTrackPosition(0, 0.0f);
	//	pController->SetTrackAnimationSet(0, CAnimationController::DIE);
	//	pController->SetAnimationState(CAnimationController::DIE);
	//}

	////얼음으로 변신
	if (GetAsyncKeyState(VK_A) & 0x0001 && ChattingSystem::GetInstance()->IsChattingActive() ==false && m_bBomb == false)
	{
		m_bIce = !m_bIce;
		pController->SetTrackAnimationSet(0, CAnimationController::IDLE);
		if (m_bIce == true)
			pController->SetAnimationState(CAnimationController::ICE);
		else
			pController->SetAnimationState(CAnimationController::IDLE);
	}

	// 망치로 때리기 애니메이션
	if (GetAsyncKeyState(VK_CONTROL) & 0x0001
		&& pController->GetAnimationState() != CAnimationController::ATTACK
		&& pController->GetAnimationState() != CAnimationController::ICE
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
			//Refresh_Inventory(CItem::NormalHammer);
	}

	// 특수 아이템 사용 버튼(ALT)
	if (GetAsyncKeyState(VK_MENU) & 0x0001)
	{
		if (m_Special_Inventory.size() > 0)
		{
			auto iter = m_Special_Inventory.begin();
			if ((*iter).second->getItemType() == CItem::GoldHammer)
			{	
				SetTrackAnimationSet(0, CAnimationController::RAISEHAND);
				SetTrackAnimationPosition(0, 0.0f);
				//m_pAnimationController->SetTrackSpeed(0, 2.0f);
				pController->SetAnimationState(CAnimationController::RAISEHAND);
#ifdef _WITH_SERVER_
			Network::GetInstance()->SendAnimationState(CAnimationController::RAISEHAND);
#endif
				Refresh_Inventory((*iter).second->getItemType());
			}
			else
			{
				if (m_pShaderManager && m_bBomb == true)
				{
					auto iter2 = m_pShaderManager->getShaderMap().find("TimerUI");
					if (iter2 != m_pShaderManager->getShaderMap().end())
					{
						SetTrackAnimationSet(0, CAnimationController::RAISEHAND);
						SetTrackAnimationPosition(0, 0.0f);
						pController->SetAnimationState(CAnimationController::RAISEHAND);
#ifdef _WITH_SERVER_
			Network::GetInstance()->SendAnimationState(CAnimationController::RAISEHAND);
#endif
						// 30초 증가
						dynamic_cast<CTimerUIShader*>((*iter2).second)->setTimer(30.f);

						Refresh_Inventory((*iter).second->getItemType());
						
					}
				}
			}	
		}
	}

	//// 폭탄이 있을 때,
	if (m_bBomb == true)
	{
		if (m_pShaderManager)
		{
			if (pController->GetAnimationState() != CAnimationController::DIE)
			{
				auto iter = m_pShaderManager->getShaderMap().find("TimerUI");
			
				//auto outcomeIter = m_pShaderManager->getShaderMap().find("OutcomeUI");
				if (iter != m_pShaderManager->getShaderMap().end())
				{
					if (((CTimerUIShader*)((*iter).second))->getTimer() <= 0.f)
					{
						auto outcomeIter = m_pShaderManager->getShaderMap().find("OutcomeUI");
						auto iter2 = m_pShaderManager->getShaderMap().find("Bomb");
						
						if (iter2 != m_pShaderManager->getShaderMap().end()
							&& outcomeIter != m_pShaderManager->getShaderMap().end())
						{
							m_BombParticle = ((CBombParticleShader*)(*iter2).second)->getBomb();
							m_BombParticle->setIsBlowing(true);
							m_bBomb = false;

							if (((COutcomeUIShader*)(*outcomeIter).second)->getIsRender() == false)
								((COutcomeUIShader*)(*outcomeIter).second)->setIsRender(true);
							else
								((COutcomeUIShader*)(*outcomeIter).second)->setIsRender(false);
							pController->SetTrackPosition(0, 0.0f);
							pController->SetTrackAnimationSet(0, CAnimationController::DIE);
							pController->SetAnimationState(CAnimationController::DIE);
						}
					}
				}
			}
		}
	}
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

void CPlayer::InitializeSound()
{
	m_pSound = new CSoundSystem;

	m_SoundCount = 4;

	m_SoundList = new const char*[m_SoundCount];

	m_SoundList[0] = "../Resource/Sound/BtnDown03.wav";
	m_SoundList[1] = "../Resource/Sound/bell1.wav";
	m_SoundList[2] = "../Resource/Sound/BombExplode2.wav";
	m_SoundList[3] = "../Resource/Sound/Effect/HammerSwing.wav";
	

	std::string s0(m_SoundList[0]);
	std::string s1(m_SoundList[1]);
	std::string s2(m_SoundList[2]);
	std::string s3(m_SoundList[3]);


	////m_SoundList[1] = "../Resource/Sound/bell1.wav";

	m_mapMusicList.emplace(FOOTSTEP, s0);
	m_mapMusicList.emplace(USETIMER, s1);
	m_mapMusicList.emplace(DIE, s2);
	m_mapMusicList.emplace(ATTACK, s3);
	
	if (m_pSound)
		m_pSound->Initialize(m_SoundCount, m_SoundList, FMOD_LOOP_OFF);
}

void CPlayer::ReleaseSound()
{
	if (m_pSound)
		m_pSound->Release();
}

void CPlayer::ChangeRole()
{
	if (m_Normal_Inventory.size() > 0)
		Refresh_Inventory(CItem::NormalHammer);
	if (m_Special_Inventory.size() > 0)
		Refresh_Inventory(CItem::GoldHammer);
	if (m_Special_Inventory.size() > 0)
		Refresh_Inventory(CItem::GoldTimer);

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
	m_pAnimationController->SetCallbackKey(m_pAnimationController->RUNFAST, 0, 0.1f, (void*)CPlayer::MUSIC_ENUM::FOOTSTEP);
	m_pAnimationController->SetCallbackKey(m_pAnimationController->RUNFAST, 1, 0.5f, (void*)CPlayer::MUSIC_ENUM::FOOTSTEP);


	m_pAnimationController->SetCallbackKeys(m_pAnimationController->RUNBACKWARD, 2);
	m_pAnimationController->SetCallbackKey(m_pAnimationController->RUNBACKWARD, 0, 0.1f, (void*)CPlayer::MUSIC_ENUM::FOOTSTEP);
	m_pAnimationController->SetCallbackKey(m_pAnimationController->RUNBACKWARD, 1, 0.3f, (void*)CPlayer::MUSIC_ENUM::FOOTSTEP);


	m_pAnimationController->SetCallbackKeys(m_pAnimationController->RAISEHAND, 1);
	m_pAnimationController->SetCallbackKey(m_pAnimationController->RAISEHAND, 0, 0.3f, (void*)CPlayer::MUSIC_ENUM::USETIMER);

	m_pAnimationController->SetCallbackKeys(m_pAnimationController->DIE, 1);
	m_pAnimationController->SetCallbackKey(m_pAnimationController->DIE, 0, 0.1f, (void*)CPlayer::MUSIC_ENUM::DIE);

	m_pAnimationController->SetCallbackKeys(m_pAnimationController->ATTACK, 1);
	m_pAnimationController->SetCallbackKey(m_pAnimationController->ATTACK, 0, 0.2f, (void*)CPlayer::MUSIC_ENUM::ATTACK);



	CAnimationCallbackHandler* pRunAnimationCallbackHandler = new CSoundCallbackHandler();
	m_pAnimationController->SetAnimationCallbackHandler(m_pAnimationController->RUNFAST, pRunAnimationCallbackHandler,
		GetSoundData());

	CAnimationCallbackHandler* pBackRunAnimationCallbackHandler = new CSoundCallbackHandler();
	m_pAnimationController->SetAnimationCallbackHandler(m_pAnimationController->RUNBACKWARD, pBackRunAnimationCallbackHandler,
		GetSoundData());

	CAnimationCallbackHandler* pRaiseHandAnimationCallbackHandler = new CSoundCallbackHandler();
	m_pAnimationController->SetAnimationCallbackHandler(m_pAnimationController->RAISEHAND, pRaiseHandAnimationCallbackHandler,
		GetSoundData());
	
	CAnimationCallbackHandler* pDieAnimationCallbackHandler = new CSoundCallbackHandler();
	m_pAnimationController->SetAnimationCallbackHandler(m_pAnimationController->DIE, pDieAnimationCallbackHandler,
		GetSoundData());

	CAnimationCallbackHandler* pAttackAnimationCallbackHandler = new CSoundCallbackHandler();
	m_pAnimationController->SetAnimationCallbackHandler(m_pAnimationController->ATTACK, pAttackAnimationCallbackHandler,
		GetSoundData());

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
		m_pCamera->SetTimeLag(0.7f);
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

