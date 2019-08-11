#pragma once

#include "../GameObject.h"

class CBillboard : public CGameObject
{
public:
	CBillboard();
	CBillboard(int nMaterial);
	virtual ~CBillboard();
};