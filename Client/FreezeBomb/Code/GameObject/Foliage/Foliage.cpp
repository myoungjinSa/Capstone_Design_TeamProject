#include "../../Stdafx/Stdafx.h"
#include "../../Material/Material.h"
#include "../../Shader/Shader.h"
#include "../../Mesh/LODMesh/LODMesh.h.h"
#include "../../FrameTransform/FrameTransform.h"
#include "Foliage.h"

CFoliageObject::CFoliageObject(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature)
{
}

CFoliageObject::~CFoliageObject()
{
}

void CFoliageObject::Render(ID3D12GraphicsCommandList *pd3dCommandList, UINT lodlevel, CCamera *pCamera, int nPipelineState)
{
	OnPrepareRender();

	if (m_pSkinningBoneTransforms)
		m_pSkinningBoneTransforms->SetSkinnedMeshBoneTransformConstantBuffer();
	if (m_pFrameTransform)
		m_pFrameTransform->SetFrameMeshWorldConstantBuffer();
	if (m_pMesh)
	{
		if (!m_pSkinningBoneTransforms) UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World);

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
				if (m_pMesh->GetLodLevel() == lodlevel) 
					m_pMesh->Render(pd3dCommandList, i, lodlevel);
			}
		}
	}

	if (m_pSibling)
		m_pSibling->Render(pd3dCommandList, lodlevel, pCamera, nPipelineState);
	if (m_pChild)
		m_pChild->Render(pd3dCommandList, lodlevel, pCamera, nPipelineState);
}