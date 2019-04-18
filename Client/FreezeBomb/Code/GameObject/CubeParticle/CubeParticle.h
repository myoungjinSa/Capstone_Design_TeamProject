#pragma once

#include "../GameObject.h"

class CCubeParticle : public CGameObject
{
public:
	CCubeParticle(int nMaterial);
	virtual ~CCubeParticle();

	virtual void Animate(float elapsedTime, CCamera* pCamera = nullptr);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState, int nInstance);

	bool	getBlowingUp()											const { return m_BlowingUp; }
	void	setBlowingUp(bool Blowing)						{ m_BlowingUp = Blowing; }
	void	setRotationSpeed(float RotationSpeed) { m_RotationSpeed = RotationSpeed; }
	void	setMovingSpeed(float MovingSpeed) { m_MovingSpeed = MovingSpeed; }
	void setDuration(float Duration) { m_Duration = Duration; }

	void PrepareExplosion();
	XMVECTOR RandomUintVectorOnSphere();

	void Initialize_Shadow(CGameObject* pGameObject);

private:
	bool					m_BlowingUp = false;
	float					m_RotationSpeed = 0.f;
	float					m_MovingSpeed = 0.f;
	float					m_Duration = 0.f;
	// ¿øÇü
	XMFLOAT3		m_SphereVector = XMFLOAT3(0.f, 0.f, 0.f);
	float					m_elapsedTime = 0.f;
};