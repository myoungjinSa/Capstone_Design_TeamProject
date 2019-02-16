#pragma once

#include "../GameObject.h"

class CEvilBear : public CGameObject
{
public:
	CEvilBear(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature,int matID);
	virtual ~CEvilBear();

	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList,bool bIce,int matID ,CCamera *pCamera = NULL);
};