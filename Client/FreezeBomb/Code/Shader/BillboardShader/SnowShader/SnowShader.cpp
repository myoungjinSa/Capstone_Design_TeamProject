#include "../../../Stdafx/Stdafx.h"
#include "SnowShader.h"
#include "../../../Mesh/BillboardMesh/BillboardMesh.h"
#include "../../../Texture/Texture.h"
#include "../../../Material/Material.h"
#include "../../../GameObject/Billboard/Snow/Snow.h"
#include "../../../GameObject/Terrain/Terrain.h"
#include "../../../GameObject/Player/Player.h"
#include "../../../Scene/Scene.h"

CSnowShader::CSnowShader()
{
}

CSnowShader::~CSnowShader()
{
}

D3D12_SHADER_BYTECODE CSnowShader::CreateVertexShader()
{
	return(CShader::CompileShaderFromFile(L"../Code/Shader/HLSL/Shaders.hlsl", "VSSnow", "vs_5_1", &m_pd3dVertexShaderBlob));
}

D3D12_SHADER_BYTECODE CSnowShader::CreatePixelShader()
{
	return(CShader::CompileShaderFromFile(L"../Code/Shader/HLSL/Shaders.hlsl", "PSSnow", "ps_5_1", &m_pd3dVertexShaderBlob));
}

void CSnowShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, 
	const map<string, CTexture*>& Context, void *pContext)
{
	//SnowBillboard는 지형의 높이정보를 알고있어야함. 그래야 지형의 밑으로 내려갔을때 다시 하늘에서 떨어지게 할 수 있다.
	m_pTerrain = (CTerrain*)pContext;

	m_pMaterial = new CMaterial(1);
	auto iter = Context.find("Snow");
	if(iter != Context.end())
		m_pMaterial->SetTexture((*iter).second, 0);

	CBillboardMesh* pSnowBillboardMesh = new CBillboardMesh(pd3dDevice, pd3dCommandList, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);

	//가상의 박스 -> 렌더링 하지는 않음
	CCube* pCubeObject = new CCube(50.0f, 50.0f, 50.0f);

	m_refCubeObject = pCubeObject->GetCubeObject();

	m_nObjects = 1000;
	m_ppObjects = new CGameObject*[m_nObjects];
	XMFLOAT3 xmf3RoatationAxis = XMFLOAT3(0.0f, 0.0, 1.0f);

	int minX = -m_refCubeObject.GetWidth();
	int maxX = m_refCubeObject.GetWidth();
	int minZ = -m_refCubeObject.GetDepth();
	int maxZ = m_refCubeObject.GetDepth();

	for (int i = 0; i < m_nObjects; i++)
	{
		CSnow* pSnow = new CSnow(1);
		pSnow->SetMesh(pSnowBillboardMesh);
		pSnow->SetMaterial(0, m_pMaterial);
		pSnow->SetOffsetX(Random(minX, maxX));
		pSnow->SetOffsetZ(Random(minZ, maxZ));
		pSnow->SetPosition(0.0f, Random(300.0f, 400.0f), 0.0f);
		pSnow->setSpeed(Random(0.5f, 1.5f));
		m_ppObjects[i] = pSnow;
	}

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CSnowShader::AnimateObjects(float fTimeElapsed, CCamera *pCamera, CPlayer *pPlayer)
{
	m_refCubeObject.UpdatePosition(pPlayer->GetPosition(), m_fTimeLagScale);

	if (m_ppObjects)
	{
		for (int i = 0; i < m_nObjects; i++)
		{
			if (m_ppObjects[i])
			{
				((CSnow*)m_ppObjects[i])->SetPositionXZ(m_refCubeObject.GetPosition().x + ((CSnow*)m_ppObjects[i])->GetOffsetX(),
					m_refCubeObject.GetPosition().z + ((CSnow*)m_ppObjects[i])->GetOffsetZ());
				((CSnow*)m_ppObjects[i])->Animate(fTimeElapsed, pCamera);

				// 눈이 지형 아래로 내려가면 다시 위에서 생성되서 다시 내려오게 함
				if (m_ppObjects[i]->GetPosition().y < m_pTerrain->GetHeight(m_ppObjects[i]->GetPosition().x, m_ppObjects[i]->GetPosition().z))
				{
					m_ppObjects[i]->SetPosition(m_refCubeObject.GetPosition().x + ((CSnow*)m_ppObjects[i])->GetOffsetX(), Random(300.0f, 400.0f),
						m_refCubeObject.GetPosition().z + ((CSnow*)m_ppObjects[i])->GetOffsetZ());
				}
			}
		}
	}
}

void CSnowShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera, int nPipelineState)
{
	OnPrepareRender(pd3dCommandList);

	UpdateShaderVariables(pd3dCommandList);

	if (m_ppObjects)
	{
		m_ppObjects[0]->Render(pd3dCommandList, pCamera, nPipelineState, m_nObjects);
	}
}

void CSnowShader::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_pd3dInstancingData = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, m_nObjects * sizeof(InstancingData), D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dInstancingData->Map(0, nullptr, (void**)&m_pMappedInstancingData);
}

void CSnowShader::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (m_pd3dInstancingData)
	{
		D3D12_GPU_VIRTUAL_ADDRESS GPUVirtualAddress = m_pd3dInstancingData->GetGPUVirtualAddress();
		pd3dCommandList->SetGraphicsRootShaderResourceView(8, GPUVirtualAddress);

		for (int i = 0; i < m_nObjects; i++)
		{
			XMStoreFloat4x4(&m_pMappedInstancingData[i].m_World, XMMatrixTranspose(XMLoadFloat4x4(&m_ppObjects[i]->m_xmf4x4World)));
		}
	}
}

void CSnowShader::ReleaseShaderVariables()
{
	if (m_pd3dInstancingData)
	{
		m_pd3dInstancingData->Unmap(0, nullptr);
		m_pd3dInstancingData->Release();
	}
}
