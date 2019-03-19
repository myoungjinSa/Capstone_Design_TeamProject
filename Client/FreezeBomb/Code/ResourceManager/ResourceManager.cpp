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

void CResourceManager::Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature)
{
	LoadTexture(pd3dDevice, pd3dCommandList);
	
	LoadModel(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	LoadMapObjectInfo(pd3dDevice, pd3dCommandList);

	LoadBound(pd3dDevice, pd3dCommandList);

}

//void CResourceManager::CreateOffScreenRenderTargeViews(ID3D12Device *pd3dDevice,ID3D12GraphicsCommandList *pd3dCommandList,int clientWidth,int clientHeight)
//{
//	CTexture *pTextureForCartoonProcessing = new CTexture(m_nCartoonScreenRenderTargetBuffers, RESOURCE_TEXTURE2D_ARRAY, 0);
//
//	D3D12_CLEAR_VALUE d3dClearValue = { DXGI_FORMAT_R8G8B8A8_UNORM, { 0.0f, 0.0f, 0.0f, 1.0f } };
//
//	for (UINT i = 0; i < m_nCartoonScreenRenderTargetBuffers; i++)
//	{
//		m_ppd3dCartoonScreenRenderTargetBuffers[i] = pTextureForCartoonProcessing->CreateTexture(pd3dDevice, pd3dCommandList, clientWidth, clientHeight, DXGI_FORMAT_R8G8B8A8_UNORM, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, i);
//		m_ppd3dCartoonScreenRenderTargetBuffers[i]->AddRef();
//		m_TextureMap.emplace("CartoonRenderTarget" + to_string(i), pTextureForCartoonProcessing);
//	}
//
//	//m_TextureMap.emplace("CartoonRenderTarget"+to_string(i),)
//
//
//}

void CResourceManager::LoadTexture(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	//CreateOffScreenRenderTargeViews(pd3dDevice,pd3dCommandList,clientWidth,clientHeight); //OffScreen RenderTarget 생성

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

	CTexture* pSnowTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	pSnowTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Resource/Textures/Effect/Snow.dds", 0);
	CScene::CreateShaderResourceViews(pd3dDevice, pSnowTexture, 16, false);
	m_TextureMap.emplace("Snow", pSnowTexture);

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
	m_TextureMap.emplace("NormalHammer", pHammer_ItemTexture);

	CTexture* pGoldHammer_ItemTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	pGoldHammer_ItemTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Resource/Textures/Item/GoldHammer_Item.dds", 0);
	CScene::CreateShaderResourceViews(pd3dDevice, pGoldHammer_ItemTexture, 17, false);
	m_TextureMap.emplace("GoldHammer", pGoldHammer_ItemTexture);
	CTexture* pGoldTimer_ItemTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	pGoldTimer_ItemTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Resource/Textures/Item/GoldTimer_Item.dds", 0);
	CScene::CreateShaderResourceViews(pd3dDevice, pGoldTimer_ItemTexture, 17, false);
	m_TextureMap.emplace("GoldTimer", pGoldTimer_ItemTexture);

	CTexture* pIceCubeTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	pIceCubeTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Resource/Textures/Model/texture2.dds", 0);
	CScene::CreateShaderResourceViews(pd3dDevice, pIceCubeTexture, 20, false);
	m_TextureMap.emplace("IceTexture", pIceCubeTexture);
}

void CResourceManager::LoadModel(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature)
{
	int nDeadTrees = 5, nPineTrees = 8, nBigRocks = 4, nDeers = 1, nPond = 1, nFence = 2;
	// 15
	CLoadedModelInfo* pDeadTreeModel01 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_DeadTrunk_01.bin", nullptr, false, "Surrounding");
	m_ModelMap.emplace("SM_DeadTrunk_01", pDeadTreeModel01);
	CLoadedModelInfo* pDeadTreeModel02 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_DeadTrunk_02.bin", nullptr, false, "Surrounding");
	m_ModelMap.emplace("SM_DeadTrunk_02", pDeadTreeModel02);
	CLoadedModelInfo* pDeadTreeModel03 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_DeadTrunk_03.bin", nullptr, false, "Surrounding");
	m_ModelMap.emplace("SM_DeadTrunk_03", pDeadTreeModel03);
	CLoadedModelInfo* pDeadTreeModel04 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_DeadTrunk_04.bin", nullptr, false, "Surrounding");
	m_ModelMap.emplace("SM_DeadTrunk_04", pDeadTreeModel04);
	CLoadedModelInfo* pDeadTreeModel05 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_DeadTrunk_05.bin", nullptr, false, "Surrounding");
	m_ModelMap.emplace("SM_DeadTrunk_05", pDeadTreeModel05);
	// 34
	CLoadedModelInfo* pPineTreeModel01 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_PineTree_Snow_01.bin", nullptr, false, "Surrounding");
	m_ModelMap.emplace("SM_PineTree_Snow_01", pPineTreeModel01);
	CLoadedModelInfo* pPineTreeModel02 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_PineTree_Snow_02.bin", nullptr, false, "Surrounding");
	m_ModelMap.emplace("SM_PineTree_Snow_02", pPineTreeModel02);
	CLoadedModelInfo* pPineTreeModel03 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_PineTree_Snow_03.bin", nullptr, false, "Surrounding");
	m_ModelMap.emplace("SM_PineTree_Snow_03", pPineTreeModel03);
	CLoadedModelInfo* pPineTreeModel04 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_PineTree_Snow_04.bin", nullptr, false, "Surrounding");
	m_ModelMap.emplace("SM_PineTree_Snow_04", pPineTreeModel04);
	CLoadedModelInfo* pPineTreeModel05 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_PineTree_Snow_05.bin", nullptr, false, "Surrounding");
	m_ModelMap.emplace("SM_PineTree_Snow_05", pPineTreeModel05);
	CLoadedModelInfo* pPineTreeModel06 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_PineTree_Snow_06.bin", nullptr, false, "Surrounding");
	m_ModelMap.emplace("SM_PineTree_Snow_06", pPineTreeModel06);
	CLoadedModelInfo* pPineTreeModel07 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_PineTree_Snow_07.bin", nullptr, false, "Surrounding");
	m_ModelMap.emplace("SM_PineTree_Snow_07", pPineTreeModel07);
	CLoadedModelInfo* pPineTreeModel08 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_PineTree_Snow_08.bin", nullptr, false, "Surrounding");
	m_ModelMap.emplace("SM_PineTree_Snow_08", pPineTreeModel08);
	// 14
	CLoadedModelInfo* pRock01 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_BigPlainRock_Snow_01.bin", nullptr, false, "Surrounding");
	m_ModelMap.emplace("SM_BigPlainRock_Snow_01", pRock01);
	CLoadedModelInfo* pRock02 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_BigPlainRock_Snow_02.bin", nullptr, false, "Surrounding");
	m_ModelMap.emplace("SM_BigPlainRock_Snow_02", pRock02);
	CLoadedModelInfo* pRock03 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_BigPlainRock_Snow_03.bin", nullptr, false, "Surrounding");
	m_ModelMap.emplace("SM_BigPlainRock_Snow_03", pRock03);
	CLoadedModelInfo* pRock04 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_BigPlainRock_Snow_04.bin", nullptr, false, "Surrounding");
	m_ModelMap.emplace("SM_BigPlainRock_Snow_04", pRock04);
	// 2
	CLoadedModelInfo* pDeer = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_Deer.bin", nullptr, false, "Surrounding");
	m_ModelMap.emplace("SM_Deer", pDeer);
	// 2
	CLoadedModelInfo* pPond = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/PondSquare.bin", nullptr, false, "Surrounding");
	m_ModelMap.emplace("PondSquare", pPond);
	// 0
	CLoadedModelInfo* pFence01 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/LowPoly_Fence_01.bin", nullptr, false, "Surrounding");
	m_ModelMap.emplace("LowPoly_-_Fence_A", pFence01);
	CLoadedModelInfo* pFence02 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/LowPoly_Fence_02.bin", nullptr, false, "Surrounding");
	m_ModelMap.emplace("LowPoly_-_Fence_B", pFence02);

	//CLoadedModelInfo* pHammer = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/Hammer.bin", nullptr, false, "Hammer");
	CLoadedModelInfo* pHammer = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/Hammer2.bin", nullptr, false, "Hammer");
	m_ModelMap.emplace("Hammer", pHammer);

	CLoadedModelInfo* pGoldTimer = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/Pocket_Watch.bin", nullptr, false, "GoldTimer");
	m_ModelMap.emplace("GoldTimer", pGoldTimer);

	CLoadedModelInfo* pEvilBearModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/EvilBear.bin", nullptr, true, "Enemy");
	m_ModelMap.emplace("EvilBear", pEvilBearModel);
}

void CResourceManager::LoadMapObjectInfo(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	string filename = "../Resource/Position/Surrounding/MapVer1.bin";

	ifstream in(filename, ios::binary);

	if (!in)
	{
		cout << filename << " - 바이너리 파일 없음" << endl;
		return;
	}

	size_t nReads = 0;

	// 맵 오브젝트 개수
	int nObjects = 0;
	in.read(reinterpret_cast<char*>(&nObjects), sizeof(int));

	for (int i = 0; i < nObjects; ++i)
	{
		MapObjectInfo* pMapObjectInfo = new MapObjectInfo;
		// 모델 이름 문자열 길이 저장
		in.read(reinterpret_cast<char*>(&nReads), sizeof(size_t));

		// 길이 + 1만큼 자원 할당
		char* p = new char[nReads + 1];
		in.read(p, sizeof(char) * nReads);
		p[nReads] = '\0';
		//  모델 이름 저장
		pMapObjectInfo->m_Name = p;
		delete[] p;

		// Position 문자열 길이 저장
		in.read(reinterpret_cast<char*>(&nReads), sizeof(size_t));
		p = new char[nReads + 1];
		in.read(p, sizeof(char)*nReads);
		p[nReads] = '\0';
		delete[] p;

		// Position x, y, z값 저장
		in.read(reinterpret_cast<char*>(&pMapObjectInfo->m_Position.x), sizeof(float));
		in.read(reinterpret_cast<char*>(&pMapObjectInfo->m_Position.y), sizeof(float));
		in.read(reinterpret_cast<char*>(&pMapObjectInfo->m_Position.z), sizeof(float));

		// Look 문자열 길이 저장
		in.read(reinterpret_cast<char*>(&nReads), sizeof(size_t));
		p = new char[nReads + 1];
		in.read(p, sizeof(char)*nReads);
		p[nReads] = '\0';
		delete[] p;

		// <Look> x, y, z값 저장
		in.read(reinterpret_cast<char*>(&pMapObjectInfo->m_Look.x), sizeof(float));
		in.read(reinterpret_cast<char*>(&pMapObjectInfo->m_Look.y), sizeof(float));
		in.read(reinterpret_cast<char*>(&pMapObjectInfo->m_Look.z), sizeof(float));
		
		// Up 문자열 길이 저장
		in.read(reinterpret_cast<char*>(&nReads), sizeof(size_t));
		p = new char[nReads + 1];
		in.read(p, sizeof(char)*nReads);
		p[nReads] = '\0';
		delete[] p;

		// <Up> x, y, z값 저장
		in.read(reinterpret_cast<char*>(&pMapObjectInfo->m_Up.x), sizeof(float));
		in.read(reinterpret_cast<char*>(&pMapObjectInfo->m_Up.y), sizeof(float));
		in.read(reinterpret_cast<char*>(&pMapObjectInfo->m_Up.z), sizeof(float));

		// Right 문자열 길이 저장
		in.read(reinterpret_cast<char*>(&nReads), sizeof(size_t));
		p = new char[nReads + 1];
		in.read(p, sizeof(char)*nReads);
		p[nReads] = '\0';
		delete[] p;

		// <Right> x, y, z값 저장
		in.read(reinterpret_cast<char*>(&pMapObjectInfo->m_Right.x), sizeof(float));
		in.read(reinterpret_cast<char*>(&pMapObjectInfo->m_Right.y), sizeof(float));
		in.read(reinterpret_cast<char*>(&pMapObjectInfo->m_Right.z), sizeof(float));

		pMapObjectInfo->m_Position.y = 0.f;

		m_MapObjectInfoMultiMap.emplace(pMapObjectInfo->m_Name, pMapObjectInfo);
	}
	in.close();
}

void CResourceManager::LoadBound(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	string filename = "../Resource/Bounds/ModelBounds.bin";
	ifstream in(filename, ios::binary);
	if (!in)
	{
		cout << filename << " - 바이너리 파일 없음" << endl;
		return;
	}

	const int size = 25;
	for (int i = 0; i < size; ++i)
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
		delete (*iter).second;
		iter = m_BoundMap.erase(iter);
	}
	m_BoundMap.clear();

	for (auto iter = m_MapObjectInfoMultiMap.begin(); iter != m_MapObjectInfoMultiMap.end(); )
	{
		delete (*iter).second;
		iter = m_MapObjectInfoMultiMap.erase(iter);
	}
	m_MapObjectInfoMultiMap.clear();
}

void CResourceManager::ReleaseModel()
{
	for (auto iter = m_ModelMap.begin(); iter != m_ModelMap.end(); )
	{
		if ((*iter).second)
		{
			delete (*iter).second;
			iter = m_ModelMap.erase(iter);
		}
	}
	m_ModelMap.clear();
}