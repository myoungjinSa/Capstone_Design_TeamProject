#pragma once

#include "../GameObject.h"

class CCubeParticle : public CGameObject
{
public:
	CCubeParticle(int nMaterial);
	virtual ~CCubeParticle();

	virtual void Animate(float elapsedTime, CCamera* pCamera = nullptr);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState, int nInstance);

	bool	GetBlowingUp()											const { return m_BlowingUp; }
	void	SetBlowingUp(bool Blowing)						{ m_BlowingUp = Blowing; }
	void	SetRotationSpeed(float RotationSpeed) { m_RotationSpeed = RotationSpeed; }
	void	SetMovingSpeed(float MovingSpeed) { m_MovingSpeed = MovingSpeed; }
	void	SetDuration(float Duration) { m_Duration = Duration; }

	void	SetVelocity(XMFLOAT3 vel) { m_velocity = vel; }

	void PrepareExplosion();
	XMVECTOR RandomUintVectorOnSphere();

	void Initialize_Shadow(CGameObject* pGameObject);

private:
	bool					m_BlowingUp = false;
	float					m_RotationSpeed = 0.f;
	float					m_MovingSpeed = 0.f;
	float					m_Duration = 0.f;
	double					m_InverseMass = 0.0f;
	double					m_Damping = 0.0f;

	XMFLOAT3				m_forceAccum = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3				m_acceleration = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3				m_velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3				m_gravity = XMFLOAT3(0.0f, -1.0f, 0.0f);
	// ¿øÇü
	XMFLOAT3				m_SphereVector = XMFLOAT3(0.f, 0.f, 0.f);
	float					m_elapsedTime = 0.f;
};