#include "../../../Stdafx/Stdafx.h"
#include "MapToolShader.h"
#include "../../../GameObject/Surrounding/Surrounding.h"
#include "../../../GameObject/Item/Item.h"
#include "../../../GameObject/EvilBear/EvilBear.h"
#include "../../../GameObject/Terrain/Terrain.h"
#include "../../../GameObject/Player/Player.h"
#include "../../../FrameTransform/FrameTransform.h"

bool operator<(pair<string, CGameObject*> &p1, pair< string, CGameObject> &p2)
{
	return p1.first < p2.first;
}

//////////////////////////////////////////////////////////////////////////////////////////

CMapToolShader::CMapToolShader()	:m_nObjects(0)
{
}

CMapToolShader::~CMapToolShader()
{
}

void CMapToolShader::InstallMapObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CPlayer* pPlayer, char MapObjectType)
{
	switch (MapObjectType)
	{
		// 1
	case DeadTree:		
		m_nCurrDeadTreeModelIndex = (m_nCurrDeadTreeModelIndex) % (m_nDeadTreeModelCount) + 1;
		InsertObject(pd3dDevice, pd3dCommandList, pPlayer, "SM_DeadTrunk_0" + to_string(m_nCurrDeadTreeModelIndex));
		break;
		// 2
	case PineTree:
		m_nCurrPineTreeModelIndex = (m_nCurrPineTreeModelIndex) % (m_nPineTreeModelCount) + 1;
		InsertObject(pd3dDevice, pd3dCommandList, pPlayer, "SM_PineTree_Snow_0" + to_string(m_nCurrPineTreeModelIndex));
		break;
		// 3
	case BigRock:
		m_nCurrBigRockModelIndex = (m_nCurrBigRockModelIndex) % (m_nBigRockModelCount) + 1;
		InsertObject(pd3dDevice, pd3dCommandList, pPlayer, "SM_BigPlainRock_Snow_0" + to_string(m_nCurrBigRockModelIndex));
		break;
		// 4
	case Deer:
		InsertObject(pd3dDevice, pd3dCommandList, pPlayer, "SM_Deer");
		break;
		// 5
	case Fence:
		InsertFence(pd3dDevice, pd3dCommandList);
		break;
		// 6
	case FirePit:
		InsertIFirePit(pd3dDevice, pd3dCommandList, pPlayer, "SM_FirePit");
		break;
		// 7
	case Foliage0:
		InsertRandomFoliage(pd3dDevice, pd3dCommandList, pPlayer, "Foliage0");
		//InsertObject(pd3dDevice, pd3dCommandList, pPlayer, "Foliage0");
		break;
		// 8
	case Foliage1:
		InsertRandomFoliage(pd3dDevice, pd3dCommandList, pPlayer, "Foliage1");
		//InsertObject(pd3dDevice, pd3dCommandList, pPlayer, "Foliage1");
		break;
		// 9
	case Foliage2:
		InsertRandomFoliage(pd3dDevice, pd3dCommandList, pPlayer, "Foliage2");
		//InsertObject(pd3dDevice, pd3dCommandList, pPlayer, "Foliage2");
		break;

		// H
	case Hammer:
		InsertObject(pd3dDevice, pd3dCommandList, pPlayer, "Hammer");
		break;

		// T
	case GoldTimer:
		InsertObject(pd3dDevice, pd3dCommandList, pPlayer, "GoldTimer");
		break;

		// E
	case EvilBear:
		InsertCharacters(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pPlayer, "EvilBear");
		break;

		// D
	case DeleteAllObject:
		DeleteAll();
		break;
		// R
	case DeleteObject:
		Delete();
		break;

		// O
	case OutputFile:
		MakeMapFile();
		break;
	
	default:
		break;
	}
}

void CMapToolShader::InsertObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CPlayer* pPlayer, const string& model)
{
	auto iter = m_ModelsList.find(model);

	if (iter != m_ModelsList.end())
	{
		if (pPlayer == nullptr)
			return;

		CSurrounding* pGameObject = new CSurrounding(pd3dDevice, pd3dCommandList, nullptr);
		pGameObject->SetChild((*iter).second->m_pModelRootObject, true);
		pGameObject->m_pFrameTransform = new CFrameTransform(pd3dDevice, pd3dCommandList, (*iter).second);

		XMFLOAT3 position = pPlayer->GetPosition();
		XMFLOAT3 lookVec = pPlayer->GetLookVector();
		XMFLOAT3 upVec = pPlayer->GetUpVector();
		XMFLOAT3 rightVec = pPlayer->GetRightVector();

		//Object 위치 설정
		pGameObject->SetPosition(position);
		//Object 방향 설정
		pGameObject->SetLookVector(lookVec);
		pGameObject->SetUpVector(upVec);
		pGameObject->SetRightVector(rightVec);

		m_Objects.emplace_back(make_pair((*iter).first, pGameObject));
	}
}

