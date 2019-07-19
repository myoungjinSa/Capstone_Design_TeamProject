#include "../Stdafx/Stdafx.h"
#include "ResourceManager.h"
#include "../Texture/Texture.h"
#include "../Scene/Scene.h"
#include "../GameObject/Foliage/Foliage.h"
#include "../Material/Material.h"
#include "../SoundSystem/SoundSystem.h"

volatile size_t g_TotalSize = 0;
volatile size_t g_FileSize = 0;
extern volatile bool g_IsloadingStart = false;

vector<MapObjectInfo*> CResourceManager::m_CharactersInfoMap;
CResourceManager::CResourceManager()
{
}

CResourceManager::~CResourceManager()
{
}

void CResourceManager::Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature)
{
	CSoundSystem::Initialize();
	PrepareLoad();
	g_IsloadingStart = true;

	LoadTexture(pd3dDevice, pd3dCommandList);
	LoadModel(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	LoadMapObjectInfo(pd3dDevice, pd3dCommandList);
	LoadCharactersInfo(pd3dDevice, pd3dCommandList);
	LoadBound(pd3dDevice, pd3dCommandList);
}

void CResourceManager::PrepareSound()
{
	struct info
	{
		info(CSoundSystem::SOUND_TYPE t, string s, int m)
			: type(t), path(s), mode(m) {}

		CSoundSystem::SOUND_TYPE type;
		string path;
		int mode;
	};

	vector<info> v;
	v.emplace_back(info(CSoundSystem::SOUND_TYPE::LOBBY_BGM, "../Resource/Sound/Used/BGM/Remembrance.wav", FMOD_LOOP_NORMAL));
	v.emplace_back(info(CSoundSystem::SOUND_TYPE::INGAME_BGM, "../Resource/Sound/Used/BGM/SnowDrop.wav", FMOD_LOOP_NORMAL));

	v.emplace_back(info(CSoundSystem::SOUND_TYPE::CHARACTER_SELECT, "../Resource/Sound/Used/Effect/MouseOver.wav", FMOD_LOOP_OFF));
	v.emplace_back(info(CSoundSystem::SOUND_TYPE::TIMER_WARNING, "../Resource/Sound/Used/Effect/TimerWarning.wav", FMOD_LOOP_NORMAL));
	v.emplace_back(info(CSoundSystem::SOUND_TYPE::ICE_BREAK, "../Resource/Sound/Used/Effect/ICEBreak.wav", FMOD_LOOP_OFF));
	v.emplace_back(info(CSoundSystem::SOUND_TYPE::GET_ITEM, "../Resource/Sound/Used/Effect/GetItem.wav", FMOD_LOOP_OFF));
	v.emplace_back(info(CSoundSystem::SOUND_TYPE::CLICK, "../Resource/Sound/Used/Effect/Click.wav", FMOD_LOOP_OFF));

	v.emplace_back(info(CSoundSystem::SOUND_TYPE::RUN1, "../Resource/Sound/Used/Effect/Run1.wav", FMOD_LOOP_OFF));
	//v.emplace_back(info(CSoundSystem::SOUND_TYPE::RUN2, "../Resource/Sound/Used/Effect/Run2.wav", FMOD_LOOP_OFF));
	v.emplace_back(info(CSoundSystem::SOUND_TYPE::RUN2, "../Resource/Sound/Used/Effect/Run1.wav", FMOD_LOOP_OFF));
	v.emplace_back(info(CSoundSystem::SOUND_TYPE::BOMBEXPLOSION_EFFECT, "../Resource/Sound/Used/Effect/BombExplosion_Effect.wav", FMOD_LOOP_OFF));
	v.emplace_back(info(CSoundSystem::SOUND_TYPE::GOLDTIMER_EFFECT, "../Resource/Sound/Used/Effect/GoldTimer_Effect.wav", FMOD_LOOP_OFF));
	v.emplace_back(info(CSoundSystem::SOUND_TYPE::HAMMERSWING_EFFECT, "../Resource/Sound/Used/Effect/HammerSwing_Effect.wav", FMOD_LOOP_OFF));
	v.emplace_back(info(CSoundSystem::SOUND_TYPE::GOLDHAMMER_EFFECT, "../Resource/Sound/Used/Effect/GoldHammer_Effect.wav", FMOD_LOOP_OFF));
	v.emplace_back(info(CSoundSystem::SOUND_TYPE::FIRE_SOUND, "../Resource/Sound/Used/Effect/Fire.mp3", FMOD_LOOP_NORMAL));


	for (int i = 0; i < v.size(); ++i)
	{
		ifstream in(v[i].path, ios::binary);
		if (!in)
		{
			cout << v[i].path << " 파일이 없습니다." << endl;
			break;
		}

		in.seekg(0, ios::end);

		size_t fileSize = in.tellg();
		g_TotalSize += fileSize;

		in.seekg(0, ios::beg);
		in.close();

		CSoundSystem::AddGameSound(v[i].type, v[i].path, fileSize, v[i].mode);
	}
}

void CResourceManager::PrepareLoad()
{
	m_TextureInfoMap.emplace("SkyBox1", TextureInfo(RESOURCE_TEXTURE_CUBE, L"../Resource/Textures/SkyBox/SkyBox_1.dds", 9));
	m_TextureInfoMap.emplace("SkyBox2", TextureInfo(RESOURCE_TEXTURE_CUBE, L"../Resource/Textures/SkyBox/SkyBox_0.dds", 9));

	m_TextureInfoMap.emplace("BaseTerrain", TextureInfo(RESOURCE_TEXTURE2D, L"../Resource/Textures/Terrain/BaseTerrain.dds", 10));
	m_TextureInfoMap.emplace("SpecularTerrain", TextureInfo(RESOURCE_TEXTURE2D, L"../Resource/Textures/Terrain/SpecularTerrain.dds", 11));

	m_TextureInfoMap.emplace("Particle", TextureInfo(RESOURCE_TEXTURE2D, L"../Resource/Textures/Effect/LampParticle.dds", 19));
	m_TextureInfoMap.emplace("IceTexture", TextureInfo(RESOURCE_TEXTURE2D, L"../Resource/Textures/Model/texture2.dds", 20));
	m_TextureInfoMap.emplace("Snow", TextureInfo(RESOURCE_TEXTURE2D, L"../Resource/Textures/Effect/Snow.dds", 21));

	int i = 0;
	m_TextureInfoMap.emplace(to_string(i++), TextureInfo(RESOURCE_TEXTURE2D, L"../Resource/Textures/Number/Blue_0.dds", 22));
	m_TextureInfoMap.emplace(to_string(i++), TextureInfo(RESOURCE_TEXTURE2D, L"../Resource/Textures/Number/Blue_1.dds", 22));
	m_TextureInfoMap.emplace(to_string(i++), TextureInfo(RESOURCE_TEXTURE2D, L"../Resource/Textures/Number/Blue_2.dds", 22));
	m_TextureInfoMap.emplace(to_string(i++), TextureInfo(RESOURCE_TEXTURE2D, L"../Resource/Textures/Number/Blue_3.dds", 22));
	m_TextureInfoMap.emplace(to_string(i++), TextureInfo(RESOURCE_TEXTURE2D, L"../Resource/Textures/Number/Blue_4.dds", 22));
	m_TextureInfoMap.emplace(to_string(i++), TextureInfo(RESOURCE_TEXTURE2D, L"../Resource/Textures/Number/Blue_5.dds", 22));
	m_TextureInfoMap.emplace(to_string(i++), TextureInfo(RESOURCE_TEXTURE2D, L"../Resource/Textures/Number/Blue_6.dds", 22));
	m_TextureInfoMap.emplace(to_string(i++), TextureInfo(RESOURCE_TEXTURE2D, L"../Resource/Textures/Number/Blue_7.dds", 22));
	m_TextureInfoMap.emplace(to_string(i++), TextureInfo(RESOURCE_TEXTURE2D, L"../Resource/Textures/Number/Blue_8.dds", 22));
	m_TextureInfoMap.emplace(to_string(i++), TextureInfo(RESOURCE_TEXTURE2D, L"../Resource/Textures/Number/Blue_9.dds", 22));

	m_TextureInfoMap.emplace("Colon", TextureInfo(RESOURCE_TEXTURE2D, L"../Resource/Textures/Number/Blue_Colon.dds", 22));

	m_TextureInfoMap.emplace("ItemBox", TextureInfo(RESOURCE_TEXTURE2D, L"../Resource/Textures/Item/Item_Box.dds", 22));
	m_TextureInfoMap.emplace("NormalHammer", TextureInfo(RESOURCE_TEXTURE2D, L"../Resource/Textures/Item/Hammer_Item.dds", 22));
	m_TextureInfoMap.emplace("GoldHammer", TextureInfo(RESOURCE_TEXTURE2D, L"../Resource/Textures/Item/GoldHammer_Item.dds", 22));
	m_TextureInfoMap.emplace("GoldTimer", TextureInfo(RESOURCE_TEXTURE2D, L"../Resource/Textures/Item/GoldTimer_Item.dds", 22));

	m_TextureInfoMap.emplace("MenuBoard", TextureInfo(RESOURCE_TEXTURE2D, L"../Resource/Textures/Menu/MenuBoard.dds", 22));
	m_TextureInfoMap.emplace("Menu_ICON", TextureInfo(RESOURCE_TEXTURE2D, L"../Resource/Textures/Menu/Menu_ICON.dds", 22));
	m_TextureInfoMap.emplace("Option", TextureInfo(RESOURCE_TEXTURE2D, L"../Resource/Textures/Menu/Option.dds", 22));
	m_TextureInfoMap.emplace("GameOver", TextureInfo(RESOURCE_TEXTURE2D, L"../Resource/Textures/Menu/GameOver.dds", 22));
	m_TextureInfoMap.emplace("Sound", TextureInfo(RESOURCE_TEXTURE2D, L"../Resource/Textures/Menu/Sound.dds", 22));
	m_TextureInfoMap.emplace("Cartoon", TextureInfo(RESOURCE_TEXTURE2D, L"../Resource/Textures/Menu/Cartoon.dds", 22));

	m_TextureInfoMap.emplace("Win", TextureInfo(RESOURCE_TEXTURE2D, L"../Resource/Textures/Text/Win.dds", 22));
	m_TextureInfoMap.emplace("Lose", TextureInfo(RESOURCE_TEXTURE2D, L"../Resource/Textures/Text/Lose.dds", 22));

	//m_TextureInfoMap.emplace("UseTimer_Effect", TextureInfo(RESOURCE_TEXTURE2D, L"../Resource/Textures/Effect/airflow_01.dds", 25));

	for (auto iter = m_TextureInfoMap.begin(); iter != m_TextureInfoMap.end(); ++iter)
	{
		wifstream in((*iter).second.m_Filename, ios::binary);
		if (!in)
		{
			cout << (*iter).first << " - 파일 없음" << endl;
			break;
		}

		in.seekg(0, ios::end);
		(*iter).second.m_FileSize = in.tellg();
		in.seekg(0, ios::beg);
		in.close();

		g_TotalSize += (*iter).second.m_FileSize;
	}

	// 15
	m_ModelInfoMap.emplace("SM_DeadTrunk_01", ModelInfo("../Resource/Models/SM_DeadTrunk_01.bin", false));
	m_ModelInfoMap.emplace("SM_DeadTrunk_02", ModelInfo("../Resource/Models/SM_DeadTrunk_02.bin", false));
	m_ModelInfoMap.emplace("SM_DeadTrunk_03", ModelInfo("../Resource/Models/SM_DeadTrunk_03.bin", false));
	m_ModelInfoMap.emplace("SM_DeadTrunk_04", ModelInfo("../Resource/Models/SM_DeadTrunk_04.bin", false));
	m_ModelInfoMap.emplace("SM_DeadTrunk_05", ModelInfo("../Resource/Models/SM_DeadTrunk_05.bin", false));
	// 34
	m_ModelInfoMap.emplace("SM_PineTree_Snow_01", ModelInfo("../Resource/Models/SM_PineTree_Snow_01.bin", false));
	m_ModelInfoMap.emplace("SM_PineTree_Snow_02", ModelInfo("../Resource/Models/SM_PineTree_Snow_02.bin", false));
	m_ModelInfoMap.emplace("SM_PineTree_Snow_03", ModelInfo("../Resource/Models/SM_PineTree_Snow_03.bin", false));
	m_ModelInfoMap.emplace("SM_PineTree_Snow_04", ModelInfo("../Resource/Models/SM_PineTree_Snow_04.bin", false));
	m_ModelInfoMap.emplace("SM_PineTree_Snow_05", ModelInfo("../Resource/Models/SM_PineTree_Snow_05.bin", false));
	m_ModelInfoMap.emplace("SM_PineTree_Snow_06", ModelInfo("../Resource/Models/SM_PineTree_Snow_06.bin", false));
	m_ModelInfoMap.emplace("SM_PineTree_Snow_07", ModelInfo("../Resource/Models/SM_PineTree_Snow_07.bin", false));
	m_ModelInfoMap.emplace("SM_PineTree_Snow_08", ModelInfo("../Resource/Models/SM_PineTree_Snow_08.bin", false));
	// 14
	m_ModelInfoMap.emplace("SM_BigPlainRock_Snow_01", ModelInfo("../Resource/Models/SM_BigPlainRock_Snow_01.bin", false));
	m_ModelInfoMap.emplace("SM_BigPlainRock_Snow_02", ModelInfo("../Resource/Models/SM_BigPlainRock_Snow_02.bin", false));
	m_ModelInfoMap.emplace("SM_BigPlainRock_Snow_03", ModelInfo("../Resource/Models/SM_BigPlainRock_Snow_03.bin", false));
	m_ModelInfoMap.emplace("SM_BigPlainRock_Snow_04", ModelInfo("../Resource/Models/SM_BigPlainRock_Snow_04.bin", false));
	// 2
	m_ModelInfoMap.emplace("SM_Deer", ModelInfo("../Resource/Models/SM_Deer.bin", false));
	// 2
	m_ModelInfoMap.emplace("Frozen_Road", ModelInfo("../Resource/Models/Frozen_Road.bin", false));
	// 0
	m_ModelInfoMap.emplace("LowPolyFence_01", ModelInfo("../Resource/Models/LowPolyFence_01.bin", false));
	// 0
	m_ModelInfoMap.emplace("LowPolyFence_02", ModelInfo("../Resource/Models/LowPolyFence_02.bin", false));

	m_ModelInfoMap.emplace("Hammer", ModelInfo("../Resource/Models/Hammer.bin", false));

	m_ModelInfoMap.emplace("GoldTimer", ModelInfo("../Resource/Models/Pocket_Watch.bin", false));

	m_ModelInfoMap.emplace("EvilBear", ModelInfo("../Resource/Models/EvilBear.bin", true));

	// 3
	m_ModelInfoMap.emplace("SM_FirePit", ModelInfo("../Resource/Models/SM_FirePit.bin", false));

#ifdef _MAPTOOL_MODE_
	m_ModelInfoMap.emplace("Foliage0", ModelInfo("../Resource/Models/Grass_C_01.bin", false));
	m_ModelInfoMap.emplace("Foliage1", ModelInfo("../Resource/Models/Grass_D_01.bin", false));
	m_ModelInfoMap.emplace("Foliage2", ModelInfo("../Resource/Models/Plant_c_01.bin", false));
#endif

	//Effect 1
	//m_ModelInfoMap.emplace("wind", ModelInfo("../Resource/Models/wind.bin", false));

	for (auto iter = m_ModelInfoMap.begin(); iter != m_ModelInfoMap.end(); ++iter)
	{
		wifstream in((*iter).second.m_Filename, ios::binary);
		if (!in)
		{
			cout << (*iter).first << " - 파일 없음" << endl;
			break;
		}

		in.seekg(0, ios::end);
		g_TotalSize += in.tellg();
		in.seekg(0, ios::beg);
		in.close();
	}
}

void CResourceManager::LoadTexture(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	for (auto iter = m_TextureInfoMap.begin(); iter != m_TextureInfoMap.end(); ++iter)
	{
		CTexture* pTexture = new CTexture(1, (*iter).second.m_ResourceType, 0);
		pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, const_cast<wchar_t*>((*iter).second.m_Filename.c_str()), 0);
		CScene::CreateShaderResourceViews(pd3dDevice, pTexture, (*iter).second.m_RootParameter, false);
		g_FileSize += (*iter).second.m_FileSize;
		m_TextureMap.emplace((*iter).first, pTexture);
	}
}

