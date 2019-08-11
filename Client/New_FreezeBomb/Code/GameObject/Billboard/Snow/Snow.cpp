#include "../../../Stdafx/Stdafx.h"
#include "Snow.h"
#include "../../../Material/Material.h"
#include "../../../Shader/Shader.h"

CSnow::CSnow(int nMaterial) : CBillboard(nMaterial)
{
}

CSnow::~CSnow()
{
}

void CSnow::Animate(float fTimeElapsed, CCamera *pCamera)
{	
	this->SetPosition(this->GetPosition().x - (fTimeElapsed * 100.0f), this->GetPosition().y - (fTimeElapsed * 30.0f) * m_speed, this->GetPosition().z);

	XMFLOAT3 xmf3CameraPosition = pCamera->GetPosition();
	SetLookAt(xmf3CameraPosition);
}

void CSnow::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera, int nPipelineState, int nInstance)
{
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
}

void CSnow::SetLookAt(XMFLOAT3& xmfTarget)
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

void CSnow::SetPositionXZ(float x, float z)
{
	m_xmf4x4World._41 = x;
	m_xmf4x4World._43 = z;
}