#pragma once

#include "../GameObject.h"

class CItem : public CGameObject
{
public:
	CItem();
	virtual ~CItem();

	void setNormalItem(bool value) { m_NormalItem = value; }
	bool getNormalItem()	const { return m_NormalItem; }

	void setSpecialItem(bool value) { m_SpecialItem = value; }
	bool getSpecialItem()	const { return m_SpecialItem; }

private:
	bool m_NormalItem = false;
	bool m_SpecialItem = false;
};