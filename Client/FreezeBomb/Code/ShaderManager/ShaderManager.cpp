#include "../Stdafx/Stdafx.h"
#include "ShaderManager.h"
#include "../ResourceManager/ResourceManager.h"

#include "../Shader/Shader.h"
#include "../Shader/SkyBoxShader/SkyBoxShader.h"
#include "../Shader/TerrainShader/TerrainShader.h"
#include "../Shader/StandardShader/StandardObjectsShader/StandardObjectsShader.h"
#include "../Shader/BillboardShader/SnowShader/SnowShader.h"
#include "../Shader/StandardShader/SkinnedAnimationObjectsShader/SkinnedAnimationObjectsShader.h"

#include "../Shader/BillboardShader/UIShader/NumberUIShader/NumberUIShader.h"
#include "../Shader/BillboardShader/UIShader/TenNumberUIShader/TenNumberUIShader.h"
#include "../Shader/BillboardShader/UIShader/HundredNumberUIShader/HundredNumberUIShader.h"
#include "../Shader/BillboardShader/UIShader/ColonUIShader/ColonUIShader.h"
#include "../Shader/BillboardShader/UIShader/ItemBoxUIShader/ItemBoxUIShader.h"
#include "../Shader/BillboardShader/UIShader/NormalItemUIShader/NormalItemUIShader.h"
#include "../Shader/BillboardShader/UIShader/SpecialItemUIShader/SpecialItemUIShader.h"