void CMapToolShader::InsertIFirePit(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, CPlayer *pPlayer, const string& model)
{
	auto iter = m_ModelsList.find(model);
	if (iter != m_ModelsList.end())
	{
		if (pPlayer == nullptr)
			return;

		CSurrounding* pGameObject = new CSurrounding(pd3dDevice, pd3dCommandList, nullptr);
		pGameObject->SetChild((*iter).second->m_pModelRootObject, true);
		pGameObject->m_pFrameTransform = new CFrameTransform(pd3dDevice, pd3dCommandList, (*iter).second);

		XMFLOAT3 position = XMFLOAT3(250.f, 0.f, 150.f);
		XMFLOAT3 lookVec = pPlayer->GetLookVector();
		XMFLOAT3 upVec = pPlayer->GetUpVector();
		XMFLOAT3 rightVec = pPlayer->GetRightVector();

		//Object 위치 설정
		pGameObject->SetPosition(position);
		//Object 방향 설정
		pGameObject->SetLookVector(lookVec);
		pGameObject->SetUpVector(upVec);
		pGameObject->SetRightVector(rightVec);

		m_Objects.emplace_back(make_pair((*iter).first, pGameObject));
	}
}

void CMapToolShader::InsertFence(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	string s = "LowPolyFence_01";
	auto iter = m_ModelsList.find(s);
	if (iter != m_ModelsList.end())
	{
		int size = 11;
		for (int i = 0; i < 2; ++i)
		{
			for (int j = 1; j < 46; ++j)
			{
				CSurrounding* pGameObject = new CSurrounding(pd3dDevice, pd3dCommandList, nullptr);
				pGameObject->SetChild((*iter).second->m_pModelRootObject, true);
				pGameObject->m_pFrameTransform = new CFrameTransform(pd3dDevice, pd3dCommandList, (*iter).second);

				if (i == 0)
					pGameObject->SetPosition(j * size, 0, 2);
				else
					pGameObject->SetPosition(j * size, 0, 305);

				m_Objects.emplace_back(make_pair("LowPolyFence_01", pGameObject));

			}

			for (int i = 0; i < 2; ++i)
			{
				for (int j = 1; j < 27; ++j)
				{
					CSurrounding* pGameObject = new CSurrounding(pd3dDevice, pd3dCommandList, nullptr);
					pGameObject->SetChild((*iter).second->m_pModelRootObject, true);
					pGameObject->m_pFrameTransform = new CFrameTransform(pd3dDevice, pd3dCommandList, (*iter).second);

					if (i == 0)
					{
						pGameObject->SetPosition(1, 0, j * size + 4);
						XMFLOAT3 rotate = XMFLOAT3(0, 1, 0);
						pGameObject->Rotate(&rotate, 90);
					}
					else
					{
						pGameObject->SetPosition(508, 0, j * size + 4);
						XMFLOAT3 rotate = XMFLOAT3(0, 1, 0);
						pGameObject->Rotate(&rotate, 90);
					}

					m_Objects.emplace_back(make_pair("LowPolyFence_01", pGameObject));
				}
			}
		}
	}
}

void CMapToolShader::InsertRandomFoliage(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, CPlayer *pPlayer, const string& model)
{
	random_device seed;
	default_random_engine dre(seed());
	uniform_int_distribution<> uid_x(10, 500);
	uniform_int_distribution<> uid_z(10, 300);

	auto iter = m_ModelsList.find(model);
	if (iter != m_ModelsList.end())
	{
		for (int i = 0; i < 100; ++i)
		{
			CSurrounding* pGameObject = new CSurrounding(pd3dDevice, pd3dCommandList, nullptr);
			pGameObject->SetChild((*iter).second->m_pModelRootObject, true);
			pGameObject->m_pFrameTransform = new CFrameTransform(pd3dDevice, pd3dCommandList, (*iter).second);

			pGameObject->SetPosition(uid_x(dre), 0, uid_z(dre));
			m_Objects.emplace_back(make_pair((*iter).first, pGameObject));
		}
	}

}

