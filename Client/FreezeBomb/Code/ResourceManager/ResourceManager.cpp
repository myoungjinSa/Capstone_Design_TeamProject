#include "../Stdafx/Stdafx.h"
#include "ResourceManager.h"
#include "../Texture/Texture.h"
#include "../Scene/Scene.h"

CResourceManager::CResourceManager()
{
}

CResourceManager::~CResourceManager()
{
}

void CResourceManager::Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	LoadTexture(pd3dDevice, pd3dCommandList);
	LoadBound(pd3dDevice, pd3dCommandList);
}

void CResourceManager::LoadTexture(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	CTexture* pSkyBoxTexture = new CTexture(1, RESOURCE_TEXTURE_CUBE, 0);
	pSkyBoxTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Resource/Textures/SkyBox/SkyBox_1.dds", 0);
	CScene::CreateShaderResourceViews(pd3dDevice, pSkyBoxTexture, 10, false);
	m_TextureMap.emplace("SkyBox", pSkyBoxTexture);

	CTexture* pTerrainBaseTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	pTerrainBaseTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Resource/Textures/Terrain/BaseTerrain.dds", 0);
	CTexture* pTerrainSpecularTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	pTerrainSpecularTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Resource/Textures/Terrain/SpecularTerrain.dds", 0);
	CScene::CreateShaderResourceViews(pd3dDevice, pTerrainBaseTexture, 13, false);
	CScene::CreateShaderResourceViews(pd3dDevice, pTerrainSpecularTexture, 14, false);
	m_TextureMap.emplace("BaseTerrain", pTerrainBaseTexture);
	m_TextureMap.emplace("SpecularTerrain", pTerrainSpecularTexture);

	CTexture* pNumber0Texture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	pNumber0Texture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Resource/Textures/Number/Blue_0.dds", 0);
	CTexture* pNumber1Texture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	pNumber1Texture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Resource/Textures/Number/Blue_1.dds", 0);
	CTexture* pNumber2Texture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	pNumber2Texture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Resource/Textures/Number/Blue_2.dds", 0);
	CTexture* pNumber3Texture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	pNumber3Texture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Resource/Textures/Number/Blue_3.dds", 0);
	CTexture* pNumber4Texture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	pNumber4Texture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Resource/Textures/Number/Blue_4.dds", 0);
	CTexture* pNumber5Texture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	pNumber5Texture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Resource/Textures/Number/Blue_5.dds", 0);
	CTexture* pNumber6Texture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	pNumber6Texture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Resource/Textures/Number/Blue_6.dds", 0);
	CTexture* pNumber7Texture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	pNumber7Texture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Resource/Textures/Number/Blue_7.dds", 0);
	CTexture* pNumber8Texture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	pNumber8Texture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Resource/Textures/Number/Blue_8.dds", 0);
	CTexture* pNumber9Texture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	pNumber9Texture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Resource/Textures/Number/Blue_9.dds", 0);

	CScene::CreateShaderResourceViews(pd3dDevice, pNumber0Texture, 17, false);
	CScene::CreateShaderResourceViews(pd3dDevice, pNumber1Texture, 17, false);
	CScene::CreateShaderResourceViews(pd3dDevice, pNumber2Texture, 17, false);
	CScene::CreateShaderResourceViews(pd3dDevice, pNumber3Texture, 17, false);
	CScene::CreateShaderResourceViews(pd3dDevice, pNumber4Texture, 17, false);
	CScene::CreateShaderResourceViews(pd3dDevice, pNumber5Texture, 17, false);
	CScene::CreateShaderResourceViews(pd3dDevice, pNumber6Texture, 17, false);
	CScene::CreateShaderResourceViews(pd3dDevice, pNumber7Texture, 17, false);
	CScene::CreateShaderResourceViews(pd3dDevice, pNumber8Texture, 17, false);
	CScene::CreateShaderResourceViews(pd3dDevice, pNumber9Texture, 17, false);

	int i = 0;
	m_TextureMap.emplace(to_string(i++), pNumber0Texture);
	m_TextureMap.emplace(to_string(i++), pNumber1Texture);
	m_TextureMap.emplace(to_string(i++), pNumber2Texture);
	m_TextureMap.emplace(to_string(i++), pNumber3Texture);
	m_TextureMap.emplace(to_string(i++), pNumber4Texture);
	m_TextureMap.emplace(to_string(i++), pNumber5Texture);
	m_TextureMap.emplace(to_string(i++), pNumber6Texture);
	m_TextureMap.emplace(to_string(i++), pNumber7Texture);
	m_TextureMap.emplace(to_string(i++), pNumber8Texture);
	m_TextureMap.emplace(to_string(i++), pNumber9Texture);

	CTexture* pColonTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	pColonTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Resource/Textures/Number/Blue_Colon.dds", 0);
	CScene::CreateShaderResourceViews(pd3dDevice, pColonTexture, 17, false);
	m_TextureMap.emplace("Colon", pColonTexture);

	CTexture* pItemBoxTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	pItemBoxTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Resource/Textures/Item/Item_Box.dds", 0);
	CScene::CreateShaderResourceViews(pd3dDevice, pItemBoxTexture, 17, false);
	m_TextureMap.emplace("ItemBox", pItemBoxTexture);

	CTexture* pHammer_ItemTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	pHammer_ItemTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Resource/Textures/Item/Hammer_Item.dds", 0);
	CScene::CreateShaderResourceViews(pd3dDevice, pHammer_ItemTexture, 17, false);
	m_TextureMap.emplace("Hammer_Item", pHammer_ItemTexture);

	CTexture* pGoldHammer_ItemTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	pGoldHammer_ItemTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Resource/Textures/Item/GoldHammer_Item.dds", 0);
	CScene::CreateShaderResourceViews(pd3dDevice, pGoldHammer_ItemTexture, 17, false);
	m_TextureMap.emplace("GoldHammer_Item", pGoldHammer_ItemTexture);
	CTexture* pGoldTimer_ItemTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	pGoldTimer_ItemTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Resource/Textures/Item/GoldTimer_Item.dds", 0);
	CScene::CreateShaderResourceViews(pd3dDevice, pGoldTimer_ItemTexture, 17, false);
	m_TextureMap.emplace("GoldTimer_Item", pGoldTimer_ItemTexture);


	CTexture* pIceCubeTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	pIceCubeTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Resource/Textures/Model/texture2.dds", 0);
	CScene::CreateShaderResourceViews(pd3dDevice, pIceCubeTexture, 20, false);
	m_TextureMap.emplace("IceTexture", pIceCubeTexture);

}

