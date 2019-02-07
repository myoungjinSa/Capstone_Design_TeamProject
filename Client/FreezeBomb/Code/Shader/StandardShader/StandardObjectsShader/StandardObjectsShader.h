#pragma once

#include "../StandardShader.h"

class CStandardObjectsShader : public CStandardShader
{
public:
	CStandardObjectsShader();
	virtual ~CStandardObjectsShader();

	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, void *pContext = NULL);
	virtual void AnimateObjects(float fTimeElapsed, CCamera *pCamera, CPlayer* pPlayer = NULL);
	virtual void ReleaseObjects();
	virtual void ReleaseUploadBuffers();
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);
};
