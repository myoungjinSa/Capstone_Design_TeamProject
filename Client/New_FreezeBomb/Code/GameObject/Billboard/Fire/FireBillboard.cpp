#include "../../../Stdafx/Stdafx.h"
#include "FireBillboard.h"
#include "../../../Mesh/BillboardMesh/BillboardMesh.h"
#include "../../../Material/Material.h"
#include "../../../Texture/Texture.h"
#include "../../../Scene/Scene.h"
#include "../../../Shader/BillboardShader/FireBillboardShader/FireBillboardShader.h"

CFireBillboard::CFireBillboard(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
	:CBillboard(1)
{
	CBillboardMesh *pBillboardMesh = new CBillboardMesh(pd3dDevice, pd3dCommandList, 40.0f, 40.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	SetMesh(pBillboardMesh);

	CTexture *pFireTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	pFireTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Resource/Textures/Effect/Fire.dds", 0);


	CScene::CreateShaderResourceViews(pd3dDevice, pFireTexture, 26, false);

	CFireBillboardShader* pFireShader = new CFireBillboardShader;
	pFireShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);

	CMaterial *pFireMaterial = new CMaterial(1);
	pFireMaterial->SetTexture(pFireTexture);
	pFireMaterial->SetShader(pFireShader);

	SetMaterial(0, pFireMaterial);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

CFireBillboard::~CFireBillboard()
{
	delete this;
}

void CFireBillboard::SetLookAt(XMFLOAT3& xmfTarget)
{
	XMFLOAT3 xmf3Up(0.0f, 1.0f, 0.0f);
	XMFLOAT3 xmf3Position(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43);
	XMFLOAT3 xmf3Look = Vector3::SubtractNormalize(xmfTarget, xmf3Position, true);
	XMFLOAT3 xmf3Right = Vector3::CrossProduct(xmf3Up, xmf3Look, true);

	XMFLOAT3 moveVector = Vector3::ScalarProduct(xmf3Look,5,true);
	
	m_xmf4x4World._11 = xmf3Right.x;
	m_xmf4x4World._12 = xmf3Right.y;
	m_xmf4x4World._13 = xmf3Right.z;

	m_xmf4x4World._21 = xmf3Up.x;
	m_xmf4x4World._22 = xmf3Up.y;
	m_xmf4x4World._23 = xmf3Up.z;

	m_xmf4x4World._31 = xmf3Look.x;
	m_xmf4x4World._32 = xmf3Look.y;
	m_xmf4x4World._33 = xmf3Look.z;

	m_xmf4x4World._41 += moveVector.x;
	m_xmf4x4World._42 += moveVector.y;
	m_xmf4x4World._43 += moveVector.z;


}

void CFireBillboard::Animate(float elapsedTime)
{

	static float totalTime = 0.0f;
	totalTime +=elapsedTime;

	//cout << "totalTime" << totalTime << endl;

	if (totalTime > 40.0f) 
	{
		//printf("Animation Clip : %d\n", m_AnimationClip);
		m_AnimationClip += 1;
		totalTime = 0.0f;
	}
	if (m_AnimationClip > 8)
	{
		m_AnimationClip = 0;
		totalTime = 0.0f;
	}
	
}

void CFireBillboard::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState)
{

	
	XMFLOAT3 xmf3CameraPosition = pCamera->GetPosition();
	SetLookAt(xmf3CameraPosition);

	CGameObject::Render(pd3dCommandList, pCamera, nPipelineState);
	
}

void CFireBillboard::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_World) + 255) &~255);
	m_pd3dcbWorld = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dcbWorld->Map(0, NULL, (void**)&m_pcbMappedWorld);

	ncbElementBytes = ((sizeof(CB_ANIMATIONCLIP) + 255) &~255);
	m_pd3dcbAnimationClip = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dcbAnimationClip->Map(0, NULL, (void**)&m_pcbMappedAnimationClip);
}

void CFireBillboard::ReleaseShaderVariables()
{
	if (m_pd3dcbWorld)
	{
		m_pd3dcbWorld->Unmap(0, nullptr);
		m_pd3dcbWorld->Release();
	}

	if (m_pd3dcbAnimationClip)
	{
		m_pd3dcbAnimationClip->Unmap(0, nullptr);
		m_pd3dcbAnimationClip->Release();
	}
}
void CFireBillboard::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (m_pd3dcbWorld)
	{
		XMFLOAT4X4 world = m_xmf4x4World;
		XMStoreFloat4x4(&m_pcbMappedWorld->m_World, XMMatrixTranspose(XMLoadFloat4x4(&world)));
		D3D12_GPU_VIRTUAL_ADDRESS GpuVirtualAddress = m_pd3dcbWorld->GetGPUVirtualAddress();
		pd3dCommandList->SetGraphicsRootConstantBufferView(1, GpuVirtualAddress);
	}

	if (m_pd3dcbAnimationClip)
	{
		m_pcbMappedAnimationClip->m_AnimationClip = m_AnimationClip;
		D3D12_GPU_VIRTUAL_ADDRESS GpuVirtualAddress = m_pd3dcbAnimationClip->GetGPUVirtualAddress();
		pd3dCommandList->SetGraphicsRootConstantBufferView(6, GpuVirtualAddress);
	}
}