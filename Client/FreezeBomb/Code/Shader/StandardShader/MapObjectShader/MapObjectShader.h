#pragma once
#include "../StandardShader.h"

struct Bounds;
class CLoadedModelInfo;

class CMapObjectsShader : public CStandardShader
{
public:
	CMapObjectsShader();
	virtual ~CMapObjectsShader();

	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature,
		const map<string, Bounds*>& Context, void *pContext = NULL);

	virtual void AnimateObjects(float fTimeElapsed, CCamera *pCamera, CPlayer *pPlayer = NULL);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera* pCamera, int nPipelineState);

private:
	CLoadedModelInfo**	m_ppDeadTreeModel{ nullptr };
	CLoadedModelInfo**	m_ppPineTreeModel{ nullptr };
	CLoadedModelInfo**	m_ppBigRockModel{ nullptr };
	CLoadedModelInfo**  m_ppDeerModel { nullptr };
	CLoadedModelInfo**  m_ppPondModel { nullptr };
	CLoadedModelInfo**  m_ppFenceModel{ nullptr };

	map<string, CLoadedModelInfo*> m_ModelsList; // ¸ðµ¨µéÀ» °ü¸®ÇÏ´Â ¸Ê 
};