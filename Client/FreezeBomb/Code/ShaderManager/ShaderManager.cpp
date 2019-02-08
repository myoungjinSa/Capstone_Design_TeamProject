#include "../Stdafx/Stdafx.h"
#include "ShaderManager.h"

#include "../Shader/Shader.h"
#include "../Shader/SkyBoxShader/SkyBoxShader.h"
#include "../Shader/TerrainShader/TerrainShader.h"
#include "../Shader/StandardShader/StandardObjectsShader/StandardObjectsShader.h"
#include "../Shader/BillboardShader/SnowShader/SnowShader.h"
#include "../Shader/StandardShader/SkinnedAnimationObjectsShader/SkinnedAnimationObjectsShader.h"

#include "../GameObject/Player/Player.h"

CShaderManager::CShaderManager()
{
}

CShaderManager::~CShaderManager()
{
}

void CShaderManager::Initialize(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	m_nShaders = 5;
	m_ppShaders = new CShader*[m_nShaders];

	CSkyBoxShader* pSkyBoxShader = new CSkyBoxShader;
	pSkyBoxShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	pSkyBoxShader->BuildObjects(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, NULL);
	m_ppShaders[0] = pSkyBoxShader;
	m_ShaderMap.emplace("SkyBox", pSkyBoxShader);

	CTerrainShader* pTerrainShader = new CTerrainShader;
	pTerrainShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	pTerrainShader->BuildObjects(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, NULL);
	m_ppShaders[1] = pTerrainShader;
	m_ShaderMap.emplace("Terrain", pTerrainShader);

	CStandardObjectsShader* pSurroundingShader = new CStandardObjectsShader;
	pSurroundingShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	pSurroundingShader->BuildObjects(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pTerrainShader->getTerrain());
	m_ppShaders[2] = pSurroundingShader;
	m_ShaderMap.emplace("Surrounding", pSurroundingShader);

	CSnowShader* pSnowShader = new CSnowShader;
	pSnowShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	pSnowShader->BuildObjects(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pTerrainShader->getTerrain());
	m_ppShaders[3] = pSnowShader;
	m_ShaderMap.emplace("Snow", pSnowShader);

	CSkinnedAnimationObjectsShader* pAnimationObjectShader = new CSkinnedAnimationObjectsShader;
	pAnimationObjectShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	pAnimationObjectShader->BuildObjects(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pTerrainShader->getTerrain());
	m_ppShaders[4] = pAnimationObjectShader;
	m_ShaderMap.emplace("Animation", pAnimationObjectShader);
}

void CShaderManager::ReleaseObjects()
{
	if (m_ppShaders)
	{
		for (int i = 0; i < m_nShaders; i++)
		{
			m_ppShaders[i]->ReleaseShaderVariables();
			m_ppShaders[i]->ReleaseObjects();
			m_ppShaders[i]->Release();
		}
		delete[] m_ppShaders;
	}
}

void CShaderManager::ReleaseUploadBuffers()
{
	for (int i = 0; i < m_nShaders; i++)
		m_ppShaders[i]->ReleaseUploadBuffers();
}

void CShaderManager::AnimateObjects(float elapsedTime, CCamera* pCamera, CPlayer* pPlayer)
{
	for (int i = 0; i < m_nShaders; i++)
		m_ppShaders[i]->AnimateObjects(elapsedTime, pCamera, pPlayer);
}

void CShaderManager::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	for (int i = 0; i < m_nShaders; i++)
		if (m_ppShaders[i])
			m_ppShaders[i]->Render(pd3dCommandList, pCamera);
}