#pragma once
#include "../StandardShader.h"

class CLoadedModelInfo;
struct MapObjectInfo;
struct Bounds;
class CMapObjectsShader : public CStandardShader
{
public:
	CMapObjectsShader();
	virtual ~CMapObjectsShader();

	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature,
		const map<string, CLoadedModelInfo*>& ModelMap, const multimap<string, MapObjectInfo*>& MapObjectInfo, const map<string, Bounds*>& BoundMap, void *pContext = NULL);

	virtual void AnimateObjects(float fTimeElapsed, CCamera *pCamera, CPlayer *pPlayer = NULL);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera* pCamera, int nPipelineState);
	virtual D3D12_RASTERIZER_DESC CreateRasterizerState();


	virtual void ReleaseObjects();
	virtual void ReleaseUploadBuffers();
	
	const list<CGameObject*>& getSurroundingList()	const { return m_SurroundingList; }

private:
	list<CGameObject*> m_SurroundingList;
};