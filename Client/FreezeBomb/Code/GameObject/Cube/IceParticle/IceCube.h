#pragma once
#include "../../GameObject.h"
//
class CMaterial;
class CShader;

class CIceCube : public CGameObject
{
public:
	CIceCube(int nMaterials);
	virtual ~CIceCube();

protected:
	XMFLOAT3		m_xmf3RotationAxis;

	bool			m_bBlowingUp;

	XMFLOAT4X4      m_pxmf4x4Transform[EXPLOSION_DEBRISES];

	float			m_fRotationSpeed;
	float			m_fElapsedTimes;
	float			m_fMovingSpeed;
	float			m_fDuration;
	float			m_fExplosionSpeed;
	float			m_fExplosionRotation;

	const int		maxParticleCount = 60;


public:
	static CMesh *m_pExplosionMesh;
	static XMFLOAT3 m_pxmf3SphereVectors[EXPLOSION_DEBRISES];

	static void PrepareExplosion(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	static XMVECTOR RandomUintVectorOnSphere();
	static int GetRandom(int min, int max);

public:
	void	SetRotationSpeed(float fRotationSpeed);
	void    SetRotationAxis(XMFLOAT3 xmf3RotationAxis);
	void	SetMovingSpeed(float fSpeed);
	void    SetExplosionDuration(float fDuration);		//Æø¹ß½Ã°£ set
	void	SetExplode(bool bBlowing);

	float   GetRotaionSpeed() { return m_fRotationSpeed; }

	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera, int nPipelineState);
};