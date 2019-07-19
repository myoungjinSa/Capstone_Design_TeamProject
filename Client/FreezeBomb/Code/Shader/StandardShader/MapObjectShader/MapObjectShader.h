#pragma once
#include "../StandardShader.h"
#include "../../../ResourceManager/ResourceManager.h"

class CMapObjectsShader : public CStandardShader
{
public:
	CMapObjectsShader();
	virtual ~CMapObjectsShader();

	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature,
		const map<string, CLoadedModelInfo*>& ModelMap, const unordered_map<unsigned char, RoundInfo>& RoundMapObjectInfo, const map<string, Bounds*>& BoundMap, void* pContext = nullptr);

	virtual void AnimateObjects(float fTimeElapsed, CCamera *pCamera, CPlayer *pPlayer = NULL);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera* pCamera, int nPipelineState);
	virtual D3D12_RASTERIZER_DESC CreateRasterizerState();

	virtual void ReleaseObjects();
	virtual void ReleaseUploadBuffers();
	
	const list<CGameObject*>& getMapObjectList()	const;


public:
	typedef struct PLAYER_SPAWN_INFO
	{
		XMFLOAT3 pos;
	
	}SPAWN_INFO;

	static SPAWN_INFO spawn[3][6];

	static void LoadSpawnInfo(const string& filename,const int& round);
private:
	using RoundMapObjectList = list<CGameObject*>;
	unordered_map<unsigned char, RoundMapObjectList> m_MapObjectList;
};

