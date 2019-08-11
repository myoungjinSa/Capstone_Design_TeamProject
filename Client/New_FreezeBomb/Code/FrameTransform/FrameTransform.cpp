#include "../Stdafx/Stdafx.h"
#include "FrameTransform.h"
#include "../GameObject/GameObject.h"

CFrameTransform::CFrameTransform(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CLoadedModelInfo* pModel)
{
	// 메쉬 개수 저장
	m_nFrameMeshes = pModel->m_nFrameMeshes;

	// SkinnedMesh가 여러개 일 경우
	m_ppFrameMeshes = new CStandardMesh*[m_nFrameMeshes];

	m_ppd3dcbWorld = new ID3D12Resource*[m_nFrameMeshes];
	m_ppcbxmf4x4World = new XMFLOAT4X4*[m_nFrameMeshes];

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	int nFrameMeshIndex = 0;
	pModel->m_pModelRootObject->FindAndSetFrameMesh(&nFrameMeshIndex, this);
}

CFrameTransform::~CFrameTransform()
{
	ReleaseShaderVariables();

	if (m_ppFrameMeshes) 
		delete[] m_ppFrameMeshes;
	if (m_ppd3dcbWorld) 
		delete[] m_ppd3dcbWorld;
	if (m_ppcbxmf4x4World) 
		delete[] m_ppcbxmf4x4World;
}

void CFrameTransform::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = (((sizeof(XMFLOAT4X4) * SKINNED_ANIMATION_BONES) + 255) & ~255); //256의 배수
	for (int i = 0; i < m_nFrameMeshes; i++)
	{
		m_ppd3dcbWorld[i] = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
		m_ppd3dcbWorld[i]->Map(0, NULL, (void **)&m_ppcbxmf4x4World[i]);
	}
}

void CFrameTransform::SetFrameMeshWorldConstantBuffer()
{
	for (int i = 0; i < m_nFrameMeshes; i++)
	{
		m_ppFrameMeshes[i]->m_ppd3dcbWorld = m_ppd3dcbWorld[i];
		m_ppFrameMeshes[i]->m_ppcbxmf4x4World = m_ppcbxmf4x4World[i];
	}
}
void CFrameTransform::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
}

void CFrameTransform::ReleaseShaderVariables()
{
	for (int i = 0; i < m_nFrameMeshes; i++)
	{
		m_ppd3dcbWorld[i]->Unmap(0, nullptr);
		m_ppd3dcbWorld[i]->Release();
	}
}