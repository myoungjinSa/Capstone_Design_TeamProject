#include "../../Stdafx/Stdafx.h"
#include "Carry.h"
#include "../Billboard/LampParticle/LampParticle.h"
#include "../../Material/Material.h"
#include "../../Shader/Shader.h"
#include "../../Mesh/BillboardMesh/BillboardMesh.h"

CCarry::CCarry(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	m_pBillboardMesh = new CBillboardMesh(pd3dDevice, pd3dCommandList, 5.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	SetMesh(m_pBillboardMesh);

	m_pLampParticle = new CLampParticle(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
}

CCarry::~CCarry()
{
	if (m_pLampParticle)
	{
		delete m_pLampParticle;
		cout << "파티클 지워짐" << endl;
	}
}

void CCarry::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(bool) + 255) &~255);
	m_pd3dcbCarry = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dcbCarry->Map(0, NULL, (void**)&m_pcbMappedCarry);
}

void CCarry::UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World)
{
	cout << (*pxmf4x4World)._41 << ", " << (*pxmf4x4World)._42 << ", " << (*pxmf4x4World)._43 << endl;
	CGameObject::UpdateShaderVariable(pd3dCommandList, pxmf4x4World);

	m_pcbMappedCarry = &m_Carry;
	D3D12_GPU_VIRTUAL_ADDRESS d3dParticleGPUVirtualAddress = m_pd3dcbCarry->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(19, d3dParticleGPUVirtualAddress);
}

void CCarry::ReleaseShaderVariables()
{
	if (m_pLampParticle)
		m_pLampParticle->ReleaseShaderVariables();

	if (m_pd3dcbCarry)
	{
		m_pd3dcbCarry->Unmap(0, NULL);
		m_pd3dcbCarry->Release();
	}
}

void CCarry::Animate(float elapsedTime)
{
	if (m_pLampParticle)
		m_pLampParticle->Animate(elapsedTime);
}

void CCarry::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	if (m_pLampParticle)
		m_pLampParticle->Render(pd3dCommandList, pCamera);
	else
		CGameObject::Render(pd3dCommandList, pCamera);
}