#include "../Shader/StandardShader/ItemShader/ItemShader.h"
#include "../Shader/ShadowShader/ShadowShader.h"

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

	m_nShaders = 3;
	m_ppShaders = new CShader*[m_nShaders];

	CSkyBoxShader* pSkyBoxShader = new CSkyBoxShader;
	pSkyBoxShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	pSkyBoxShader->BuildObjects(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, m_pResourceManager->getTextureMap(), nullptr);
	m_ppShaders[0] = pSkyBoxShader;
	m_ShaderMap.emplace("SkyBox", pSkyBoxShader);

	CTerrainShader* pTerrainShader = new CTerrainShader;
	pTerrainShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	pTerrainShader->BuildObjects(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, m_pResourceManager->getTextureMap(), nullptr);
	m_ppShaders[1] = pTerrainShader;
	m_ShaderMap.emplace("Terrain", pTerrainShader);

	/*CStandardObjectsShader* pSurroundingShader = new CStandardObjectsShader;
	pSurroundingShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	pSurroundingShader->BuildObjects(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, m_pResourceManager->getBoundMap(), pTerrainShader->getTerrain());
	m_ppShaders[2] = pSurroundingShader;
	m_ShaderMap.emplace("Surrounding", pSurroundingShader);

	CSkinnedAnimationObjectsShader* pAnimationObjectShader = new CSkinnedAnimationObjectsShader;
	pAnimationObjectShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	pAnimationObjectShader->BuildObjects(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, m_pResourceManager->getBoundMap(), pTerrainShader->getTerrain());
	m_ppShaders[3] = pAnimationObjectShader;
	m_ShaderMap.emplace("°õµ¹ÀÌ", pAnimationObjectShader);

	CSnowShader* pSnowShader = new CSnowShader;
	pSnowShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	pSnowShader->BuildObjects(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, m_pResourceManager->getTextureMap(), pTerrainShader->getTerrain());
	m_ppShaders[4] = pSnowShader;
	m_ShaderMap.emplace("Snow", pSnowShader);

	CNumberUIShader* pNumberUIShader = new CNumberUIShader;
	pNumberUIShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	pNumberUIShader->BuildObjects(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, m_pResourceManager->getTextureMap(), nullptr);
	m_ppShaders[5] = pNumberUIShader;
	m_ShaderMap.emplace("NumberUI", pNumberUIShader);

	CTenNumberUIShader* pTenNumberUIShader = new CTenNumberUIShader;
	pTenNumberUIShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	pTenNumberUIShader->BuildObjects(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, m_pResourceManager->getTextureMap(), nullptr);
	m_ppShaders[6] = pTenNumberUIShader;
	m_ShaderMap.emplace("TenNumberUI", pTenNumberUIShader);

	CHundredNumberUIShader* pHundredNumberUIShader = new CHundredNumberUIShader;
	pHundredNumberUIShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	pHundredNumberUIShader->BuildObjects(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, m_pResourceManager->getTextureMap(), nullptr);
	m_ppShaders[7] = pHundredNumberUIShader;
	m_ShaderMap.emplace("HundredNumberUI", pHundredNumberUIShader);

	CColonUIShader* pColonUIShader = new CColonUIShader;
	pColonUIShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	pColonUIShader->BuildObjects(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, m_pResourceManager->getTextureMap(), nullptr);
	m_ppShaders[8] = pColonUIShader;
	m_ShaderMap.emplace("ColonUI", pColonUIShader);

	CItemBoxUIShader* pItemBoxUIShader = new CItemBoxUIShader;
	pItemBoxUIShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	pItemBoxUIShader->BuildObjects(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, m_pResourceManager->getTextureMap(), nullptr);
	m_ppShaders[9] = pItemBoxUIShader;
	m_ShaderMap.emplace("ItemBoxUI", pColonUIShader);

	CNoramlItemUIShader* pNormalItemUIShader = new CNoramlItemUIShader;
	pNormalItemUIShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	pNormalItemUIShader->BuildObjects(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, m_pResourceManager->getTextureMap(), nullptr);
	m_ppShaders[10] = pNormalItemUIShader;
	m_ShaderMap.emplace("Normal_ItemUI", pNormalItemUIShader);

	CSpecialItemUIShader* pSpecialItemUIShader = new CSpecialItemUIShader;
	pSpecialItemUIShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	pSpecialItemUIShader->BuildObjects(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, m_pResourceManager->getTextureMap(), nullptr);
	m_ppShaders[11] = pSpecialItemUIShader;
	m_ShaderMap.emplace("Special_ItemUI", pSpecialItemUIShader);

	CItemShader* pItemShader = new CItemShader;
	pItemShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	pItemShader->BuildObjects(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, m_pResourceManager->getBoundMap(), pTerrainShader->getTerrain());
	m_ppShaders[12] = pItemShader;
	m_ShaderMap.emplace("Item", pItemShader);*/

	CShadowShader* pShadowShader = new CShadowShader;
	pShadowShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	pShadowShader->BuildObjects(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pTerrainShader->getTerrain());
	m_ppShaders[2] = pShadowShader;
	m_ShaderMap.emplace("Shadow", pShadowShader);
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
		auto iter = m_ShaderMap.find("Normal_ItemUI");
		if (iter != m_ShaderMap.end())
			dynamic_cast<CNoramlItemUIShader*>((*iter).second)->setRender(true);
	}
	else if (pPlayer->get_Normal_InventorySize() == 0)
	{
		auto iter = m_ShaderMap.find("Normal_ItemUI");
		if (iter != m_ShaderMap.end())
			dynamic_cast<CNoramlItemUIShader*>((*iter).second)->setRender(false);
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
	//static int sec = 0, min = 0;
	//if (((CUIShader*)m_ppShaders[5])->getTime() >= 10)
	//	((CUIShader*)m_ppShaders[6])->setTime(++sec);
	//if (((CUIShader*)m_ppShaders[6])->getTime() >= 6)
	//{
	//	sec = 0;
	//	((CUIShader*)m_ppShaders[7])->setTime(++min);
	//}
	//if (((CUIShader*)m_ppShaders[7])->getTime() >= 10)
	//	min = 0;

	for (int i = 0; i < m_nShaders; i++)
	{
		if (m_ppShaders[i])
			m_ppShaders[i]->Render(pd3dCommandList, pCamera);
	}
}