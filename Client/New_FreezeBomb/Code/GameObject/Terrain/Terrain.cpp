#include "../../Stdafx/Stdafx.h"
#include "Terrain.h"
#include "../../Texture/Texture.h"
#include "../../Material/Material.h"
#include "../../Shader/TerrainShader/TerrainShader.h"
#include "../../Scene/Scene.h"

CTerrain::CTerrain(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, LPCTSTR pFileName, int nWidth, int nLength, XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color) : CGameObject(1)
{
	m_nWidth = nWidth;
	m_nLength = nLength;
	m_xmf3Scale = xmf3Scale;

	m_pHeightMapImage = new CHeightMapImage(pFileName, nWidth, nLength, xmf3Scale);

	//CHeightMapGridMesh* pMesh = new CHeightMapGridMesh(pd3dDevice, pd3dCommandList, 0, 0, nWidth, nLength, xmf3Scale, xmf4Color, m_pHeightMapImage);
	//SetMesh(pMesh);

	//CTexture* pTerrainBaseTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	//pTerrainBaseTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Resource/Terrain/Plane/Sand.dds", 0);
	//CTexture* pTerrainDetailTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	//pTerrainDetailTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Resource/Terrain/Plane/Detail_Texture_7.dds", 0);

	//CTerrainShader* pTerrainShader = new CTerrainShader;
	//pTerrainShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	//pTerrainShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	//CScene::CreateShaderResourceViews(pd3dDevice, pTerrainBaseTexture, 13, false);
	//CScene::CreateShaderResourceViews(pd3dDevice, pTerrainDetailTexture, 14, false);

	//CMaterial *pTerrainMaterial = new CMaterial(3);
	//pTerrainMaterial->SetTexture(pTerrainBaseTexture, 0);
	//pTerrainMaterial->SetTexture(pTerrainDetailTexture, 1);
	//pTerrainMaterial->SetShader(pTerrainShader);

	//SetMaterial(0, pTerrainMaterial);*/
}

CTerrain::~CTerrain(void)
{
	//if (m_pHeightMapImage) 
	//	delete m_pHeightMapImage;
}

void CTerrain::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera* pCamera, int nPipelineState)
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
				m_pMesh->Render(pd3dCommandList, i);
			}
		}
	}
}