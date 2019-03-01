#include "../../../Stdafx/Stdafx.h"
#include "MapObjectShader.h"
#include "../../../GameObject/Surrounding/Surrounding.h"
#include "../../../GameObject/Terrain/Terrain.h"
#include "../../../ResourceManager/ResourceManager.h"

CMapObjectsShader::CMapObjectsShader()
{

}

CMapObjectsShader::~CMapObjectsShader()
{
	if (m_ppDeadTreeModel)
		delete[] m_ppDeadTreeModel;
	if (m_ppPineTreeModel)
		delete[] m_ppPineTreeModel;
	if (m_ppBigRockModel)
		delete[] m_ppBigRockModel;
	if (m_ppDeerModel)
		delete[] m_ppDeerModel;
	if (m_ppPondModel)
		delete[] m_ppPondModel;
	if (m_ppFenceModel)
		delete[] m_ppFenceModel;
	
}

void CMapObjectsShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature,
	const map<string, Bounds*>& Context, void* pContext)
{
	int nDeadTrees = 5, nPineTrees = 8, nBigRocks = 4, nDeers = 1 ,nPond =1,nFence =2;

	int i = 0;
	m_ppDeadTreeModel = new CLoadedModelInfo*[nDeadTrees];
	CLoadedModelInfo* pDeadTreeModel01 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_DeadTrunk_01.bin", this, false);
	m_ppDeadTreeModel[i++] = pDeadTreeModel01;
	CLoadedModelInfo* pDeadTreeModel02 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_DeadTrunk_02.bin", this, false);
	m_ppDeadTreeModel[i++] = pDeadTreeModel02;
	CLoadedModelInfo* pDeadTreeModel03 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_DeadTrunk_03.bin", this, false);
	m_ppDeadTreeModel[i++] = pDeadTreeModel03;
	CLoadedModelInfo* pDeadTreeModel04 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_DeadTrunk_04.bin", this, false);
	m_ppDeadTreeModel[i++] = pDeadTreeModel04;
	CLoadedModelInfo* pDeadTreeModel05 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_DeadTrunk_05.bin", this, false);
	m_ppDeadTreeModel[i++] = pDeadTreeModel05;

	i = 0;
	m_ppPineTreeModel = new CLoadedModelInfo*[nPineTrees];
	CLoadedModelInfo* pPineTreeModel01 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_PineTree_Snow_01.bin", this, false);
	m_ppPineTreeModel[i++] = pPineTreeModel01;
	CLoadedModelInfo* pPineTreeModel02 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_PineTree_Snow_02.bin", this, false);
	m_ppPineTreeModel[i++] = pPineTreeModel02;
	CLoadedModelInfo* pPineTreeModel03 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_PineTree_Snow_03.bin", this, false);
	m_ppPineTreeModel[i++] = pPineTreeModel03;
	CLoadedModelInfo* pPineTreeModel04 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_PineTree_Snow_04.bin", this, false);
	m_ppPineTreeModel[i++] = pPineTreeModel04;
	CLoadedModelInfo* pPineTreeModel05 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_PineTree_Snow_05.bin", this, false);
	m_ppPineTreeModel[i++] = pPineTreeModel05;
	CLoadedModelInfo* pPineTreeModel06 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_PineTree_Snow_06.bin", this, false);
	m_ppPineTreeModel[i++] = pPineTreeModel06;
	CLoadedModelInfo* pPineTreeModel07 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_PineTree_Snow_07.bin", this, false);
	m_ppPineTreeModel[i++] = pPineTreeModel07;
	CLoadedModelInfo* pPineTreeModel08 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_PineTree_Snow_08.bin", this, false);
	m_ppPineTreeModel[i++] = pPineTreeModel08;

	i = 0;
	m_ppBigRockModel = new CLoadedModelInfo*[nBigRocks];
	CLoadedModelInfo* pRock01 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_BigPlainRock_Snow_01.bin", this, false);
	m_ppBigRockModel[i++] = pRock01;
	CLoadedModelInfo* pRock02 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_BigPlainRock_Snow_02.bin", this, false);
	m_ppBigRockModel[i++] = pRock02;
	CLoadedModelInfo* pRock03 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_BigPlainRock_Snow_03.bin", this, false);
	m_ppBigRockModel[i++] = pRock03;
	CLoadedModelInfo* pRock04 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_BigPlainRock_Snow_04.bin", this, false);
	m_ppBigRockModel[i++] = pRock04;
	/*CLoadedModelInfo* pRock05 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_BigPlainRock_Snow_04.bin", this, false);
	m_ppRockModel[i++] = pRock05;

*/
	i = 0;
	m_ppDeerModel = new CLoadedModelInfo*[nDeers];
	CLoadedModelInfo* pDeer = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_Deer.bin", this, false);
	m_ppDeerModel[i++] = pDeer;

	i = 0;
	m_ppPondModel = new CLoadedModelInfo*[nPond];
	CLoadedModelInfo* pPond01 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/PondSquare.bin", this, false);
	m_ppPondModel[i++] = pPond01;

	i = 0;
	m_ppFenceModel = new CLoadedModelInfo*[nFence];
	CLoadedModelInfo* pFence01 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/LowPoly_Fence_01.bin", this, false);
	m_ppFenceModel[i++] = pFence01;
	CLoadedModelInfo* pFence02 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/LowPoly_Fence_02.bin", this, false);
	m_ppFenceModel[i++] = pFence02;



	//모델 맵에 저장
	m_ModelsList.emplace("SM_DeadTrunk_01", pDeadTreeModel01);
	m_ModelsList.emplace("SM_DeadTrunk_02", pDeadTreeModel02);
	m_ModelsList.emplace("SM_DeadTrunk_03", pDeadTreeModel03);
	m_ModelsList.emplace("SM_DeadTrunk_04", pDeadTreeModel04);
	m_ModelsList.emplace("SM_DeadTrunk_05", pDeadTreeModel05);

	m_ModelsList.emplace("SM_PineTree_Snow_01", pPineTreeModel01);
	m_ModelsList.emplace("SM_PineTree_Snow_02", pPineTreeModel02);
	m_ModelsList.emplace("SM_PineTree_Snow_03", pPineTreeModel03);
	m_ModelsList.emplace("SM_PineTree_Snow_04", pPineTreeModel04);
	m_ModelsList.emplace("SM_PineTree_Snow_05", pPineTreeModel05);
	m_ModelsList.emplace("SM_PineTree_Snow_06", pPineTreeModel06);
	m_ModelsList.emplace("SM_PineTree_Snow_07", pPineTreeModel07);
	m_ModelsList.emplace("SM_PineTree_Snow_08", pPineTreeModel08);

	m_ModelsList.emplace("SM_BigPlainRock_Snow_01", pRock01);
	m_ModelsList.emplace("SM_BigPlainRock_Snow_02", pRock02);
	m_ModelsList.emplace("SM_BigPlainRock_Snow_03", pRock03);
	m_ModelsList.emplace("SM_BigPlainRock_Snow_04", pRock04);

	m_ModelsList.emplace("SM_Deer", pDeer);

	m_ModelsList.emplace("PondSquare", pPond01);
	m_ModelsList.emplace("LowPoly_-_Fence_A", pFence01);
	m_ModelsList.emplace("LowPoly_-_Fence_B", pFence02);



	CTerrain* pTerrain = (CTerrain*)pContext;

	string filename = "MapVer1";

	ifstream inf(filename + ".bin", ios::binary);

	if (!inf)
	{
		cout << filename + ".bin" + string("바이너리 파일 없음\n");
		return;
	}

	cout << filename + string("로딩 성공\n");

	size_t nReads = 0;

	
	inf.read(reinterpret_cast<char*>(&m_nObjects), sizeof(int));

	m_ppObjects = new CGameObject*[m_nObjects];


	for(int i=0;i<m_nObjects;i++)	
	{
		m_ppObjects[i] = new CSurrounding(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
		inf.read(reinterpret_cast<char*>(&nReads), sizeof(size_t));
		char* p = new char[nReads + 1];

		

		inf.read(p, sizeof(char)*nReads);
		p[nReads] = '\0';
		cout << p << "\n";

		auto iter = m_ModelsList.find(string(p));
		if (iter != m_ModelsList.end())
		{
			CLoadedModelInfo* pSurroundingModel = (*iter).second;
			if (!strcmp(p, pSurroundingModel->m_pModelRootObject->m_pstrFrameName))
			{
				m_ppObjects[i]->SetChild(pSurroundingModel->m_pModelRootObject, true);
				delete[] p;
				p = nullptr;
				

				inf.read(reinterpret_cast<char*>(&nReads), sizeof(size_t));
				p = new char[nReads + 1];


				inf.read(p, sizeof(char)*nReads);
				p[nReads] = '\0';
				cout << p << "\n";

				if (!strcmp(p, "<Position>"))
				{
					float x, y, z = 0.0f;
					inf.read(reinterpret_cast<char*>(&x), sizeof(float));
					inf.read(reinterpret_cast<char*>(&y), sizeof(float));
					inf.read(reinterpret_cast<char*>(&z), sizeof(float));
					//printf("%f",x);
					m_ppObjects[i]->SetPosition(x, y, z);
				
				}
				delete[] p;
				p = nullptr;

				inf.read(reinterpret_cast<char*>(&nReads), sizeof(size_t));
				p = new char[nReads + 1];


				inf.read(p, sizeof(char)*nReads);
				p[nReads] = '\0';
				cout << p << "\n";
				if (!strcmp(p, "<Look>"))
				{
					float x, y, z = 0.0f;
					inf.read(reinterpret_cast<char*>(&x), sizeof(float));
					inf.read(reinterpret_cast<char*>(&y), sizeof(float));
					inf.read(reinterpret_cast<char*>(&z), sizeof(float));
					m_ppObjects[i]->SetLookVector(XMFLOAT3(x,y,z));
				}
				delete[] p;
				p = nullptr;

				inf.read(reinterpret_cast<char*>(&nReads), sizeof(size_t));
				p = new char[nReads + 1];


				inf.read(p, sizeof(char)*nReads);
				p[nReads] = '\0';
				cout << p << "\n";

				if (!strcmp(p, "<Up>"))
				{
					float x, y, z = 0.0f;
					inf.read(reinterpret_cast<char*>(&x), sizeof(float));
					inf.read(reinterpret_cast<char*>(&y), sizeof(float));
					inf.read(reinterpret_cast<char*>(&z), sizeof(float));
					m_ppObjects[i]->SetUpVector(XMFLOAT3(x, y, z));
				}
				delete[] p;
				p = nullptr;

				inf.read(reinterpret_cast<char*>(&nReads), sizeof(size_t));
				p = new char[nReads + 1];


				inf.read(p, sizeof(char)*nReads);
				p[nReads] = '\0';
				cout << p << "\n";

				if (!strcmp(p, "<Right>"))
				{
					float x, y, z = 0.0f;
					inf.read(reinterpret_cast<char*>(&x), sizeof(float));
					inf.read(reinterpret_cast<char*>(&y), sizeof(float));
					inf.read(reinterpret_cast<char*>(&z), sizeof(float));
					m_ppObjects[i]->SetRightVector(XMFLOAT3(x, y, z));
				}
				delete[] p;
				p = nullptr;
			}


		}
		//if(!strcmp())
		/*if (!strcmp(p, "<Position>"))
		{
			
		}*/
		if (p)
		{
			delete[] p;
		}
		
	}

	inf.close();
}


void CMapObjectsShader::AnimateObjects(float fTimeElapsed, CCamera* pCamera, CPlayer* pPlayer)
{
	m_fElapsedTime += fTimeElapsed;

	for (int i = 0; i < m_nObjects; ++i)
	{
		if (m_ppObjects[i])
		{
			m_ppObjects[i]->Animate(m_fElapsedTime);
		}
	}
}

void CMapObjectsShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CShader::Render(pd3dCommandList, pCamera);
	for (int i = 0; i < m_nObjects; ++i)
	{
		if (m_ppObjects[i])
		{
			//m_ppObjects[i]->Animate(m_fElapsedTime);
			m_ppObjects[i]->UpdateTransform(nullptr);
			m_ppObjects[i]->Render(pd3dCommandList, pCamera);
		}
	}
}