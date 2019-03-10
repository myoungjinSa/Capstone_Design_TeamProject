#pragma once

#include "../GameObject.h"

class CLoadedModelInfo;
class CShadow;
class CItem : public CGameObject
{
public:
	CItem();
	virtual ~CItem();

	void Initialize_Shadow(CLoadedModelInfo* pLoadedModel, CGameObject* pGameObject);

	bool getDeleteItem()	const { return m_DeleteItem; }
	void setDeleteItem(bool value) { m_DeleteItem = value; }

	void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera, int nPipelineState);

private:
	bool m_DeleteItem = false;
	CShadow*	m_pShadow{ nullptr };
};