#define SIZE 22
void CResourceManager::LoadBound(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	string filename = "../Resource/Bounds/ModelBounds.bin";
	ifstream in(filename, ios::binary);
	if (!in)
	{
		cout << filename << " - 바이너리 파일 없음" << endl;
		return;
	}

	for (int i = 0; i < SIZE; ++i)
	{
		Bounds* pBound = new Bounds;

		size_t FrameNameSize = 0;
		in.read(reinterpret_cast<char*>(&FrameNameSize), sizeof(size_t));

		char* p = new char[FrameNameSize + 1];
		in.read(p, sizeof(char) * FrameNameSize);
		p[FrameNameSize] = '\0';
		pBound->m_Name = p;
		delete[] p;

		size_t BoundNameSize = 0;
		in.read(reinterpret_cast<char*>(&BoundNameSize), sizeof(size_t));

		p = new char[BoundNameSize + 1];
		in.read(p, sizeof(char) * BoundNameSize);
		p[BoundNameSize] = '\0';
		string BoundName = p;
		delete[] p;

		in.read(reinterpret_cast<char*>(&pBound->m_xmf3Center.x), sizeof(float));
		in.read(reinterpret_cast<char*>(&pBound->m_xmf3Center.y), sizeof(float));
		in.read(reinterpret_cast<char*>(&pBound->m_xmf3Center.z), sizeof(float));
		in.read(reinterpret_cast<char*>(&pBound->m_xmf3Extent.x), sizeof(float));
		in.read(reinterpret_cast<char*>(&pBound->m_xmf3Extent.y), sizeof(float));
		in.read(reinterpret_cast<char*>(&pBound->m_xmf3Extent.z), sizeof(float));

		m_BoundMap.emplace(pBound->m_Name, pBound);
	}
	in.close();
}

void CResourceManager::Release()
{
	for (auto iter = m_TextureMap.begin(); iter != m_TextureMap.end(); )
		iter = m_TextureMap.erase(iter);

	m_TextureMap.clear();

	for (auto iter = m_BoundMap.begin(); iter != m_BoundMap.end(); )
	{
		delete(*iter).second;
		iter = m_BoundMap.erase(iter);
	}
	m_BoundMap.clear();
}