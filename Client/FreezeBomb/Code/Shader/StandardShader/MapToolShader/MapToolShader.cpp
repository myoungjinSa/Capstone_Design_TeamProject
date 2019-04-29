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

CMapToolShader::CMapToolShader()	:m_nObjects(0)
{
}

CMapToolShader::~CMapToolShader()
{
}

void CMapToolShader::InsertObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CPlayer* pPlayer, const string& model)
{
	auto iter = m_ModelsList.find(model);

	if (iter != m_ModelsList.end())
	{
		if (pPlayer == nullptr)
			return;

		XMFLOAT3 position = pPlayer->GetPosition(); 
		XMFLOAT3 lookVec = pPlayer->GetLookVector();
		XMFLOAT3 upVec = pPlayer->GetUpVector();
		XMFLOAT3 rightVec = pPlayer->GetRightVector();

		CSurrounding* pGameObject = new CSurrounding(pd3dDevice, pd3dCommandList, nullptr);
		//Object 위치 설정
		pGameObject->SetPosition(position);

		//Object 방향 설정
		pGameObject->SetLookVector(lookVec);
		pGameObject->SetUpVector(upVec);
		pGameObject->SetRightVector(rightVec);
		pGameObject->SetChild((*iter).second->m_pModelRootObject, true);


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

	out.close();

}

void CMapToolShader::MakeMapFile()
{
	ofstream fout;

	string str;

	SortDescByName();		//파일에 출력하기전에 이름의 내림차순으로 정렬
	
	fout.open("../Resource/Position/Surrounding/MapVer1.txt");
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

void CMapToolShader::BuildObjects(const map<string, CLoadedModelInfo*>& ModelMap)
{
	m_ModelsList = ModelMap;
}


void CMapToolShader::ReleaseObjects()
{
	for (auto iter = m_Objects.begin(); iter != m_Objects.end(); )
	{
		iter = m_Objects.erase(iter);
	}
	m_Objects.clear();

	for (auto iter = m_ModelsList.begin(); iter != m_ModelsList.end(); )
		iter = m_ModelsList.erase(iter);
	m_ModelsList.clear();

}
void CMapToolShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera, int nPipelineState)
{
	CShader::Render(pd3dCommandList, pCamera, nPipelineState);

	for (auto iter = m_Objects.begin(); iter != m_Objects.end();iter++)
	{
		if (iter->second)
		{
			iter->second->Animate(m_fElapsedTime);
			iter->second->UpdateTransform(NULL);
			iter->second->Render(pd3dCommandList, pCamera, nPipelineState);
		}
	}
}
