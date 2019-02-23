#include "../../Stdafx/Stdafx.h"
#include "EvilBear.h"
#include "../../Material/Material.h"
#include "../../Shader/Shader.h"

CEvilBear::CEvilBear(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature,int matID)
{
	m_matID = matID;
}

CEvilBear::~CEvilBear()
{
}

void CEvilBear::Render(ID3D12GraphicsCommandList* pd3dCommandList,bool bIce,int matID, CCamera* pCamera)
{
	CGameObject::Render(pd3dCommandList,m_bHammer,m_bBomb,bIce,matID, pCamera);
	//OnPrepareRender();

	//if (m_pSkinningBoneTransforms)
	//	m_pSkinningBoneTransforms->SetSkinnedMeshBoneTransformConstantBuffer();

	//if (m_pMesh)
	//{
	//	if (!m_pSkinningBoneTransforms)
	//		UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World);

	//	if (m_nMaterials > 0)
	//	{
	//		for (int i = 0; i < m_nMaterials; i++)
	//		{
	//			if (m_ppMaterials[i])
	//			{
	//				if (m_ppMaterials[i]->m_pShader)
	//					m_ppMaterials[i]->m_pShader->Render(pd3dCommandList, pCamera);

	//				m_ppMaterials[i]->UpdateShaderVariables(pd3dCommandList);
	//			}
	//			m_pMesh->Render(pd3dCommandList, i);
	//		}
	//	}
	//}

	//if (m_pSibling)
	//	m_pSibling->Render(pd3dCommandList, pCamera);
	//if (m_pChild)
	//	m_pChild->Render(pd3dCommandList, pCamera);
}