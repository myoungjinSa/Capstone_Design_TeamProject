#include "../../Stdafx/Stdafx.h"
#include "LODMesh.h.h"

CLODMesh::CLODMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
	:CStandardMesh(pd3dDevice, pd3dCommandList)
{

}

CLODMesh::~CLODMesh()
{

}

void CLODMesh::Render(ID3D12GraphicsCommandList *pd3dCommandList, int nSubSet, UINT lodLevel)
{
	if (m_LodLevel == lodLevel) 
	{
		UpdateShaderVariables(pd3dCommandList);

		OnPreRender(pd3dCommandList, NULL);

		pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);

		if ((m_nSubMeshes > 0) && (nSubSet < m_nSubMeshes))
		{
			pd3dCommandList->IASetIndexBuffer(&(m_pd3dSubSetIndexBufferViews[nSubSet]));
			pd3dCommandList->DrawIndexedInstanced(m_pnSubSetIndices[nSubSet], 1, 0, 0, 0);
		}
		else
		{
			pd3dCommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0);
		}
	}
}

