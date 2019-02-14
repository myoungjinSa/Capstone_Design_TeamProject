#include "../Stdafx/Stdafx.h"
#include "Cube.h"

CCube::CCube()
{
}

CCube::CCube(float fWidth, float fHeight, float fDepth)
{
	m_nVertex = 8;
	m_pVertex = new XMFLOAT3[m_nVertex];

	m_CenterPos = XMFLOAT3(0.0f, 0.0f, 0.0f);

	float fx = fWidth * 0.5f;
	float fy = fHeight * 0.5f;
	float fz = fDepth * 0.5f;

	m_fWidth = fWidth;
	m_fHeight = fHeight;
	m_fDepth = fDepth;

	m_pVertex[0] = XMFLOAT3(-fx, +fy, +fz);			//left-top
	m_pVertex[1] = XMFLOAT3(+fx, +fy, +fz);			//right-top
	m_pVertex[2] = XMFLOAT3(-fx, -fy, +fz);			//left-Bottom
	m_pVertex[3] = XMFLOAT3(+fx, -fy, +fz);			//right-Bottom
	m_pVertex[4] = XMFLOAT3(-fx, +fy, -fz);
	m_pVertex[5] = XMFLOAT3(+fx, +fy, -fz);			//right-top
	m_pVertex[6] = XMFLOAT3(-fx, -fy, -fz);			//left-Bottom
	m_pVertex[7] = XMFLOAT3(+fx, -fy, -fz);			//right-Bottom
}

CCube::~CCube()
{
	if (m_pVertex)
		delete m_pVertex;
}

void CCube::UpdatePosition(const XMFLOAT3 position, float fTimeLag)
{
	float fx = m_fWidth * 0.5f;
	float fy = m_fHeight * 0.5f;
	float fz = m_fDepth * 0.5f;

	m_CenterPos = position;

	m_pVertex[0] =		Vector3::Add(m_CenterPos, XMFLOAT3(-fx, +fy, +fz));
	m_pVertex[1] =		Vector3::Add(m_CenterPos, XMFLOAT3(+fx, +fy, +fz));
	m_pVertex[2] =		Vector3::Add(m_CenterPos, XMFLOAT3(-fx, -fx, +fz));
	m_pVertex[3] =		Vector3::Add(m_CenterPos, XMFLOAT3(+fx, -fy, +fz));
	m_pVertex[4] =	Vector3::Add(m_CenterPos, XMFLOAT3(-fx, +fy, -fz));
	m_pVertex[5] =		Vector3::Add(m_CenterPos, XMFLOAT3(+fx, +fy, -fz));
	m_pVertex[6] =		Vector3::Add(m_CenterPos, XMFLOAT3(-fx, -fy, -fz));
	m_pVertex[7] =		Vector3::Add(m_CenterPos, XMFLOAT3(+fx, -fy, -fz));
}
