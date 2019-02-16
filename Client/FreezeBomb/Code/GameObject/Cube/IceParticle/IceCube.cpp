#include "../../../Stdafx/Stdafx.h"
#include "../../../Shader/Shader.h"
#include "../../../Material/Material.h"
#include "IceCube.h"


XMFLOAT3 CIceCube::m_pxmf3SphereVectors[EXPLOSION_DEBRISES];
CMesh *CIceCube::m_pExplosionMesh = NULL;


CIceCube::CIceCube(int nMaterials)
	: CGameObject(nMaterials),
	m_xmf3RotationAxis(XMFLOAT3(0.0f, 1.0f, 0.0f)),
	m_fMovingSpeed(0.0f),
	m_bBlowingUp(false),
	m_fRotationSpeed(0.0f),
	m_fDuration(0.0f),
	m_fExplosionRotation(720.0f),
	m_fExplosionSpeed(10),
	m_fElapsedTimes(0.0f)
{

}

CIceCube::~CIceCube()
{

}

void CIceCube::SetExplosionDuration(float fDuration)
{
	m_fDuration = fDuration;
}
void CIceCube::SetRotationSpeed(float fRotationSpeed)
{
	m_fRotationSpeed = fRotationSpeed;
}

void CIceCube::SetRotationAxis(XMFLOAT3 xmf3RotationAxis)
{
	m_xmf3RotationAxis = xmf3RotationAxis;
}
void CIceCube::SetMovingSpeed(float fSpeed)
{
	m_fMovingSpeed = fSpeed;
}

void CIceCube::SetExplode(bool bBlowing)
{
	if (m_bBlowingUp == false)
	{
		m_bBlowingUp = bBlowing;
	}
}

void CIceCube::Animate(float fTimeElapsed)
{
	m_fElapsedTimes += fTimeElapsed;

	if (m_fElapsedTimes <= m_fDuration)
	{
		XMFLOAT3 xmf3Position = GetPosition();
		for (int i = 0; i < EXPLOSION_DEBRISES; i++)
		{
			m_pxmf4x4Transform[i] = Matrix4x4::Identity();
			m_pxmf4x4Transform[i]._41 = xmf3Position.x + m_pxmf3SphereVectors[i].x * m_fExplosionSpeed * m_fElapsedTimes;
			m_pxmf4x4Transform[i]._42 = xmf3Position.y + m_pxmf3SphereVectors[i].y * m_fExplosionSpeed * m_fElapsedTimes;
			m_pxmf4x4Transform[i]._43 = xmf3Position.z + m_pxmf3SphereVectors[i].z * m_fExplosionSpeed * m_fElapsedTimes;
			m_pxmf4x4Transform[i] = Matrix4x4::Multiply(Matrix4x4::RotationAxis(m_pxmf3SphereVectors[i], m_fExplosionRotation * m_fElapsedTimes), m_pxmf4x4Transform[i]);
		}
	}
	else
	{
		m_fElapsedTimes = 0.0f;
	}

}

void CIceCube::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	if (m_bBlowingUp)
	{
		for (int i = 0; i < EXPLOSION_DEBRISES; i++)
		{
			if (m_pMesh)
			{
				XMFLOAT4X4 xmf4x4World;
				XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_pxmf4x4Transform[i])));

				pd3dCommandList->SetGraphicsRoot32BitConstants(1, 16, &xmf4x4World, 0);


				if (m_nMaterials > 0)
				{
					for (int i = 0; i < m_nMaterials; i++)
					{
						if (m_ppMaterials[i])
						{
							if (m_ppMaterials[i]->m_pShader)
							{
								m_ppMaterials[i]->m_pShader->Render(pd3dCommandList,pCamera);
								//m_ppMaterials[i]->m_pShader->Render(pd3dCommandList, pCamera);
							}
							m_ppMaterials[i]->UpdateShaderVariables(pd3dCommandList);
						}

						m_pMesh->Render(pd3dCommandList, i);
					}
				}
			}
		}
	}

}


//////////////////////////////////////////////////////////////////////////////////
//static 함수
void CIceCube::PrepareExplosion(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	for (int i = 0; i < EXPLOSION_DEBRISES; i++)
	{
		XMStoreFloat3(&m_pxmf3SphereVectors[i], CIceCube::RandomUintVectorOnSphere());
	}
}

XMVECTOR CIceCube::RandomUintVectorOnSphere()
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

int CIceCube::GetRandom(int min, int max)
{
	//시드 설정
	std::random_device rn;

	default_random_engine rnd(rn());

	//분포 설정

	std::uniform_real_distribution<double> urd(min, max);

	return urd(rnd);
}

