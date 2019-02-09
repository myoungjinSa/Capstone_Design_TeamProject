#include "../Stdafx/Stdafx.h"
#include "ShaderManager.h"
#include "../TextureManager/TextureManager.h"

#include "../Shader/Shader.h"
#include "../Shader/SkyBoxShader/SkyBoxShader.h"
#include "../Shader/TerrainShader/TerrainShader.h"
#include "../Shader/StandardShader/StandardObjectsShader/StandardObjectsShader.h"
#include "../Shader/BillboardShader/SnowShader/SnowShader.h"
#include "../Shader/StandardShader/SkinnedAnimationObjectsShader/SkinnedAnimationObjectsShader.h"
#include "../Shader/BillboardShader/UIShader/NumberUIShader/NumberUIShader.h"
#include "../Shader/BillboardShader/UIShader/TenNumberUIShader/TenNumberUIShader.h"
#include "../Shader/BillboardShader/UIShader/HundredNumberUIShader/HundredNumberUIShader.h"

#include "../GameObject/Player/Player.h"

CShaderManager::CShaderManager()
{
}

CShaderManager::~CShaderManager()
{
}

void CShaderManager::Initialize(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	m_pTextureManager = new CTextureManager;
	m_pTextureManager->Initialize(pd3dDevice, pd3dCommandList);

	m_nShaders = 8;
	m_ppShaders = new CShader*[m_nShaders];

	CSkyBoxShader* pSkyBoxShader = new CSkyBoxShader;
	pSkyBoxShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	pSkyBoxShader->BuildObjects(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, nullptr);
	m_ppShaders[0] = pSkyBoxShader;
	m_ShaderMap.emplace("SkyBox", pSkyBoxShader);

	CTerrainShader* pTerrainShader = new CTerrainShader;
	pTerrainShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	pTerrainShader->BuildObjects(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, nullptr);
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

	CNumberUIShader* pNumberUIShader = new CNumberUIShader;
	pNumberUIShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	pNumberUIShader->BuildObjects(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, m_pTextureManager->getTextureMap());
	m_ppShaders[5] = pNumberUIShader;
	m_ShaderMap.emplace("NumberUI", pNumberUIShader);

	CTenNumberUIShader* pTenNumberUIShader = new CTenNumberUIShader;
	pTenNumberUIShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	pTenNumberUIShader->BuildObjects(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, m_pTextureManager->getTextureMap());
	m_ppShaders[6] = pTenNumberUIShader;
	m_ShaderMap.emplace("TenNumberUI", pTenNumberUIShader);

	CHundredNumberUIShader* pHundredNumberUIShader = new CHundredNumberUIShader;
	pHundredNumberUIShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	pHundredNumberUIShader->BuildObjects(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, m_pTextureManager->getTextureMap());
	m_ppShaders[7] = pHundredNumberUIShader;
	m_ShaderMap.emplace("HundredNumberUI", pHundredNumberUIShader);
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

	if (m_pTextureManager)
	{
		m_pTextureManager->Release();
		delete m_pTextureManager;
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
	static int sec = 0, min = 0;
	if (((CUIShader*)m_ppShaders[5])->getTime() >= 10)
		((CUIShader*)m_ppShaders[6])->setTime(++sec);
	if (((CUIShader*)m_ppShaders[6])->getTime() >= 6)
	{
		sec = 0;
		((CUIShader*)m_ppShaders[7])->setTime(++min);
	}
	if (((CUIShader*)m_ppShaders[7])->getTime() >= 10)
		min = 0;

	for (int i = 0; i < m_nShaders; i++)
	{
		if (m_ppShaders[i])
			m_ppShaders[i]->Render(pd3dCommandList, pCamera);
	}
}