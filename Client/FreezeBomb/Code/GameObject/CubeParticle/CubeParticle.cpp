#include "../../Stdafx/Stdafx.h"
#include "CubeParticle.h"
#include "../../Material/Material.h"
#include "../../Shader/Shader.h"
#include "../Shadow/Shadow.h"

CCubeParticle::CCubeParticle(int nMaterial) : CGameObject(nMaterial)
{

}

CCubeParticle::~CCubeParticle()
{
	if (m_pShadow)
		delete m_pShadow;
}

void CCubeParticle::Initialize_Shadow(CGameObject* pGameObject)
{
	m_pShadow = new CShadow(nullptr, this);
}

void CCubeParticle::SetMass(const double& mass)
{
	assert(mass != 0);
	m_InverseMass = ((double)1.0) / mass;
}

const double& CCubeParticle::GetMass() const
{
	if (m_InverseMass == 0)
		return DBL_MAX;
	else
		return ((double)1.0 / m_InverseMass);
}
void CCubeParticle::Animate(float elapsedTime, CCamera* pCamera)
{
	m_elapsedTime += elapsedTime;
	//m_elapsedTime*=0.001f;



	if (m_elapsedTime <= m_Duration && m_BlowingUp == true)
	{
		XMFLOAT4X4 world = Matrix4x4::Identity();
		XMFLOAT3 position = GetPosition();

		//if (m_InverseMass < 0.0f) return;

		//Vector3::ScalarProduct(m_MovingSpeed, elapsedTime);
		

		//world = Matrix4x4::Multiply(Matrix4x4::RotationAxis(m_SphereVector, m_RotationSpeed * elapsedTime), world);
		//position = Vector3::Add(position, Vector3::ScalarProduct(m_velocity, m_elapsedTime,false));

		//cout << m_xmf4x4World._41 << "," << m_xmf4x4World._42 << "," << m_xmf4x4World._43 << endl;
		XMFLOAT3 resAcc = m_acceleration;
		resAcc = Vector3::Add(resAcc,Vector3::ScalarProduct(resAcc,m_InverseMass,false));

	/*	m_velocity = Vector3::Add(m_velocity,Vector3::ScalarProduct(resAcc, m_elapsedTime,false));
		m_velocity = Vector3::ScalarProduct(m_velocity,pow(0.1f, m_elapsedTime));
		SetPosition(position.x, position.y, position.z);*/


		world._41 = position.x + (m_SphereVector.x * m_MovingSpeed * elapsedTime);// +resAcc.y);
		world._42 = position.y + (m_SphereVector.y * m_MovingSpeed * elapsedTime) + resAcc.y; //+ resAcc.y);
		world._43 = position.z + (m_SphereVector.z * m_MovingSpeed * elapsedTime);//+ resAcc.y);
		if (world._42 <= 0.0f)
		{
			world._42 = 0.1f;
		}
	
		//SetPosition( XMFLOAT3(0.0f, -0.98f, 0.0f));
		m_xmf4x4World = world;

		
		m_MovingSpeed*= pow(0.1f, m_elapsedTime);
	}
	else
	{
		random_device rd;
		default_random_engine dre(rd());
		//default_random_engine dre;
		uniform_real_distribution<double> urd(10, 100);
		
		m_MovingSpeed =(float)urd(dre);
		m_elapsedTime = 0.f;
		m_BlowingUp = false;
	}
}

void CCubeParticle::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState, int nInstance)
{
	if (m_BlowingUp == true)
	{
		switch (nPipelineState)
		{
		case GameObject:
			if (m_pMesh)
			{
				if (m_nMaterials > 0)
				{
					for (int i = 0; i < m_nMaterials; i++)
					{
						if (m_ppMaterials[i])
						{
							if (m_ppMaterials[i]->m_pShader)
								m_ppMaterials[i]->m_pShader->Render(pd3dCommandList, pCamera, nPipelineState);

							m_ppMaterials[i]->UpdateShaderVariables(pd3dCommandList);
						}
						m_pMesh->Render(pd3dCommandList, i, nInstance);
					}
				}
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

void CCubeParticle::PrepareExplosion()
{
	XMStoreFloat3(&m_SphereVector, RandomUintVectorOnSphere());
}

XMVECTOR CCubeParticle::RandomUintVectorOnSphere()
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