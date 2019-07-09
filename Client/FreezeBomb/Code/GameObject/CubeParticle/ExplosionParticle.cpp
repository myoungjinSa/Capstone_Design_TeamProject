#include "../../Stdafx/Stdafx.h"
#include "ExplosionParticle.h"
#include "../Shadow/Shadow.h"

CExplosionParticle::CExplosionParticle()
	:CGameObject()
{

}

CExplosionParticle::~CExplosionParticle()
{
	if (m_pShadow)
		delete m_pShadow;
}

void CExplosionParticle::Initialize_Shadow(CGameObject* pGameObject)
{
	m_pShadow = new CShadow(nullptr, this);
}

void CExplosionParticle::SetMass(const double& mass)
{
	assert(mass != 0);
	m_InverseMass = ((double)1.0) / mass;
}

const double& CExplosionParticle::GetMass() const
{
	if (m_InverseMass == 0)
		return DBL_MAX;
	else
		return((double)1.0 / m_InverseMass);
}


void CExplosionParticle::Animate(float elapsedTime,CCamera* pCamera)
{
	m_elapsedTime += elapsedTime;

	if(m_elapsedTime <= m_Duration && m_BlowingUp == true)
	{
		XMFLOAT4X4 world = Matrix4x4::Identity();
		XMFLOAT3 pos = GetPosition();

		XMFLOAT3 gravity = m_acceleration;

		gravity = Vector3::Add(gravity, Vector3::ScalarProduct(gravity, m_InverseMass, false));

		world._41 = pos.x + (m_SphereVector.x * m_MovingSpeed * elapsedTime);
		world._42 = pos.y + (m_SphereVector.y * m_MovingSpeed * elapsedTime) + gravity.y;
		world._43 = pos.z + (m_SphereVector.z * m_MovingSpeed * elapsedTime);

		m_xmf4x4World = world;

		
		m_MovingSpeed*= pow(0.1f, m_elapsedTime);
	}
	else
	{
		m_elapsedTime = 0.f;
		m_BlowingUp = false;
	}
}

void CExplosionParticle::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState, int nInstance)
{
	if (m_BlowingUp == true)
	{
		switch (nPipelineState)
		{
		case GameObject:
			if (m_pMesh)
			{
				m_pMesh->Render(pd3dCommandList, 0, nInstance);
			}

			break;

		case GameObject_Shadow:
			if (m_pShadow)
				m_pShadow->Render(pd3dCommandList, pCamera, GameObject_Shadow, nInstance);
			break;
		default:
			break;
		}
	}
}

void CExplosionParticle::PrepareExplosion()
{
	XMStoreFloat3(&m_SphereVector, RandomUintVectorOnSphere());
}

XMVECTOR CExplosionParticle::RandomUintVectorOnSphere()
{
	XMVECTOR xmvOne = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	XMVECTOR xmvZero = XMVectorZero();

	while (true)
	{
		XMVECTOR v = XMVectorSet(RandF(-0.8f, 0.7f), RandF(-0.5f, 0.5f), RandF(-0.8f, 0.7f), 0.0f);

		if (!XMVector3Greater(XMVector3LengthSq(v), xmvOne))
		{
			return(XMVector3Normalize(v));
		}
	}
}