void CMapToolShader::InsertCharacters(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CPlayer *pPlayer, const string& model)
{
	auto iter = m_ModelsList.find(model);
	if (iter != m_ModelsList.end())
	{
		XMFLOAT3 position[6] =
		{
			XMFLOAT3(125.f, 0.f, 50.f),
			XMFLOAT3(250.f, 0.f, 50.f),
			XMFLOAT3(375.f, 0.f, 50.f),
			XMFLOAT3(125.f, 0.f, 240.f),
			XMFLOAT3(250.f, 0.f, 240.f),
			XMFLOAT3(375.f, 0.f, 240.f)
		};

		for (int i = 0; i < 6; ++i)
		{
			CEvilBear* pGameObject = new CEvilBear(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, 0);
			pGameObject->SetChild((*iter).second->m_pModelRootObject, true);
			pGameObject->m_pAnimationController = new CAnimationController(1, (*iter).second->m_pAnimationSets);
			// 0번 트랙에 0번 애니메이션을 Set
			pGameObject->m_pAnimationController->SetTrackAnimationSet(0, pGameObject->m_pAnimationController->IDLE);
			pGameObject->m_pSkinningBoneTransforms = new CSkinningBoneTransforms(pd3dDevice, pd3dCommandList, (*iter).second);
			pGameObject->m_pFrameTransform = new CFrameTransform(pd3dDevice, pd3dCommandList, (*iter).second);

			XMFLOAT3 scale = XMFLOAT3(10.0f, 10.0f, 10.0f);
			pGameObject->SetScale(scale.x, scale.y, scale.z);
			//Object 위치 설정
			pGameObject->SetPosition(position[i]);

			m_Objects.emplace_back(make_pair((*iter).first, pGameObject));
		}
	}
}

void CMapToolShader::SortDescByName()
{
	if (!m_Objects.empty()) 
	{
		sort(m_Objects.begin(), m_Objects.end());
	}
}

void CMapToolShader::MakeMapFile()
{
	OutputMapTextFile();
	OutputMapBinaryFile();
}

void CMapToolShader::OutputMapTextFile()
{
	string filePath;
	int index = 0;
	// 같은 파일 있는지 확인
	while(true)
	{
		filePath = "../Resource/MapObjectInfo/Position/Round" + to_string(index++) + ".txt";
		ifstream in(filePath);
		if (!in)
			break;
	}

	//파일에 출력하기전에 이름의 내림차순으로 정렬
	SortDescByName();		

	ofstream out(filePath);
	out << "Objects Count:" << m_Objects.size() << "\n";

	for (auto iter = m_Objects.begin(); iter != m_Objects.end(); iter++)
	{
		string str;

		str = "<" + iter->first + ">\n";			//write Objects Name
		out.write(str.c_str(), str.size());
		str = "<Position>: ";
		out.write(str.c_str(), str.size());
		out << iter->second->GetPosition().x << " " << iter->second->GetPosition().y << " " << iter->second->GetPosition().z << "\n";

		str = "<Look>: ";
		out.write(str.c_str(), str.size());
		out << iter->second->GetLook().x << " " << iter->second->GetLook().y << " " << iter->second->GetLook().z << "\n";

		str = "<Up>: ";
		out.write(str.c_str(), str.size());
		out << iter->second->GetUp().x << " " << iter->second->GetUp().y << " " << iter->second->GetUp().z << "\n";

		str = "<Right>: ";
		out.write(str.c_str(), str.size());
		out << iter->second->GetRight().x << " " << iter->second->GetRight().y << " " << iter->second->GetRight().z << "\n";
		out << "\n";

	}
	out.close();
}

