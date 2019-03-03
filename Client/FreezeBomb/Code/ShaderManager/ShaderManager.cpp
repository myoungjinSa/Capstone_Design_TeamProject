#include "../Stdafx/Stdafx.h"
#include "ShaderManager.h"
#include "../ResourceManager/ResourceManager.h"

#include "../Shader/Shader.h"
#include "../Shader/SkyBoxShader/SkyBoxShader.h"
#include "../Shader/TerrainShader/TerrainShader.h"
#include "../Shader/StandardShader/StandardObjectsShader/StandardObjectsShader.h"
#include "../Shader/BillboardShader/SnowShader/SnowShader.h"
#include "../Shader/StandardShader/SkinnedAnimationObjectsShader/SkinnedAnimationObjectsShader.h"

#include "../Shader/BillboardShader/UIShader/TimerUIShader/TimerUIShader.h"
#include "../Shader/BillboardShader/UIShader/ItemUIShader/ItemUIShader.h"

#include "../Shader/StandardShader/MapObjectShader/MapObjectShader.h"

#include "../Shader/StandardShader/ItemShader/ItemShader.h"
#include "../Shader/ShadowShader/ShadowShader.h"

#include "../Shader/FoliageShader/FoliageShader.h"
#include "../Shader/CubeParticleShader/IceParticleShader/IceParticleShader.h"
#include "../Shader/PostProcessShader/CartoonShader/SobelCartoonShader.h"

#include "../GameObject/Player/Player.h"
#include "../GameObject/Item/Item.h"

CShaderManager::CShaderManager()
{
}

CShaderManager::~CShaderManager()
{
}

