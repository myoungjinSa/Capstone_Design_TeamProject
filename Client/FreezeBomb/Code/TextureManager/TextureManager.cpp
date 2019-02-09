#include "../Stdafx/Stdafx.h"
#include "TextureManager.h"
#include "../Texture/Texture.h"
#include "../Scene/Scene.h"

CTextureManager::CTextureManager()
{
}

CTextureManager::~CTextureManager()
{
}

void CTextureManager::Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
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

	m_TextureMap.emplace(0, pNumber0Texture);
	m_TextureMap.emplace(1, pNumber1Texture);
	m_TextureMap.emplace(2, pNumber2Texture);
	m_TextureMap.emplace(3, pNumber3Texture);
	m_TextureMap.emplace(4, pNumber4Texture);
	m_TextureMap.emplace(5, pNumber5Texture);
	m_TextureMap.emplace(6, pNumber6Texture);
	m_TextureMap.emplace(7, pNumber7Texture);
	m_TextureMap.emplace(8, pNumber8Texture);
	m_TextureMap.emplace(9, pNumber9Texture);
}

void CTextureManager::Release()
{
	for (auto iter = m_TextureMap.begin(); iter != m_TextureMap.end(); )
		iter = m_TextureMap.erase(iter);

	m_TextureMap.clear();
}