#include "../../../Stdafx/Stdafx.h"
#include "MapToolShader.h"
#include "../../../GameObject/Surrounding/Surrounding.h"
#include "../../../GameObject/Terrain/Terrain.h"
#include "../../../GameObject/Player/Player.h"


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

		XMFLOAT3 position = Vector3::Add(pPlayer->GetPosition(),XMFLOAT3(0.0f,0.01f,0.0f)); 
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

void CMapToolShader::MakeMapBinaryFile(const string& filename)
{

	ofstream out(filename + ".bin", ios::binary);

	
	int objCount = m_Objects.size();
	out.write(reinterpret_cast<const char*>(&objCount), sizeof(int));

	cout << objCount;

	for (int i = 0 ; i < objCount ; i++)
	{
		size_t stringLength = m_Objects[i].first.length();

		out.write(reinterpret_cast<const char*>(&stringLength), sizeof(size_t));
		out.write(m_Objects[i].first.c_str(), sizeof(char)*stringLength);

		string pos = "<Position>";
		stringLength = pos.length();
		out.write(reinterpret_cast<const char*>(&stringLength), sizeof(size_t));
		out.write(pos.c_str(), sizeof(char)*stringLength);
		//out.write(reinterpret_cast<const char*>(&m_Objects[i].second->GetPosition()),sizeof(XMFLOAT3));
		//out.write(reinterpret_cast<const char*>(&(m_Objects[i].second)->GetPosition().x), sizeof(float));

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
	//BYTE nReads = 0;
	////out << sizeof("<Object Count>:");
	////out.write(&nReads, sizeof("<Object Count>:"));
	//out.write("<Object Count>:", string("<Object Count>").length());
	//for (auto iter = m_Objects.begin(); iter != m_Objects.end(); iter++)
	//{ 
	//	out.write(iter->first.c_str(), sizeof(iter->first.length()));
	//	out.write("<Position>:",string("<Position>:").length());
	//	out << iter->second->GetPosition().x<<iter->second->GetPosition().y<<iter->second->GetPosition().z;
	//	out.write("<Look>:", string("<Look>:").length());
	//	out << iter->second->GetLook().x << iter->second->GetLook().y<< iter->second->GetLook().z;
	//	out.write("<Up>:", string("<Up>:").length());
	//	out << iter->second->GetUp().x << iter->second->GetUp().y << iter->second->GetUp().z;
	//	out.write("<Right>:", string("<Right>:").length());
	//	out << iter->second->GetRight().x  << iter->second->GetRight().y  << iter->second->GetRight().z;
	//}
	//

	out.close();

}

void CMapToolShader::MakeMapFile()
{
	ofstream fout;

	string str;


	SortDescByName();		//파일에 출력하기전에 이름의 내림차순으로 정렬
	
	fout.open("MapVer1.txt"/*ios::out -> automatically Set*/);
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
	CLoadedModelInfo* pDeadTreesModel01 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_DeadTrunk_01.bin", this, false, "Surrounding");
	CLoadedModelInfo* pDeadTreesModel02 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_DeadTrunk_02.bin", this, false, "Surrounding");
	CLoadedModelInfo* pDeadTreesModel03 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_DeadTrunk_03.bin", this, false, "Surrounding");
	CLoadedModelInfo* pDeadTreesModel04 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_DeadTrunk_04.bin", this, false, "Surrounding");
	CLoadedModelInfo* pDeadTreesModel05 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_DeadTrunk_05.bin", this, false, "Surrounding");
	m_nDeadTreeModelCount = 5;


	CLoadedModelInfo* pPineTreesModel01 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_PineTree_Snow_01.bin", this, false, "Surrounding");
	CLoadedModelInfo* pPineTreesModel02 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_PineTree_Snow_02.bin", this, false, "Surrounding");
	CLoadedModelInfo* pPineTreesModel03 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_PineTree_Snow_03.bin", this, false, "Surrounding");
	CLoadedModelInfo* pPineTreesModel04 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_PineTree_Snow_04.bin", this, false, "Surrounding");
	CLoadedModelInfo* pPineTreesModel05 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_PineTree_Snow_05.bin", this, false, "Surrounding");
	CLoadedModelInfo* pPineTreesModel06 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_PineTree_Snow_06.bin", this, false, "Surrounding");
	CLoadedModelInfo* pPineTreesModel07 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_PineTree_Snow_07.bin", this, false, "Surrounding");
	CLoadedModelInfo* pPineTreesModel08 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_PineTree_Snow_08.bin", this, false, "Surrounding");
	m_nPineTreeModelCount = 8;


	CLoadedModelInfo* pRocks01 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_BigPlainRock_Snow_01.bin", this, false, "Surrounding");
	CLoadedModelInfo* pRocks02 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_BigPlainRock_Snow_02.bin", this, false, "Surrounding");
	CLoadedModelInfo* pRocks03 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_BigPlainRock_Snow_03.bin", this, false, "Surrounding");
	CLoadedModelInfo* pRocks04 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_BigPlainRock_Snow_04.bin", this, false, "Surrounding");
	m_nBigRockModelCount = 4;
	
	CLoadedModelInfo* pRocks05 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_PlainSmall_Snow_01.bin", this, false, "Surrounding");
	m_nSmallRockModelCount = 1;

	CLoadedModelInfo* pDeer01 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/SM_Deer.bin", this, false, "Surrounding");
	m_nDeerModelCount = 1;

	CLoadedModelInfo* pPond01 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/PondSquare.bin", this, false, "Surrounding");
	m_nPondModelCount = 1;


	CLoadedModelInfo* pFence01 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/LowPoly_Fence_01.bin", this, false, "Surrounding");
	CLoadedModelInfo* pFence02 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "../Resource/Models/LowPoly_Fence_02.bin", this, false, "Surrounding");
	m_nFenceModelCount = 2;
	
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
	//m_ModelsList.emplace("SM_PlainSmall_Snow_01", pRocks05);
	m_ModelsList.emplace("SM_Deer", pDeer01);
	m_ModelsList.emplace("PondSquare", pPond01);

	m_ModelsList.emplace("LowPoly_-_Fence_A", pFence01);
	m_ModelsList.emplace("LowPoly_-_Fence_B", pFence02);




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
void CMapToolShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera, int nPipelineState)
{
	CShader::Render(pd3dCommandList, pCamera, nPipelineState);

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