void CShaderManager::Initialize(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	m_pResourceManager = new CResourceManager;
	m_pResourceManager->Initialize(pd3dDevice, pd3dCommandList);

	m_nShaders = 9;

	//맵툴 모드일때는 맵의 오브젝트들을 그리지 않게 하기 위해 
	// 그래야 맵툴모드에서 적용해서 배치한 오브젝트들만 볼 수 있다.
#ifdef _MAPTOOL_MODE_
	m_nShaders = m_nShaders + 1;
#endif
	m_ppShaders = new CShader*[m_nShaders];

	int index = 0;
	CSkyBoxShader* pSkyBoxShader = new CSkyBoxShader;
	pSkyBoxShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	pSkyBoxShader->BuildObjects(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, m_pResourceManager->getTextureMap(), nullptr);
	m_ppShaders[index++] = pSkyBoxShader;
	m_ShaderMap.emplace("SkyBox", pSkyBoxShader);

	CTerrainShader* pTerrainShader = new CTerrainShader;
	pTerrainShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	pTerrainShader->BuildObjects(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, m_pResourceManager->getTextureMap(), nullptr);
	m_ppShaders[index++] = pTerrainShader;
	m_ShaderMap.emplace("Terrain", pTerrainShader);

#ifndef _MAPTOOL_MODE_
	CMapObjectsShader *pMapShader = new CMapObjectsShader;
	pMapShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	pMapShader->BuildObjects(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, m_pResourceManager->getBoundMap(), pTerrainShader->getTerrain());
	m_ppShaders[index++] = pMapShader;
	m_ShaderMap.emplace("MapShader", pMapShader);
#endif

	//Foliage는 충돌처리가 필요 없음.. 따라서 Bound박스 필요  없다.
	CFoliageShader* pFoliageShader = new CFoliageShader;
	pFoliageShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	pFoliageShader->BuildObjects(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pTerrainShader->getTerrain());
	m_ppShaders[index++] = pFoliageShader;
	m_ShaderMap.emplace("Foliage", pFoliageShader);

	CItemShader* pItemShader = new CItemShader;
	//pItemShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	pItemShader->BuildObjects(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, m_pResourceManager->getBoundMap(), pTerrainShader->getTerrain());
	m_ppShaders[index++] = pItemShader;
	m_ShaderMap.emplace("Item", pItemShader);

	CShadowShader* pShadowShader = new CShadowShader;
	pShadowShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	pShadowShader->BuildObjects(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, m_pResourceManager->getTextureMap(), pTerrainShader->getTerrain());
	m_ppShaders[index++] = pShadowShader;
	m_ShaderMap.emplace("Shadow", pShadowShader);

	/*CSobelCartoonShader *pCartoonShader = new CSobelCartoonShader;
	pCartoonShader->CreateGraphicsRootSignature(pd3dDevice);
	pCartoonShader->CreateShader(pd3dDevice, pCartoonShader->GetGraphicsRootSignature(), 1);
	pCartoonShader->BuildObjects(pd3dDevice, pd3dCommandList, m_pResourceManager->getTextureMap());
*/

	//CStandardObjectsShader* pSurroundingShader = new CStandardObjectsShader;
	//pSurroundingShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	//pSurroundingShader->BuildObjects(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, m_pResourceManager->getBoundMap(), pTerrainShader->getTerrain());
	//m_ppShaders[index++] = pSurroundingShader;
	//m_ShaderMap.emplace("Surrounding", pSurroundingShader);

	//CSkinnedAnimationObjectsShader* pAnimationObjectShader = new CSkinnedAnimationObjectsShader;
	////pAnimationObjectShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	//pAnimationObjectShader->BuildObjects(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, m_pResourceManager->getBoundMap(), pTerrainShader->getTerrain());
	//m_ppShaders[index++] = pAnimationObjectShader;
	//m_ShaderMap.emplace("곰돌이", pAnimationObjectShader);

	CSnowShader * pSnowShader = new CSnowShader;
	pSnowShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	pSnowShader->BuildObjects(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, m_pResourceManager->getTextureMap(), pTerrainShader->getTerrain());
	m_ppShaders[index++] = pSnowShader;
	m_ShaderMap.emplace("Snow", pSnowShader);

	//CCubeIceShader* pIceParticleShader = new CCubeIceShader;
	//pIceParticleShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	//pIceParticleShader->BuildObjects(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, m_pResourceManager->getTextureMap(), nullptr);
	//m_ppShaders[index++] = pIceParticleShader;
	//m_ShaderMap.emplace("IceParticle", pIceParticleShader);

	CTimerUIShader* pTimerUIShader = new CTimerUIShader;
	pTimerUIShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	pTimerUIShader->BuildObjects(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, m_pResourceManager->getTextureMap(), nullptr);
	m_ppShaders[index++] = pTimerUIShader;
	m_ShaderMap.emplace("TimerUI", pTimerUIShader);

	CItemUIShader* pItemUIShader = new CItemUIShader;
	pItemUIShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	pItemUIShader->BuildObjects(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, m_pResourceManager->getTextureMap(), nullptr);
	m_ppShaders[index++] = pItemUIShader;
	m_ShaderMap.emplace("ItemUI", pItemUIShader);
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

	if (m_pResourceManager)
	{
		m_pResourceManager->Release();
		delete m_pResourceManager;
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

	if (pPlayer->get_Normal_InventorySize() > 0)
	{
		auto iter = m_ShaderMap.find("ItemUI");
		if (iter != m_ShaderMap.end())
			dynamic_cast<CItemUIShader*>((*iter).second)->setRender(true);
	}
	else
	{
		auto iter = m_ShaderMap.find("ItemUI");
		if (iter != m_ShaderMap.end())
			dynamic_cast<CItemUIShader*>((*iter).second)->setRender(false);
	}

	//if (pPlayer->getItem()->getSpecialItem() == false)
	//{
	//	auto iter = m_ShaderMap.find("Special_ItemUI");
	//	if (iter != m_ShaderMap.end())
	//		dynamic_cast<CSpecialItemUIShader*>((*iter).second)->setRender(false);
	//}
	//else
	//{
	//	auto iter = m_ShaderMap.find("Special_ItemUI");
	//	if (iter != m_ShaderMap.end())
	//		dynamic_cast<CSpecialItemUIShader*>((*iter).second)->setRender(true);
	//}

}

void CShaderManager::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	for (int i = 0; i < m_nShaders; i++)
	{
		if (m_ppShaders[i])
			m_ppShaders[i]->Render(pd3dCommandList, pCamera, GameObject);
	}
}