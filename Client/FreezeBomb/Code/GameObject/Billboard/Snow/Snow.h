#pragma once

#include "../Billboard.h"

class CSnow : public CBillboard
{
public:
	CSnow(int nMaterial);
	virtual ~CSnow();

	virtual void Animate(float fTimeElapsed, CCamera *pCamera = NULL);

	void SetOffsetX(float offset) { m_offsetX = offset; }
	float GetOffsetX()	const { return m_offsetX; }

	void SetOffsetZ(float offset) { m_offsetZ = offset; }
	float GetOffsetZ()	const { return m_offsetZ; }

	void SetPositionXZ(float x, float z);
	void SetLookAt(XMFLOAT3& xmfTarget);

private:
	float						m_offsetX = 0.0f;
	float						m_offsetZ = 0.0f;
};