void CMapToolShader::OutputMapBinaryFile()
{
	string filePath;
	int index = 0;
	// 같은 파일 있는지 확인
	while (true)
	{
		filePath = "../Resource/MapObjectInfo/Position/Round" + to_string(index++) + ".bin";
		ifstream in(filePath);
		if (!in)
			break;
	}

	//파일에 출력하기전에 이름의 내림차순으로 정렬
	SortDescByName();		

	ofstream out(filePath, ios::binary);

	int objCount = m_Objects.size();
	out.write(reinterpret_cast<const char*>(&objCount), sizeof(int));

	
	for (int i = 0 ; i < objCount; i++)
	{
		size_t stringLength = m_Objects[i].first.length();

		out.write(reinterpret_cast<const char*>(&stringLength), sizeof(size_t));
		out.write(m_Objects[i].first.c_str(), sizeof(char)*stringLength);

		string pos = "<Position>";
		stringLength = pos.length();
		out.write(reinterpret_cast<const char*>(&stringLength), sizeof(size_t));
		out.write(pos.c_str(), sizeof(char)*stringLength);

		float posX = m_Objects[i].second->GetPosition().x;
		float posY = m_Objects[i].second->GetPosition().y;
		float posZ = m_Objects[i].second->GetPosition().z;

		out.write(reinterpret_cast<const char*>(&posX), sizeof(float));
		out.write(reinterpret_cast<const char*>(&posY), sizeof(float));
		out.write(reinterpret_cast<const char*>(&posZ), sizeof(float));

		string look = "<Look>";
		stringLength = look.length();
		out.write(reinterpret_cast<const char*>(&stringLength), sizeof(size_t));
		out.write(look.c_str(), sizeof(char)*stringLength);

		float lookX = m_Objects[i].second->GetLook().x;
		float lookY = m_Objects[i].second->GetLook().y;
		float lookZ = m_Objects[i].second->GetLook().z;

		out.write(reinterpret_cast<const char*>(&lookX), sizeof(float));
		out.write(reinterpret_cast<const char*>(&lookY), sizeof(float));
		out.write(reinterpret_cast<const char*>(&lookZ), sizeof(float));

		string up = "<Up>";
		stringLength = up.length();
		out.write(reinterpret_cast<const char*>(&stringLength), sizeof(size_t));
		out.write(up.c_str(), sizeof(char)*stringLength);

		float upX = m_Objects[i].second->GetUp().x;
		float upY = m_Objects[i].second->GetUp().y;
		float upZ = m_Objects[i].second->GetUp().z;
		out.write(reinterpret_cast<const char*>(&upX), sizeof(float));
		out.write(reinterpret_cast<const char*>(&upY), sizeof(float));
		out.write(reinterpret_cast<const char*>(&upZ), sizeof(float));

		string right = "<Right>";
		stringLength = right.length();

		out.write(reinterpret_cast<const char*>(&stringLength), sizeof(size_t));
		out.write(right.c_str(), sizeof(char)*stringLength);

		float rightX = m_Objects[i].second->GetRight().x;
		float rightY = m_Objects[i].second->GetRight().y;
		float rightZ = m_Objects[i].second->GetRight().z;
		out.write(reinterpret_cast<const char*>(&rightX), sizeof(float));
		out.write(reinterpret_cast<const char*>(&rightY), sizeof(float));
		out.write(reinterpret_cast<const char*>(&rightZ), sizeof(float));
	}



	out.close();
}

void CMapToolShader::BuildObjects(const map<string, CLoadedModelInfo*>& ModelMap)
{
	m_ModelsList = ModelMap;

	m_nDeadTreeModelCount = 5;
	m_nPineTreeModelCount = 8;
	m_nBigRockModelCount = 4;
	m_nFenceModelCount = 2;	
}

void CMapToolShader::ReleaseObjects()
{
	for (auto iter = m_Objects.begin(); iter != m_Objects.end(); )
	{
		(*iter).second->Release();
		iter = m_Objects.erase(iter);
	}
	m_Objects.clear();

	for (auto iter = m_ModelsList.begin(); iter != m_ModelsList.end(); )
		iter = m_ModelsList.erase(iter);
	m_ModelsList.clear();

	for (auto iter = m_RemovedObjectList.begin(); iter != m_RemovedObjectList.end();)
	{
		(*iter)->Release();
		iter = m_RemovedObjectList.erase(iter);
	}
	m_RemovedObjectList.clear();
}

void CMapToolShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera, int nPipelineState)
{
	CShader::Render(pd3dCommandList, pCamera, nPipelineState);

	for (auto iter = m_Objects.begin(); iter != m_Objects.end(); iter++)
	{
		if (iter->second)
		{
			iter->second->Animate(m_fElapsedTime);
			iter->second->UpdateTransform(nullptr, false);
			iter->second->Render(pd3dCommandList, pCamera, nPipelineState);
		}
	}
}

void CMapToolShader::DeleteAll()
{
	for (auto iter = m_Objects.begin(); iter != m_Objects.end(); ++iter)
	{
		m_RemovedObjectList.emplace_back((*iter).second);
	}
	for (auto iter = m_Objects.begin(); iter != m_Objects.end();)
		iter = m_Objects.erase(iter);

	m_Objects.clear();

}

void CMapToolShader::Delete()
{
	if (m_Objects.size() > 0)
	{
		auto iter = m_Objects.end();
		--iter;
		m_RemovedObjectList.emplace_back((*iter).second);
		iter = m_Objects.erase(iter);
	}
}
