#include "stdafx.h"
#include "AngryBot.h"

CAngryBot::CAngryBot(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature)
{
}

CAngryBot::~CAngryBot()
{
}

void CAngryBot::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = (((sizeof(XMFLOAT4X4) * SKINNED_ANIMATION_BONES) + 255) & ~255); //256ÀÇ ¹è¼ö

	m_pd3dcbBoneTransforms = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dcbBoneTransforms->Map(0, NULL, (void **)&m_pcbxmf4x4BoneTransforms);
}
void CAngryBot::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	if (m_pd3dcbBoneTransforms)
	{
		//Skinned Bone Transforms
		D3D12_GPU_VIRTUAL_ADDRESS d3dcbBoneTransformsGpuVirtualAddress = m_pd3dcbBoneTransforms->GetGPUVirtualAddress();
		pd3dCommandList->SetGraphicsRootConstantBufferView(12, d3dcbBoneTransformsGpuVirtualAddress);
		for (int i = 0; i < nSkinningBoneSize; i++)
		{
			//XMStoreFloat4x4(&m_pcbxmf4x4BoneTransforms[i], XMMatrixTranspose(XMLoadFloat4x4(&m_ppSkinningBoneFrameCaches[i]->m_xmf4x4World)));
			XMFLOAT4X4 Transforms = v_Transforms[i]->m_xmf4x4World;
			XMStoreFloat4x4(&m_pcbxmf4x4BoneTransforms[i], XMMatrixTranspose(XMLoadFloat4x4(&Transforms)));
		}
	}
}
void CAngryBot::OnPrepareAnimate()
{
}

void CAngryBot::Animate(float fTimeElapsed)
{
	CGameObject::Animate(fTimeElapsed);
}