void CResourceManager::LoadModel(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature)
{
	for (auto iter = m_ModelInfoMap.begin(); iter != m_ModelInfoMap.end(); ++iter)
	{
		CLoadedModelInfo* pModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, const_cast<char*>((*iter).second.m_Filename.c_str()), nullptr, (*iter).second.m_HasAnimation);
		m_ModelMap.emplace((*iter).first, pModel);
	}
}

void CResourceManager::LoadMapObjectInfo(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	int totalRound = 3;
	for (int i = 0; i < totalRound; ++i)
	{
		multimap<string, MapObjectInfo*> objectInfo;

		string filename;
		filename = "../Resource/MapObjectInfo/Position/Round" + to_string(i) + ".bin";

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

			//m_MapObjectInfoMultiMap.emplace(pMapObjectInfo->m_Name, pMapObjectInfo);
			objectInfo.emplace(pMapObjectInfo->m_Name, pMapObjectInfo);
		}
		in.close();

		m_RoundInfoMapObjectInfo.emplace(i, objectInfo);
	}
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

void CResourceManager::LoadCharactersInfo(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	string filename = "../Resource/MapObjectInfo/Position/Characters.bin";

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
	m_CharactersInfoMap.reserve(nObjects);

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

		m_CharactersInfoMap.emplace_back(pMapObjectInfo);
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

	for (auto iter = m_TextureInfoMap.begin(); iter != m_TextureInfoMap.end(); )
	{
		iter = m_TextureInfoMap.erase(iter);
	}
	m_TextureInfoMap.clear();
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
