#include "../../../Stdafx/Stdafx.h"
#include "StandardObjectsShader.h"
#include "../../../GameObject/Surrounding/Surrounding.h"
#include "../../../GameObject/Terrain/Terrain.h"
#include "../../../ResourceManager/ResourceManager.h"

CStandardObjectsShader::CStandardObjectsShader()
{
}

CStandardObjectsShader::~CStandardObjectsShader()
{
	if (m_ppDeadTreeModel)
		delete[] m_ppDeadTreeModel;
	if (m_ppPineTreeModel)
		delete[] m_ppPineTreeModel;
	if (m_ppRockModel)
		delete[] m_ppRockModel;
}

void CStandardObjectsShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, 
	const map<string, Bounds*>& Context, void* pContext)
{
	int nDeadTrees = 5, nPineTrees = 8, nRocks = 5, nDeers = 1;
	
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
	m_ppRockModel = new CLoadedModelInfo*[nRocks];
	CLoadedModelInfo* pRock01 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_BigPlainRock_Snow_01.bin", this, false);
	m_ppRockModel[i++] = pRock01;
	CLoadedModelInfo* pRock02 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_BigPlainRock_Snow_02.bin", this, false);
	m_ppRockModel[i++] = pRock02;
	CLoadedModelInfo* pRock03 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_BigPlainRock_Snow_03.bin", this, false);
	m_ppRockModel[i++] = pRock03;
	CLoadedModelInfo* pRock04 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_BigPlainRock_Snow_04.bin", this, false);
	m_ppRockModel[i++] = pRock04;
	CLoadedModelInfo* pRock05 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_PlainSmall_Snow_01.bin", this, false);
	m_ppRockModel[i++] = pRock05;

	CLoadedModelInfo* pDeer01 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_Deer.bin", this, false);

	CTerrain* pTerrain = (CTerrain*)pContext;


	enum OBJECT_TYPE { DeadTree, PineTree, Rock, Deer };
	
	m_nObjects = nDeadTrees + nPineTrees + nRocks + nDeers;
	m_ppObjects = new CGameObject*[m_nObjects];

	m_ppObjects[0] = new CSurrounding(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppObjects[0]->SetChild(pDeadTreesModel01->m_pModelRootObject);
	pDeadTreesModel01->m_pModelRootObject->AddRef();

	m_ppObjects[1] = new CSurrounding(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppObjects[1]->SetChild(pDeadTreesModel02->m_pModelRootObject);
	pDeadTreesModel02->m_pModelRootObject->AddRef();

	m_ppObjects[2] = new CSurrounding(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppObjects[2]->SetChild(pDeadTreesModel03->m_pModelRootObject);
	pDeadTreesModel03->m_pModelRootObject->AddRef();

	m_ppObjects[3] = new CSurrounding(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppObjects[3]->SetChild(pDeadTreesModel04->m_pModelRootObject);
	pDeadTreesModel04->m_pModelRootObject->AddRef();

	m_ppObjects[4] = new CSurrounding(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppObjects[4]->SetChild(pDeadTreesModel05->m_pModelRootObject);
	pDeadTreesModel05->m_pModelRootObject->AddRef();

	m_ppObjects[5] = new CSurrounding(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppObjects[5]->SetChild(pPineTreesModel01->m_pModelRootObject);
	pPineTreesModel01->m_pModelRootObject->AddRef();

	m_ppObjects[6] = new CSurrounding(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppObjects[6]->SetChild(pPineTreesModel02->m_pModelRootObject);
	pPineTreesModel02->m_pModelRootObject->AddRef();

	m_ppObjects[7] = new CSurrounding(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppObjects[7]->SetChild(pPineTreesModel03->m_pModelRootObject);
	pPineTreesModel03->m_pModelRootObject->AddRef();

	m_ppObjects[8] = new CSurrounding(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppObjects[8]->SetChild(pPineTreesModel04->m_pModelRootObject);
	pPineTreesModel04->m_pModelRootObject->AddRef();

	m_ppObjects[9] = new CSurrounding(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppObjects[9]->SetChild(pPineTreesModel05->m_pModelRootObject);
	pPineTreesModel05->m_pModelRootObject->AddRef();

	m_ppObjects[10] = new CSurrounding(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppObjects[10]->SetChild(pPineTreesModel06->m_pModelRootObject);
	pPineTreesModel06->m_pModelRootObject->AddRef();

	m_ppObjects[11] = new CSurrounding(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppObjects[11]->SetChild(pPineTreesModel07->m_pModelRootObject);
	pPineTreesModel06->m_pModelRootObject->AddRef();

	m_ppObjects[12] = new CSurrounding(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppObjects[12]->SetChild(pPineTreesModel08->m_pModelRootObject);
	pPineTreesModel07->m_pModelRootObject->AddRef();

	m_ppObjects[13] = new CSurrounding(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppObjects[13]->SetChild(pRocks01->m_pModelRootObject);
	pRocks01->m_pModelRootObject->AddRef();

	m_ppObjects[14] = new CSurrounding(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppObjects[14]->SetChild(pRocks02->m_pModelRootObject);
	pRocks02->m_pModelRootObject->AddRef();

	m_ppObjects[15] = new CSurrounding(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppObjects[15]->SetChild(pRocks03->m_pModelRootObject);
	pRocks03->m_pModelRootObject->AddRef();

	m_ppObjects[16] = new CSurrounding(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppObjects[16]->SetChild(pRocks04->m_pModelRootObject);
	pRocks04->m_pModelRootObject->AddRef();

	m_ppObjects[17] = new CSurrounding(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppObjects[17]->SetChild(pRocks05->m_pModelRootObject);
	pRocks05->m_pModelRootObject->AddRef();

	m_ppObjects[18] = new CSurrounding(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppObjects[18]->SetChild(pDeer01->m_pModelRootObject);
	pDeer01->m_pModelRootObject->AddRef();


	for (int i = 0; i < m_nObjects; ++i)
	{
		m_ppObjects[i] = new CSurrounding(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
		Position.x = Random(10.f, 490.f);
		Position.z = Random(10.f, 290.f);
		m_ppObjects[i]->SetPosition(Position.x, pTerrain->GetHeight(Position.x, Position.z), Position.z);
		
		int random = Random(0, 3);
		switch (random)
		{
		case DeadTree:
			random = Random(0, nDeadTrees - 1);
			m_ppObjects[i]->SetChild(m_ppDeadTreeModel[random]->m_pModelRootObject, true);
			m_ppObjects[i]->setID("<DeadTree0" + to_string(random) + ">");
			break;
		case PineTree:
			random = Random(0, nPineTrees - 1);
			m_ppObjects[i]->SetChild(m_ppPineTreeModel[random]->m_pModelRootObject, true);
			m_ppObjects[i]->setID("<PineTree0" + to_string(random) + ">");
			break;
		case Rock:
			random = Random(0, nRocks - 1);
			m_ppObjects[i]->SetChild(m_ppRockModel[random]->m_pModelRootObject, true);
			m_ppObjects[i]->setID("<Rock0" + to_string(random) + ">");
			break;
		case Deer:
			m_ppObjects[i]->SetChild(pDeer01->m_pModelRootObject, true);
			m_ppObjects[i]->setID("<Deer01>");
			break;
		}
		auto iter = Context.find(m_ppObjects[i]->getID());
		if (iter != Context.end())
			m_ppObjects[i]->SetOOBB((*iter).second->m_xmf3Center, (*iter).second->m_xmf3Extent, XMFLOAT4(0, 0, 0, 1));
	}

	
	if (pDeadTreeModel01) delete pDeadTreeModel01;
	if (pDeadTreeModel02) delete pDeadTreeModel02;
	if (pDeadTreeModel03) delete pDeadTreeModel03;
	if (pDeadTreeModel04) delete pDeadTreeModel04;
	if (pDeadTreeModel05) delete pDeadTreeModel05;

	if (pPineTreeModel01) delete pPineTreeModel01;
	if (pPineTreeModel02) delete pPineTreeModel02;
	if (pPineTreeModel03) delete pPineTreeModel03;
	if (pPineTreeModel04) delete pPineTreeModel04;
	if (pPineTreeModel05) delete pPineTreeModel05;
	if (pPineTreeModel06) delete pPineTreeModel06;
	if (pPineTreeModel07) delete pPineTreeModel07;
	if (pPineTreeModel08) delete pPineTreeModel08;

	if (pRock01) delete pRock01;
	if (pRock02) delete pRock02;
	if (pRock03) delete pRock03;
	if (pRock04) delete pRock04;
	if (pRock05) delete pRock05;

	if (pDeer01) delete pDeer01;
}

void CStandardObjectsShader::AnimateObjects(float fTimeElapsed, CCamera* pCamera, CPlayer* pPlayer)
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

void CStandardObjectsShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
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