#pragma once

class CCube
{
public:
	CCube();
	CCube(float fWidth, float fHeight, float fDepth);
	virtual ~CCube();

	XMFLOAT3 GetPosition() { return m_CenterPos; }
	void UpdatePosition(const XMFLOAT3 pos, float fTimeLag);
	CCube& GetCubeObject() { return *this; }

	float GetDepth() { return m_fDepth; }
	float GetWidth() { return m_fWidth; }
	float GetHeight() { return m_fHeight; }

private:

	XMFLOAT3*	m_pVertex = NULL;
	int					m_nVertex;

	XMFLOAT3	m_CenterPos;
	float				m_fWidth, m_fHeight, m_fDepth;
};