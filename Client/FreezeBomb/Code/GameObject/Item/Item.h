#pragma once

#include "../GameObject.h"

class CLoadedModelInfo;
class CItem : public CGameObject
{
public:
	CItem();
	virtual ~CItem();

	void Initialize_Shadow(CLoadedModelInfo* pLoadedModel, CGameObject* pGameObject);

	bool getDeleteItem()	const { return m_DeleteItem; }
	void setDeleteItem(bool value) { m_DeleteItem = value; }

	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera, int nPipelineState);

	enum ItemType { NormalHammer, GoldHammer, GoldTimer, Empty };
	byte getItemType()	const { return m_ItemType; }
	void setItemType(byte value) { m_ItemType = value; }

private:
	byte m_ItemType = 0;

	bool m_DeleteItem = false;
};