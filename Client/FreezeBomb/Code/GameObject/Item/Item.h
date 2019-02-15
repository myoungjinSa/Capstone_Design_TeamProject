#pragma once

#include "../GameObject.h"

class CItem : public CGameObject
{
public:
	CItem();
	virtual ~CItem();

	bool getDeleteItem()	const { return m_DeleteItem; }
	void setDeleteItem(bool value) { m_DeleteItem = value; }

private:
	bool m_DeleteItem = false;
};