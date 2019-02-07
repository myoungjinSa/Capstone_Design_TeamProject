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

	CHeightMapGridMesh* pMesh = new CHeightMapGridMesh(pd3dDevice, pd3dCommandList, 0, 0, nWidth, nLength, xmf3Scale, xmf4Color, m_pHeightMapImage);
	SetMesh(pMesh);

	//CreateShaderVariables(pd3dDevice, pd3dCommandList);

	CTexture* pTerrainBaseTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	pTerrainBaseTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Resource/Terrain/Plane/Sand.dds", 0);
	CTexture* pTerrainDetailTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	pTerrainDetailTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Resource/Terrain/Plane/Detail_Texture_7.dds", 0);

	CTerrainShader* pTerrainShader = new CTerrainShader;
	pTerrainShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	pTerrainShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	//CScene::CreateCbvSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 0, 2);
	CScene::CreateShaderResourceViews(pd3dDevice, pTerrainBaseTexture, 13, false);
	CScene::CreateShaderResourceViews(pd3dDevice, pTerrainDetailTexture, 14, false);

	CMaterial *pTerrainMaterial = new CMaterial(3);
	pTerrainMaterial->SetTexture(pTerrainBaseTexture, 0);
	pTerrainMaterial->SetTexture(pTerrainDetailTexture, 1);
	pTerrainMaterial->SetShader(pTerrainShader);

	SetMaterial(0, pTerrainMaterial);
}

CTerrain::~CTerrain(void)
{
	if (m_pHeightMapImage) 
		delete m_pHeightMapImage;
}