#pragma once
#include "../StandardShader/StandardShader.h"


class CMapToolShader : public CStandardShader
{
public:
	CMapToolShader();
	virtual ~CMapToolShader();

	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, void *pContext = NULL);
	//virtual void AnimateObjects(float fTimeElapsed)
	virtual void ReleaseObjects();
	
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);
	

	void InsertObject(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList,CPlayer *pPlayer,const string& model);
	void DeleteObject();
	void MakeMapFile();

protected:
	CTerrain*			m_pTerrain;
	map<string, CLoadedModelInfo*> m_ModelsList; // 모델들을 관리하는 맵 

	vector<pair<string,CGameObject*>> m_Objects;			//실제 파일에 쓸 오브젝트;

	int m_nObjects;
};

