#include "../../Stdafx/Stdafx.h"
#include "EvilBear.h"
#include "../Shadow/Shadow.h"
#include "../../Material/Material.h"
#include "../../Shader/Shader.h"

CEvilBear::CEvilBear(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature,int matID)
{
	m_matID = matID;
}

CEvilBear::~CEvilBear()
{
}

void CEvilBear::Initialize_Shadow(CLoadedModelInfo* pLoadedModel, CGameObject* pGameObject)
{
	m_pShadow = new CShadow(pLoadedModel, this);
}

void CEvilBear::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera, int nPipelineState)
{
	if (m_pShadow)
		m_pShadow->Render(pd3dCommandList, pCamera, GameObject_Shadow);

	OnPrepareRender();

	if (m_pSkinningBoneTransforms)
		m_pSkinningBoneTransforms->SetSkinnedMeshBoneTransformConstantBuffer();

	if (m_pMesh)
	{
		if (!m_pSkinningBoneTransforms)
			UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World);

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
				m_pMesh->Render(pd3dCommandList, i);
			}
		}
	}

	if (m_pSibling)
		((CEvilBear*)m_pSibling)->CEvilBear::Render(pd3dCommandList, pCamera, nPipelineState);
	if (m_pChild)
		((CEvilBear*)m_pChild)->CEvilBear::Render(pd3dCommandList, pCamera, nPipelineState);
}

void CEvilBear::Render(ID3D12GraphicsCommandList *pd3dCommandList, bool bHammer, bool bBomb, bool bIce, int matID, CCamera* pCamera, int nPipelineState)
{
	if (m_pShadow)
		m_pShadow->Render(pd3dCommandList, pCamera, GameObject_Shadow);

	OnPrepareRender();
	if (m_pSkinningBoneTransforms)
		m_pSkinningBoneTransforms->SetSkinnedMeshBoneTransformConstantBuffer();

	if (m_pMesh)
	{
		if (!m_pSkinningBoneTransforms)
			UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World);

		if (m_nMaterials == 1)			//폭탄과 같은 재질이 하나 있는 오브젝트 렌더
		{
			if (m_ppMaterials[0])
			{
				if (m_ppMaterials[0]->m_pShader)
				{
					m_ppMaterials[0]->m_pShader->Render(pd3dCommandList, pCamera, nPipelineState);
				}
				m_ppMaterials[0]->UpdateShaderVariables(pd3dCommandList);
			}
		}
		else if (m_nMaterials == 2)		//망치의 재질 개수 2개 , 일반 망치, 황금 망치
		{
			if (m_ppMaterials[1])
			{
				if (m_ppMaterials[1]->m_pShader)
					m_ppMaterials[0]->m_pShader->Render(pd3dCommandList, pCamera, nPipelineState);
				m_ppMaterials[1]->UpdateShaderVariables(pd3dCommandList);
			}
		}
		else if (m_nMaterials == 7)
		{
			if (bIce)							//얼음 상태일때
			{
				if (m_ppMaterials[CAnimationController::ICE])
				{
					if (m_ppMaterials[MATERIALTYPE::ICEMAT]->m_pShader)
					{
						m_ppMaterials[MATERIALTYPE::ICEMAT]->m_pShader->Render(pd3dCommandList, pCamera, nPipelineState);
					}
					m_ppMaterials[MATERIALTYPE::ICEMAT]->UpdateShaderVariables(pd3dCommandList);
				}
			}
			else
			{
				if (m_ppMaterials[matID])		//얼음상태가 아닐때 
				{
					if (m_ppMaterials[matID]->m_pShader)
					{
						m_ppMaterials[matID]->m_pShader->Render(pd3dCommandList, pCamera, nPipelineState);
					}
					m_ppMaterials[matID]->UpdateShaderVariables(pd3dCommandList);
				}
			}
		}
		m_pMesh->Render(pd3dCommandList, 0);
	}

	if (!strcmp(this->m_pstrFrameName, "Lamp"))		//불꽅 파티클은 일반 GameObject Render하는 방식을 해야한다. 
	{
		if (m_pSibling)
			m_pSibling->Render(pd3dCommandList, pCamera, nPipelineState);
		if (m_pChild)
			m_pChild->Render(pd3dCommandList, pCamera, nPipelineState);
	}
	else
	{
		if (m_pSibling)
		{
			if (!strncmp(m_pSibling->m_pstrFrameName, "hammer", strlen(m_pSibling->m_pstrFrameName)))
			{
				if (bHammer)
				{
					m_pSibling->Render(pd3dCommandList, bHammer, bBomb, bIce, matID, pCamera, nPipelineState);
				}
			}
			else
			{
				m_pSibling->Render(pd3dCommandList, bHammer, bBomb, bIce, matID, pCamera, nPipelineState);
			}

		}
		if (m_pChild)
		{
			if (!strncmp(m_pChild->m_pstrFrameName, "black-handbomb", strlen(m_pChild->m_pstrFrameName)))
			{
				if (bBomb)
				{
					m_pChild->Render(pd3dCommandList, bHammer, bBomb, bIce, matID, pCamera, nPipelineState);
				}
			}
			else
			{
				m_pChild->Render(pd3dCommandList, bHammer, bBomb, bIce, matID, pCamera, nPipelineState);
			}
		}
	}
}