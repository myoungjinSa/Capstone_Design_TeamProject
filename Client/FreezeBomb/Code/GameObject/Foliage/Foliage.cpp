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

	if (m_pFrameTransform)
		m_pFrameTransform->SetFrameMeshWorldConstantBuffer();

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

				//((CLODMesh*)m_pMesh)->CLODMesh::Render(pd3dCommandList, i, lodlevel);
				m_pMesh->Render(pd3dCommandList, i, lodlevel);
			}
		}
	}

	if (m_pSibling)
		((CFoliageObject*)m_pSibling)->CFoliageObject::Render(pd3dCommandList, lodlevel, pCamera, nPipelineState);
	if (m_pChild)
		((CFoliageObject*)m_pChild)->CFoliageObject::Render(pd3dCommandList, lodlevel, pCamera, nPipelineState);
}