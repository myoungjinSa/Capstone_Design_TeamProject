#include "../../Stdafx/Stdafx.h"
#include "../../GameObject/Surrounding/Surrounding.h"
#include "../../GameObject/Terrain/Terrain.h"
#include "../../GameObject/Player/Player.h"
#include "MapTool.h"


bool operator<(pair<string, CGameObject*> &p1, pair< string, CGameObject> &p2)
{
	return p1.first < p2.first;
}

//////////////////////////////////////////////////////////////////////////////////////////

CMapToolShader::CMapToolShader()
	:m_nObjects(0),
	m_pTerrain(nullptr)
{

}

CMapToolShader::~CMapToolShader()
{


}

void CMapToolShader::InsertObject(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, CPlayer* pPlayer, const string& model)
{


	auto iter = m_ModelsList.find(model);

	if (iter != m_ModelsList.end())
	{

		if (pPlayer == nullptr)
		{
			return;
		}
		CLoadedModelInfo* pSurroundingModel = (*iter).second;

		XMFLOAT3 position = pPlayer->GetPosition();
		XMFLOAT3 lookVec = pPlayer->GetLookVector();
		XMFLOAT3 upVec = pPlayer->GetUpVector();
		XMFLOAT3 rightVec = pPlayer->GetRightVector();

		
		CGameObject* pGameObject = new CSurrounding(pd3dDevice, pd3dCommandList,NULL);
		//Object 위치 설정
		pGameObject->SetPosition(position);

		//Object 방향 설정
		pGameObject->SetLookVector(lookVec);
		pGameObject->SetUpVector(upVec);
		pGameObject->SetRightVector(rightVec);
		pGameObject->SetChild(pSurroundingModel->m_pModelRootObject,true);
		//pSurroundingModel->m_pModelRootObject->AddRef();

		m_Objects.emplace_back(make_pair((*iter).first, pGameObject));
	}


}
void CMapToolShader::DeleteObject()
{

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
	ofstream fout;

	string str;


	SortDescByName();		//파일에 출력하기전에 이름의 내림차순으로 정렬
	
	fout.open("MapVer_1.txt"/*ios::out -> automatically Set*/);
	fout <<"Objects Count:"<< m_Objects.size()<<"\n";
	
	for (auto iter = m_Objects.begin(); iter != m_Objects.end();iter++)
	{
		
		str = "<" + iter->first + ">\n";			//write Objects Name
		fout.write(str.c_str(), str.size());
		str = "<Position>: ";
		fout.write(str.c_str(), str.size());
		fout << iter->second->GetPosition().x << " " << iter->second->GetPosition().y << " " << iter->second->GetPosition().z << "\n";
		//fout << "\n";

		str = "<Look>: ";
		fout.write(str.c_str(), str.size());
		fout << iter->second->GetLook().x << " " << iter->second->GetLook().y << " " << iter->second->GetLook().z << "\n";

		str = "<Up>: ";
		fout.write(str.c_str(), str.size());
		fout << iter->second->GetUp().x << " " << iter->second->GetUp().y << " " << iter->second->GetUp().z << "\n";

		str = "<Right>: ";
		fout.write(str.c_str(), str.size());
		fout << iter->second->GetRight().x << " " << iter->second->GetRight().y << " " << iter->second->GetRight().z << "\n";
		fout << "\n";

	}
	
	fout.close();




}


void CMapToolShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, void *pContext)
{

	CLoadedModelInfo* pDeadTreesModel01 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_DeadTrunk_01.bin", this, false);
	CLoadedModelInfo* pDeadTreesModel02 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_DeadTrunk_02.bin", this, false);
	CLoadedModelInfo* pDeadTreesModel03 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_DeadTrunk_03.bin", this, false);
	CLoadedModelInfo* pDeadTreesModel04 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_DeadTrunk_04.bin", this, false);
	CLoadedModelInfo* pDeadTreesModel05 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_DeadTrunk_05.bin", this, false);

	CLoadedModelInfo* pPineTreesModel01 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_PineTree_Snow_01.bin", this, false);
	CLoadedModelInfo* pPineTreesModel02 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_PineTree_Snow_02.bin", this, false);
	CLoadedModelInfo* pPineTreesModel03 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_PineTree_Snow_03.bin", this, false);
	CLoadedModelInfo* pPineTreesModel04 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_PineTree_Snow_04.bin", this, false);
	CLoadedModelInfo* pPineTreesModel05 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_PineTree_Snow_05.bin", this, false);
	CLoadedModelInfo* pPineTreesModel06 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_PineTree_Snow_06.bin", this, false);
	CLoadedModelInfo* pPineTreesModel07 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_PineTree_Snow_07.bin", this, false);
	CLoadedModelInfo* pPineTreesModel08 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_PineTree_Snow_08.bin", this, false);

	CLoadedModelInfo* pRocks01 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_BigPlainRock_Snow_01.bin", this, false);
	CLoadedModelInfo* pRocks02 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_BigPlainRock_Snow_02.bin", this, false);
	CLoadedModelInfo* pRocks03 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_BigPlainRock_Snow_03.bin", this, false);
	CLoadedModelInfo* pRocks04 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_BigPlainRock_Snow_04.bin", this, false);
	CLoadedModelInfo* pRocks05 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_PlainSmall_Snow_01.bin", this, false);

	CLoadedModelInfo* pDeer01 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_Deer.bin", this, false);


	m_pTerrain = (CTerrain*)pContext;

	//모델 Map에 저장

	/*m_ModelsList.insert(pair<string, CLoadedModelInfo*>("SM_DeadTrunk_01", pDeadTreesModel01));
	m_ModelsList.insert(pair<string, CLoadedModelInfo*>("SM_DeadTrunk_02", pDeadTreesModel02));
	m_ModelsList.insert(pair<string, CLoadedModelInfo*>("SM_DeadTrunk_03", pDeadTreesModel03));
	m_ModelsList.insert(pair<string, CLoadedModelInfo*>("SM_DeadTrunk_04", pDeadTreesModel04));
	m_ModelsList.insert(pair<string, CLoadedModelInfo*>("SM_DeadTrunk_05", pDeadTreesModel05));

	m_ModelsList.insert(pair < string, CLoadedModelInfo*>("SM_PineTree_Snow_01", pPineTreesModel01));
	m_ModelsList.insert(pair < string, CLoadedModelInfo*>("SM_PineTree_Snow_02", pPineTreesModel02));
	m_ModelsList.insert(pair < string, CLoadedModelInfo*>("SM_PineTree_Snow_03", pPineTreesModel03));
	m_ModelsList.insert(pair < string, CLoadedModelInfo*>("SM_PineTree_Snow_04", pPineTreesModel04));
	m_ModelsList.insert(pair < string, CLoadedModelInfo*>("SM_PineTree_Snow_05", pPineTreesModel05));
	m_ModelsList.insert(pair < string, CLoadedModelInfo*>("SM_PineTree_Snow_06", pPineTreesModel06));
	m_ModelsList.insert(pair < string, CLoadedModelInfo*>("SM_PineTree_Snow_07", pPineTreesModel07));
	m_ModelsList.insert(pair < string, CLoadedModelInfo*>("SM_PineTree_Snow_08", pPineTreesModel08));

	m_ModelsList.insert(pair < string, CLoadedModelInfo*>("SM_BigPlainRock_Snow_01", pRocks01));
	m_ModelsList.insert(pair < string, CLoadedModelInfo*>("SM_BigPlainRock_Snow_02", pRocks02));
	m_ModelsList.insert(pair < string, CLoadedModelInfo*>("SM_BigPlainRock_Snow_03", pRocks03));
	m_ModelsList.insert(pair < string, CLoadedModelInfo*>("SM_BigPlainRock_Snow_04", pRocks04));
	m_ModelsList.insert(pair < string, CLoadedModelInfo*>("SM_PlainSmall_Snow_01", pRocks05));*/

	m_ModelsList.emplace("SM_DeadTrunk_01", pDeadTreesModel01);
	m_ModelsList.emplace("SM_DeadTrunk_02", pDeadTreesModel02);
	m_ModelsList.emplace("SM_DeadTrunk_03", pDeadTreesModel03);
	m_ModelsList.emplace("SM_DeadTrunk_04", pDeadTreesModel04);
	m_ModelsList.emplace("SM_DeadTrunk_05", pDeadTreesModel05);

	m_ModelsList.emplace("SM_PineTree_Snow_01", pPineTreesModel01);
	m_ModelsList.emplace("SM_PineTree_Snow_02", pPineTreesModel02);
	m_ModelsList.emplace("SM_PineTree_Snow_03", pPineTreesModel03);
	m_ModelsList.emplace("SM_PineTree_Snow_04", pPineTreesModel04);
	m_ModelsList.emplace("SM_PineTree_Snow_05", pPineTreesModel05);
	m_ModelsList.emplace("SM_PineTree_Snow_06", pPineTreesModel06);
	m_ModelsList.emplace("SM_PineTree_Snow_07", pPineTreesModel07);
	m_ModelsList.emplace("SM_PineTree_Snow_08", pPineTreesModel08);

	m_ModelsList.emplace("SM_BigPlainRock_Snow_01", pRocks01);
	m_ModelsList.emplace("SM_BigPlainRock_Snow_02", pRocks02);
	m_ModelsList.emplace("SM_BigPlainRock_Snow_03", pRocks03);
	m_ModelsList.emplace("SM_BigPlainRock_Snow_04", pRocks04);
	m_ModelsList.emplace("SM_PlainSmall_Snow_01", pRocks05);



}

void CMapToolShader::ReleaseObjects()
{
	for (auto iter = m_Objects.begin(); iter != m_Objects.end(); )
	{
		iter = m_Objects.erase(iter);
	}
	m_Objects.clear();

	for (auto iter = m_ModelsList.begin(); iter != m_ModelsList.end(); )
	{
		iter = m_ModelsList.erase(iter);
	}
	m_ModelsList.clear();


}
void CMapToolShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CShader::Render(pd3dCommandList, pCamera);

	//for (int i = 0; i < m_nObjects; ++i)
	//{
	//	if (m_ppObjects[i])
	//	{
	//		m_ppObjects[i]->Render(pd3dCommandList, pCamera);
	//	}
	//}
	for (auto iter = m_Objects.begin(); iter != m_Objects.end();iter++)
	{
		if (iter->second)
		{
			iter->second->Animate(m_fElapsedTime);
			iter->second->UpdateTransform(NULL);
			iter->second->Render(pd3dCommandList, pCamera);
		}
	}
}
