#include "../../../Stdafx/Stdafx.h"
#include "MapObjectShader.h"
#include "../../../GameObject/Surrounding/Surrounding.h"
#include "../../../GameObject/Terrain/Terrain.h"
#include "../../../ResourceManager/ResourceManager.h"

CMapObjectsShader::CMapObjectsShader()
{
}

CMapObjectsShader::~CMapObjectsShader()
{
}

void CMapObjectsShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature,
	const map<string, CLoadedModelInfo*>& ModelMap, const multimap<string, MapObjectInfo*>& MapObjectInfo, const map<string, Bounds*>& BoundMap, void* pContext)
{
	for (auto iter = ModelMap.begin(); iter != ModelMap.end(); ++iter)
	{
		string name = (*iter).first;
		// multimap 컨테이너에서 같은 키를 갖는 벨류를 찾을 때, 사용하는 루프
		for (auto iter2 = MapObjectInfo.lower_bound(name); iter2 != MapObjectInfo.upper_bound(name); ++iter2)
		{
			CSurrounding* pSurrounding = new CSurrounding(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
			pSurrounding->SetChild((*iter).second->m_pModelRootObject, true);
			pSurrounding->SetPosition((*iter2).second->m_Position);
			pSurrounding->SetLookVector((*iter2).second->m_Look);
			pSurrounding->SetUpVector((*iter2).second->m_Up);
			pSurrounding->SetRightVector((*iter2).second->m_Right);
			auto iter3 = BoundMap.find(name);
			if (iter3 != BoundMap.end())
				pSurrounding->SetOOBB((*iter3).second->m_xmf3Center, (*iter3).second->m_xmf3Extent, XMFLOAT4(0, 0, 0, 1));

			if (name != "PondSquare")
				pSurrounding->Initialize_Shadow((*iter).second, pSurrounding);

			m_SurroundingList.emplace_back(pSurrounding);
		}
	}
}

void CMapObjectsShader::AnimateObjects(float fTimeElapsed, CCamera* pCamera, CPlayer* pPlayer)
{
	m_fElapsedTime += fTimeElapsed;

	for (auto iter = m_SurroundingList.begin(); iter != m_SurroundingList.end(); ++iter)
	{
		(*iter)->Animate(m_fElapsedTime);
	}
}

void CMapObjectsShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera, int nPipelineState)
{
	for (auto iter = m_SurroundingList.begin(); iter != m_SurroundingList.end(); ++iter)
	{
		(*iter)->UpdateTransform(nullptr);
		(*iter)->Render(pd3dCommandList, pCamera, nPipelineState);
	}
}

void CMapObjectsShader::ReleaseObjects()
{
	for (auto iter = m_SurroundingList.begin(); iter != m_SurroundingList.end(); ++iter)
		(*iter)->Release();
}

void CMapObjectsShader::ReleaseUploadBuffers()
{
	for (auto iter = m_SurroundingList.begin(); iter != m_SurroundingList.end(); ++iter)
		(*iter)->ReleaseUploadBuffers();
}