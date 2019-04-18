#include "../../../Stdafx/Stdafx.h"
#include "Bomb.h"
#include "../../../Mesh/BillboardMesh/BillboardMesh.h"
#include "../../../Material/Material.h"
#include "../../../Texture/Texture.h"
#include "../../../Scene/Scene.h"

CBomb::CBomb(int nMaterial) : CBillboard(nMaterial)
{

}

CBomb::~CBomb()
{
}

void CBomb::Animate(float elapsedTime)
{
	if (m_IsBlowing)
	{
		static float totalTime = 0.0f;
		totalTime += 5.0f * elapsedTime;

		if (totalTime > 1.0f)
		{
			m_AnimationClip += (UINT)totalTime;
			totalTime = 0.0f;
		}
		if (m_AnimationClip > 5)
		{
			m_AnimationClip = 0;
			totalTime = 0.0f;
			m_IsBlowing = false;
		}
	}
}

void CBomb::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState)
{
	if (m_IsBlowing)
	{
		CGameObject::Render(pd3dCommandList, pCamera, nPipelineState);
	}
}

void CBomb::SetLookAt(XMFLOAT3& xmfTarget)
{
	XMFLOAT3 xmf3Up(0.0f, 1.0f, 0.0f);
	XMFLOAT3 xmf3Position(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43);
	XMFLOAT3 xmf3Look = Vector3::SubtractNormalize(xmfTarget, xmf3Position, true);
	XMFLOAT3 xmf3Right = Vector3::CrossProduct(xmf3Up, xmf3Look, true);

	m_xmf4x4World._11 = xmf3Right.x;
	m_xmf4x4World._12 = xmf3Right.y;
	m_xmf4x4World._13 = xmf3Right.z;

	m_xmf4x4World._21 = xmf3Up.x;
	m_xmf4x4World._22 = xmf3Up.y;
	m_xmf4x4World._23 = xmf3Up.z;

	m_xmf4x4World._31 = xmf3Look.x;
	m_xmf4x4World._32 = xmf3Look.y;
	m_xmf4x4World._33 = xmf3Look.z;
}