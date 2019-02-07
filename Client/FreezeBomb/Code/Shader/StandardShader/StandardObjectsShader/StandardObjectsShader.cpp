#include "../../../Stdafx/Stdafx.h"
#include "StandardObjectsShader.h"
#include "../../../GameObject/GameObject.h"
#include "../../../GameObject/Surrounding/Surrounding.h"
#include "../../../GameObject/Terrain/Terrain.h"

CStandardObjectsShader::CStandardObjectsShader()
{
}

CStandardObjectsShader::~CStandardObjectsShader()
{
}

void CStandardObjectsShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, void *pContext)
{
	m_nObjects = 19;
	m_ppObjects = new CGameObject*[m_nObjects];

	CLoadedModelInfo* pDeadTreesModel01 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Model/SM_DeadTrunk_01.bin", this, false);
	CLoadedModelInfo* pDeadTreesModel02 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Model/SM_DeadTrunk_02.bin", this, false);
	CLoadedModelInfo* pDeadTreesModel03 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Model/SM_DeadTrunk_03.bin", this, false);
	CLoadedModelInfo* pDeadTreesModel04 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Model/SM_DeadTrunk_04.bin", this, false);
	CLoadedModelInfo* pDeadTreesModel05 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Model/SM_DeadTrunk_05.bin", this, false);

	CLoadedModelInfo* pPineTreesModel01 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Model/SM_PineTree_Snow_01.bin", this, false);
	CLoadedModelInfo* pPineTreesModel02 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Model/SM_PineTree_Snow_02.bin", this, false);
	CLoadedModelInfo* pPineTreesModel03 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Model/SM_PineTree_Snow_03.bin", this, false);
	CLoadedModelInfo* pPineTreesModel04 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Model/SM_PineTree_Snow_04.bin", this, false);
	CLoadedModelInfo* pPineTreesModel05 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Model/SM_PineTree_Snow_05.bin", this, false);
	CLoadedModelInfo* pPineTreesModel06 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Model/SM_PineTree_Snow_06.bin", this, false);
	CLoadedModelInfo* pPineTreesModel07 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Model/SM_PineTree_Snow_07.bin", this, false);
	CLoadedModelInfo* pPineTreesModel08 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Model/SM_PineTree_Snow_08.bin", this, false);

	CLoadedModelInfo* pRocks01 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Model/SM_BigPlainRock_Snow_01.bin", this, false);
	CLoadedModelInfo* pRocks02 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Model/SM_BigPlainRock_Snow_02.bin", this, false);
	CLoadedModelInfo* pRocks03 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Model/SM_BigPlainRock_Snow_03.bin", this, false);
	CLoadedModelInfo* pRocks04 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Model/SM_BigPlainRock_Snow_04.bin", this, false);
	CLoadedModelInfo* pRocks05 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Model/SM_PlainSmall_Snow_01.bin", this, false);

	CLoadedModelInfo* pDeer01 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Model/SM_Deer.bin", this, false);

	CTerrain* pTerrain = (CTerrain*)pContext;

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

	XMFLOAT3 Position;
	for (int i = 0; i < m_nObjects; ++i)
	{
		Position.x = Random(0.f, 500.f);
		Position.z = Random(0.f, 300.f);
		m_ppObjects[i]->SetPosition(Position.x, pTerrain->GetHeight(Position.x, Position.z), Position.z);
	}

	if (pDeadTreesModel01) delete pDeadTreesModel01;
	if (pDeadTreesModel02) delete pDeadTreesModel02;
	if (pDeadTreesModel03) delete pDeadTreesModel03;
	if (pDeadTreesModel04) delete pDeadTreesModel04;
	if (pDeadTreesModel05) delete pDeadTreesModel05;

	if (pPineTreesModel01) delete pPineTreesModel01;
	if (pPineTreesModel02) delete pPineTreesModel02;
	if (pPineTreesModel03) delete pPineTreesModel03;
	if (pPineTreesModel04) delete pPineTreesModel04;
	if (pPineTreesModel05) delete pPineTreesModel05;
	if (pPineTreesModel06) delete pPineTreesModel06;
	if (pPineTreesModel07) delete pPineTreesModel07;
	if (pPineTreesModel08) delete pPineTreesModel08;

	if (pRocks01) delete pRocks01;
	if (pRocks02) delete pRocks02;
	if (pRocks03) delete pRocks03;
	if (pRocks04) delete pRocks04;
	if (pRocks05) delete pRocks05;
}

void CStandardObjectsShader::ReleaseObjects()
{
	if (m_ppObjects)
	{
		for (int i = 0; i < m_nObjects; ++i)
			if (m_ppObjects[i])
				m_ppObjects[i]->Release();
		delete[] m_ppObjects;
	}
}

void CStandardObjectsShader::AnimateObjects(float fTimeElapsed, CCamera* pCamera, CPlayer* pPlayer)
{
}

void CStandardObjectsShader::ReleaseUploadBuffers()
{
	for (int i = 0; i < m_nObjects; ++i)
		if (m_ppObjects[i])
			m_ppObjects[i]->ReleaseUploadBuffers();
}

void CStandardObjectsShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CShader::Render(pd3dCommandList, pCamera);

	for (int i = 0; i < m_nObjects; ++i)
	{
		if (m_ppObjects[i])
		{
			m_ppObjects[i]->Render(pd3dCommandList, pCamera);
		}
	}
}