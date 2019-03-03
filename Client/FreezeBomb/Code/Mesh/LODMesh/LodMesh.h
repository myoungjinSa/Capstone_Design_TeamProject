#pragma once
#include "../Mesh.h"

//카메라와의 거리에 따라 정점(폴리곤) 수가 결정되는 메시
class CLodMesh : public CStandardMesh
{
public:
	CLodMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual ~CLodMesh();

public:

	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, int nSubSet, UINT lodLevel);

};