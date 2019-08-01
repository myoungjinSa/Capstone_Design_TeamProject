#include "../../Stdafx/Stdafx.h"
#include "EvilBear.h"
#include <vector>
#include "../Shadow/Shadow.h"
#include "../../Material/Material.h"
#include "../../Shader/Shader.h"
#include "../../FrameTransform/FrameTransform.h"
#include "../Billboard/Bomb/Bomb.h"
#include "../../Network/Network.h"


CEvilBear::CEvilBear(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature,int matID)
{
	m_matID = matID;
	m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);
}

CEvilBear::~CEvilBear()
{
	if (m_pShadow)
		delete m_pShadow;
}


void CEvilBear::Initialize_Shadow(CLoadedModelInfo* pLoadedModel, CGameObject* pGameObject)
{
	m_pShadow = new CShadow(pLoadedModel, this);
}

void CEvilBear::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera* pCamera,int matID, int nPipelineState)
{
	if (IsVisible(pCamera) == true)
	{
		if (m_bBomb == true)
		{
			CGameObject::Tagger_Render(pd3dCommandList, pCamera, matID, m_bGoldTimer, GameObject);
			if (m_pShadow)
				m_pShadow->Tagger_Render(pd3dCommandList, pCamera, matID, m_bGoldTimer, GameObject_Shadow);
		}
		else
		{
			CGameObject::RunAway_Render(pd3dCommandList, pCamera, matID, m_bIce, m_bHammer, m_bGoldHammer,m_bLightening ,nPipelineState);
			if (m_pShadow)
				m_pShadow->RunAway_Render(pd3dCommandList, pCamera, matID, m_bIce, m_bHammer, m_bGoldHammer, GameObject_Shadow);
		}

	}
}

void CEvilBear::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera* pCamera, int nPipelineState)
{
	if (IsVisible(pCamera) == true)
	{
		if (m_bBomb == true)
		{
			CGameObject::Tagger_Render(pd3dCommandList, pCamera, m_matID, m_bGoldTimer, GameObject);
			if (m_pShadow)
				m_pShadow->Tagger_Render(pd3dCommandList, pCamera, m_matID, m_bGoldTimer, GameObject_Shadow);
		}
		else
		{
			CGameObject::RunAway_Render(pd3dCommandList, pCamera, m_matID, m_bIce, m_bHammer, m_bGoldHammer,m_bLightening ,nPipelineState);
			if (m_pShadow)
				m_pShadow->RunAway_Render(pd3dCommandList, pCamera, m_matID, m_bIce, m_bHammer, m_bGoldHammer, GameObject_Shadow);
		}

	}
}

void CEvilBear::Move(const XMFLOAT3& xmf3Shift,bool bUpdateVelocity)
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

	}
}
void CEvilBear::Animate(float fTimeElapsed)
{
	// 루트 오브젝트를 제외한 나머지는 모두 nullptr이다. 왜냐하면, 루트 오브젝트를 제어하기 위함이므로
	if (m_pAnimationController)
		m_pAnimationController->AdvanceTime(fTimeElapsed, (float*)&m_fDistanceToTarget);

	m_xmOOBBTransformed.Transform(m_xmOOBB, XMLoadFloat4x4(&m_xmf4x4World));
	XMStoreFloat4(&m_xmOOBBTransformed.Orientation, XMQuaternionNormalize(XMLoadFloat4(&m_xmOOBBTransformed.Orientation)));



	if (m_pSibling)
		m_pSibling->Animate(fTimeElapsed);
	if (m_pChild)
		m_pChild->Animate(fTimeElapsed);
}
void CEvilBear::RotateAndMove(float fTimeElapsed)
{
	if (m_keyType == SC_PRESS_UP_KEY && m_bIsMoving == true)
	{
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, m_xmf3Look, 0.7f);
		
	}
	if (m_keyType == SC_PRESS_DOWN_KEY && m_bIsMoving == true)
	{
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, m_xmf3Look, -0.7f);
	}
	if((m_keyType == SC_PRESS_LEFT_KEY || m_keyType == SC_PRESS_RIGHT_KEY)
		&& m_bIsRotating == true)
	{
		RotateAxisY(fTimeElapsed, m_keyType);
	}
	Move(m_xmf3Velocity, false);
	
	m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);

	m_keyType = -1;
}

void CEvilBear::RotateAxisY(float fTimeElapsed, const char& keyType)
{
	XMFLOAT3& xmf3Look = m_xmf3Look;
	XMFLOAT3& xmf3Right = m_xmf3Right;
	XMFLOAT3& xmf3Up = m_xmf3Up;
	if (m_keyType == SC_PRESS_RIGHT_KEY)
	{
		float fDotProduct = Vector3::DotProduct(xmf3Look, xmf3Right);

		float fAngle = ::IsEqual(fDotProduct, 1.0f) ? 0.0f : ((fDotProduct > 1.0f) ? XMConvertToDegrees(acos(fDotProduct)) : 90.0f);


		XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up), XMConvertToRadians(fAngle*fTimeElapsed));
		m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
		m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);


		//SetDirection(0x00);
	}
	else if (m_keyType == SC_PRESS_LEFT_KEY)
	{
		float fDotProduct = Vector3::DotProduct(xmf3Look, xmf3Right);

		float fAngle = ::IsEqual(fDotProduct, 1.0f) ? 0.0f : ((fDotProduct > 1.0f) ? XMConvertToDegrees(acos(fDotProduct)) : 90.0f);

		XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up), XMConvertToRadians(-(fAngle*fTimeElapsed)));
		m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
		m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);

		//SetDirection(0x00);
	}
}