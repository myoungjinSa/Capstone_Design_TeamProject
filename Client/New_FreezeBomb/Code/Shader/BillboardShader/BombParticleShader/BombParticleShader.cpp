#include "../../../Stdafx/Stdafx.h"
#include "BombParticleShader.h"
#include "../../../GameObject/GameObject.h"
#include "../../../GameObject/Billboard/Bomb/Bomb.h"
#include "../../../Mesh/BillboardMesh/BillboardMesh.h"
#include "../../../Material/Material.h"
#include "../../../Texture/Texture.h"
#include "../../../Scene/Scene.h"
#include "../../../GameObject/Player/Player.h"

CBombParticleShader::CBombParticleShader()
{
}

CBombParticleShader::~CBombParticleShader()
{
}

D3D12_SHADER_BYTECODE CBombParticleShader::CreateVertexShader()
{
	return(CShader::CompileShaderFromFile(L"../Code/Shader/HLSL/Shaders.hlsl", "VSBombParticle", "vs_5_1", &m_pd3dVertexShaderBlob));
}

D3D12_SHADER_BYTECODE CBombParticleShader::CreatePixelShader()
{
	return(CShader::CompileShaderFromFile(L"../Code/Shader/HLSL/Shaders.hlsl", "PSBombParticle", "ps_5_1", &m_pd3dVertexShaderBlob));
}

void CBombParticleShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature,
	const map<string, CTexture*>& Context, void *pContext)
{
	auto iter = Context.find("Particle");
	if (iter != Context.end())
	{
		CBillboardMesh* pBillboardMesh = new CBillboardMesh(pd3dDevice, pd3dCommandList, 13.0f, 13.0f, 0.0f, 0.0f, 0.0f, 0.0f);

		m_BombParticle = new CBomb(1);
		m_BombParticle->SetMesh(pBillboardMesh);
		m_BombParticle->SetPosition(0, 0, 0);

		CMaterial *pParticleMaterial = new CMaterial(1);
		pParticleMaterial->SetTexture((*iter).second, 0);
		m_BombParticle->SetMaterial(0, pParticleMaterial);

		CreateShaderVariables(pd3dDevice, pd3dCommandList);
	}
}

void CBombParticleShader::AnimateObjects(float elapsedTime, CCamera *pCamera, CPlayer *pPlayer)
{
	m_elapsedTime = elapsedTime;
	if (m_BombParticle)
	{
		// 곰돌이의 프레임을 찾는다.
		//CGameObject* p = pPlayer->FindFrame("black-handbomb");
		//if (p != nullptr)
		//{
			// 곰돌이의 프레임에 월드를 얻어온다.
			//m_BombParticle->SetPosition(p->m_xmf4x4World._41, p->m_xmf4x4World._42, p->m_xmf4x4World._43);
		//}
		XMFLOAT3 xmf3CameraPosition = pCamera->GetPosition();
		m_BombParticle->SetLookAt(xmf3CameraPosition);
		m_BombParticle->Animate(elapsedTime);
	}
}

void CBombParticleShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera, int nPipelineState)
{
	if (m_BombParticle)
	{
		OnPrepareRender(pd3dCommandList);
		UpdateShaderVariable(pd3dCommandList, nullptr);
		m_BombParticle->Render(pd3dCommandList, pCamera, nPipelineState);
	}
}

void CBombParticleShader::ReleaseObjects()
{
	if (m_BombParticle)
		m_BombParticle->Release();
}

void CBombParticleShader::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_World) + 255) & ~255); //256의 배수
	m_pd3dcbWorld = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dcbWorld->Map(0, nullptr, (void**)&m_pcbMappedWorld);

	ncbElementBytes = ((sizeof(CB_ANIMATIONCLIP) + 255) &~255);
	m_pd3dcbAnimationClip = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dcbAnimationClip->Map(0, NULL, (void**)&m_pcbMappedAnimationClip);
}

void CBombParticleShader::ReleaseShaderVariables()
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

void CBombParticleShader::UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World)
{
	if (m_pd3dcbWorld)
	{
		XMStoreFloat4x4(&m_pcbMappedWorld->m_World, XMMatrixTranspose(XMLoadFloat4x4(&m_BombParticle->m_xmf4x4World)));
		D3D12_GPU_VIRTUAL_ADDRESS GpuVirtualAddress = m_pd3dcbWorld->GetGPUVirtualAddress();
		pd3dCommandList->SetGraphicsRootConstantBufferView(1, GpuVirtualAddress);
	}

	if (m_BombParticle)
	{
		m_pcbMappedAnimationClip->m_AnimationClip = m_BombParticle->m_AnimationClip;
		D3D12_GPU_VIRTUAL_ADDRESS GpuVirtualAddress = m_pd3dcbAnimationClip->GetGPUVirtualAddress();
		pd3dCommandList->SetGraphicsRootConstantBufferView(6 , GpuVirtualAddress);
	}
}

