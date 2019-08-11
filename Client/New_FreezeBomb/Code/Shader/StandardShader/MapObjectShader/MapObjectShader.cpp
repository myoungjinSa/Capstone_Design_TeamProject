#include "../../../Stdafx/Stdafx.h"
#include "MapObjectShader.h"
#include "../../../GameObject/Surrounding/Surrounding.h"
#include "../../../GameObject/Terrain/Terrain.h"
#include "../../../FrameTransform/FrameTransform.h"


extern unsigned char g_Round;

CMapObjectsShader::SPAWN_INFO CMapObjectsShader::spawn[3][6] = {
	{{{0.0f,0.0f,0.0f}},{{0.0f,0.0f,0.0f}},{{0.0f,0.0f,0.0f}},{{0.0f,0.0f,0.0f}},{{0.0f,0.0f,0.0f}},{{0.0f,0.0f,0.0f}}},
	{{{0.0f,0.0f,0.0f}},{{0.0f,0.0f,0.0f}},{{0.0f,0.0f,0.0f}},{{0.0f,0.0f,0.0f}},{{0.0f,0.0f,0.0f}},{{0.0f,0.0f,0.0f}}},
	{{{0.0f,0.0f,0.0f}},{{0.0f,0.0f,0.0f}},{{0.0f,0.0f,0.0f}},{{0.0f,0.0f,0.0f}},{{0.0f,0.0f,0.0f}},{{0.0f,0.0f,0.0f}}}
};

CMapObjectsShader::CMapObjectsShader()
{
}

CMapObjectsShader::~CMapObjectsShader()
{
}

void CMapObjectsShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature,
	const map<string, CLoadedModelInfo*>& ModelMap, const unordered_map<unsigned char, RoundInfo>& RoundMapObjectInfo, const map<string, Bounds*>& BoundMap, void* pContext)
{
	unsigned char round = 0;
	// 모든 라운드에 해당하는 모델정보 루프를 돌면서
	for (auto iter = RoundMapObjectInfo.begin(); iter != RoundMapObjectInfo.end(); ++iter)
	{
		list<CGameObject*> objectList;
		int index = 0;
		// 모든 모델의 루프를 돌고,
		for (auto iter2 = ModelMap.begin(); iter2 != ModelMap.end(); ++iter2)
		{
			string name = (*iter2).first;
			if (name == "Hammer" || name == "GoldTimer")				continue;

			// multimap 컨테이너에서 같은 키를 갖는 벨류를 찾을 때, 사용하는 루프
			for (auto iter3 = (*iter).second.lower_bound(name); iter3 != (*iter).second.upper_bound(name); ++iter3)
			{
				CSurrounding* pSurrounding = new CSurrounding(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
				pSurrounding->m_pFrameTransform = new CFrameTransform(pd3dDevice, pd3dCommandList, (*iter2).second);

				pSurrounding->SetChild((*iter2).second->m_pModelRootObject, true);
				if (name == "SM_FirePit") {
					(*iter3).second->m_Position.y = 0.001f;
					//크기 조절
					(*iter2).second->m_pModelRootObject->m_xmf3Scale = Vector3::Multiply((*iter2).second->m_pModelRootObject->m_xmf3Scale, XMFLOAT3(0.95f, 0.95f, 0.95f));
				}
				pSurrounding->SetPosition((*iter3).second->m_Position);
				pSurrounding->SetLookVector((*iter3).second->m_Look);
				pSurrounding->SetUpVector((*iter3).second->m_Up);
				pSurrounding->SetRightVector((*iter3).second->m_Right);
				
				if (name == "LowPolyFence_01")
				{
					XMFLOAT3 rotate = XMFLOAT3(0, 1, 0);
					float angle = 180.f;
					pSurrounding->Rotate(&rotate, angle);
				}


				dynamic_cast<CSurrounding*>(pSurrounding)->SetIndex(index++);
				pSurrounding->Initialize_Shadow((*iter2).second, pSurrounding);
				
				auto iter4 = BoundMap.find(name);
				if (iter4 != BoundMap.end())
				{
					if (name == "SM_Deer")
						pSurrounding->SetOOBB(Vector3::Add((*iter4).second->m_xmf3Center, XMFLOAT3(0.0f, 0.0f, -3.5f)), Vector3::Multiply((*iter4).second->m_xmf3Extent, XMFLOAT3(1.0f, 1.0f, 3.5f)), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
					else
						pSurrounding->SetOOBB((*iter4).second->m_xmf3Center, Vector3::Multiply((*iter4).second->m_xmf3Extent, (*iter2).second->m_pModelRootObject->m_xmf3Scale), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
				}
				objectList.emplace_back(pSurrounding);
			}
		}

		
		m_MapObjectList.emplace(round++, objectList);
	}

	const int MAX_ROUND = 3;
	for (int i = 0; i < MAX_ROUND; ++i)
	{
		string filename = "../Resource/MapObjectInfo/SpawnPosition/Round" + to_string(i);
		filename +="Spawn.bin";
		LoadSpawnInfo(filename, i);
	}
}

void CMapObjectsShader::AnimateObjects(float fTimeElapsed, CCamera* pCamera, CPlayer* pPlayer)
{
	m_fElapsedTime += fTimeElapsed;
}

void CMapObjectsShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera, int nPipelineState)
{
	auto iter = m_MapObjectList.find(g_Round);
	if (iter != m_MapObjectList.end())
	{
		for (auto iter2 = (*iter).second.begin(); iter2 != (*iter).second.end(); ++iter2)
		{
			(*iter2)->Animate(m_fElapsedTime);
			(*iter2)->UpdateTransform(nullptr);
			(*iter2)->Render(pd3dCommandList, pCamera, nPipelineState);
		}
	}
}

void CMapObjectsShader::ReleaseObjects()
{
	for (auto iter = m_MapObjectList.begin(); iter != m_MapObjectList.end(); )
	{
		for (auto iter2 = (*iter).second.begin(); iter2 != (*iter).second.end(); )
		{
		
			(*iter2)->Release();

			iter2 = (*iter).second.erase(iter2);
			
		}
		(*iter).second.clear();
		iter = m_MapObjectList.erase(iter);
	}
	m_MapObjectList.clear();

}

void CMapObjectsShader::ReleaseUploadBuffers()
{
	for (auto iter = m_MapObjectList.begin(); iter != m_MapObjectList.end(); ++iter)
	{
		for (auto iter2 = (*iter).second.begin(); iter2 != (*iter).second.end(); ++iter2)
		{
			(*iter2)->ReleaseUploadBuffers();
		}
	}
}

D3D12_RASTERIZER_DESC CMapObjectsShader::CreateRasterizerState()
{
	D3D12_RASTERIZER_DESC d3dRasterizerDesc;
	::ZeroMemory(&d3dRasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));
	d3dRasterizerDesc.FillMode = D3D12_FILL_MODE_WIREFRAME;
	d3dRasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	d3dRasterizerDesc.FrontCounterClockwise = FALSE;
	d3dRasterizerDesc.DepthBias = 0;
	d3dRasterizerDesc.DepthBiasClamp = 0.0f;
	d3dRasterizerDesc.SlopeScaledDepthBias = 0.0f;
	d3dRasterizerDesc.DepthClipEnable = TRUE;
	d3dRasterizerDesc.MultisampleEnable = FALSE;
	d3dRasterizerDesc.AntialiasedLineEnable = FALSE;
	d3dRasterizerDesc.ForcedSampleCount = 0;
	d3dRasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	return(d3dRasterizerDesc);
}

const list<CGameObject*>& CMapObjectsShader::getMapObjectList()	const
{
	auto iter = m_MapObjectList.find(g_Round);
	//cout << "라운드: " << (int)g_Round << "\n";
	if (iter != m_MapObjectList.end())
		return (*iter).second;
}


void CMapObjectsShader::LoadSpawnInfo(const string& filename,const int& round)
{
	ifstream in(filename,ios::binary);

	if(!in)
	{
		cout << "존재하지 않는 파일입니다." << filename << endl;
		cout << "Player Spawn 정보 로딩 실패"<<endl;
		return;
	}

	int playerCount = 0;
	in.read(reinterpret_cast<char*>(&playerCount), sizeof(int));

	for(int i=0;i<playerCount;++i)
	{
		in.read(reinterpret_cast<char*>(&spawn[round][i].pos.x), sizeof(float));
		in.read(reinterpret_cast<char*>(&spawn[round][i].pos.y), sizeof(float));
		in.read(reinterpret_cast<char*>(&spawn[round][i].pos.z), sizeof(float));
	}
}