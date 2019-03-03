#include "../../../Stdafx/Stdafx.h"
#include "UI.h"
#include "../../../Material/Material.h"
#include "../../../Shader/Shader.h"

CUI::CUI(int nMaterial) : CBillboard(nMaterial)
{
}

CUI::~CUI()
{
}

void CUI::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera, int nPipelineState)
{
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
				m_pMesh->Render(pd3dCommandList, 0);
			}
		}
	}
}