#pragma once

#include "../StandardShader.h"

class CTerrain;
class CLoadedModelInfo;
class CMapToolShader : public CStandardShader
{
public:
	CMapToolShader();
	virtual ~CMapToolShader();
	
	void InstallMapObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CPlayer* pPlayer, char MapObjectType);

	void InsertObject(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, CPlayer *pPlayer, const string& model);

	void InsertItem(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, CPlayer *pPlayer, const string& model);

	void SortDescByName();	//오브젝트 이름의 내림차순으로 정렬 (동일한 이름일 경우 순서 정할수 없음)

	void MakeMapFile();
	void OutputMapTextFile();
	void OutputMapBinaryFile();

	virtual void BuildObjects(const map<string, CLoadedModelInfo*>& ModelMap);
	virtual void ReleaseObjects();
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera, int nPipelineState);

	void DeleteAll();
	void Delete();

	int GetPineTreeModelCount() { return m_nPineTreeModelCount; }
	int GetDeadTreeModelCount() { return m_nDeadTreeModelCount; }
	int GetBigRockModelCount() { return m_nBigRockModelCount; }
	int GetFenceModelCount() { return m_nFenceModelCount; }

	int GetCurrDeadTreeIndex()		const { return m_nCurrDeadTreeModelIndex; }
	int GetCurrPineTreeIndex()		const { return m_nCurrPineTreeModelIndex; }
	int GetCurrBigRockIndex()		const { return m_nCurrBigRockModelIndex; }
	int GetCurrFenceIndex()			const { return m_nCurrFenceModelIndex; }

	void SetCurrDeadTreeIndex(int index) { m_nCurrDeadTreeModelIndex = index; }
	void SetCurrPineTreeIndex(int index) { m_nCurrPineTreeModelIndex = index; }
	void SetCurrBigRockIndex(int index) { m_nCurrBigRockModelIndex = index; }
	void SetCurrFenceIndex(int index) { m_nCurrFenceModelIndex = index; }

	enum MapObjectType { DeadTree = 0x31, PineTree = 0x32, BigRock = 0x33, Deer = 0x34, Fence = 0x35, Frozen_Road = 0x36, FirePit = 0x37,
		Hammer = 0x48, GoldTimer = 0x54,
		DeleteAllObject = 0x44, DeleteObject = 0x52, OutputFile = 0x4F };

	void BuildWall(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);

private:
	map<string, CLoadedModelInfo*> m_ModelsList;			// 모델들을 관리하는 맵 
	vector<pair<string, CGameObject*>> m_Objects;			//실제 파일에 쓸 오브젝트;
	list<CGameObject*> m_RemovedObjectList;

	int m_nObjects;

	int m_nDeadTreeModelCount = 0;
	int m_nPineTreeModelCount = 0;
	int m_nBigRockModelCount = 0;
	int m_nFenceModelCount = 0;

	int m_nCurrDeadTreeModelIndex = 0;
	int m_nCurrPineTreeModelIndex = 0;
	int m_nCurrBigRockModelIndex = 0;
	int m_nCurrFenceModelIndex = 0;
};

