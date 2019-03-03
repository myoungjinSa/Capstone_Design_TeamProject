#pragma once
#include "../GameObject.h"

class CLoadedModelInfo;
class CFoliageObject : public CGameObject
{
public:
	CFoliageObject(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature);
	virtual ~CFoliageObject();

	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, UINT lodlevel, CCamera *pCamera, int nPipelineState);

	static CGameObject *LoadFrameHierarchyFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, CGameObject *pParent, FILE *pInFile, CShader *pShader, int *pnSkinnedMeshes, string Type);
	static CLoadedModelInfo* LoadGeometryAndAnimationFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, char *pstrFileName, CShader *pShader, bool bHasAnimation, string Type);
};