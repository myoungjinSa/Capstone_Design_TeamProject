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

void CCubeParticle::Animate(float elapsedTime, CCamera* pCamera)
{
	m_elapsedTime += elapsedTime;

	if (m_elapsedTime <= m_Duration && m_BlowingUp == true)
	{
		XMFLOAT4X4 world = Matrix4x4::Identity();
		XMFLOAT3 position = GetPosition();

		world._41 = position.x + m_SphereVector.x * m_MovingSpeed * elapsedTime;
		world._42 = position.y + m_SphereVector.y * m_MovingSpeed * elapsedTime;
		world._43 = position.z + m_SphereVector.z * m_MovingSpeed * elapsedTime;
		world = Matrix4x4::Multiply(Matrix4x4::RotationAxis(m_SphereVector, m_RotationSpeed * elapsedTime), world);
		m_xmf4x4World = world;
	}
	else
	{
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
		XMVECTOR v = XMVectorSet(RandF(-1.0f, 1.0f), RandF(-1.0f, 1.0f), RandF(-1.0f, 1.0f), 0.0f);

		if (!XMVector3Greater(XMVector3LengthSq(v), xmvOne))
		{
			return(XMVector3Normalize(v));
		}
	}
}