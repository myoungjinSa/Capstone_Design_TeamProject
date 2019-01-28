#pragma once

#include "../StandardShader.h"

class CGameObject;

class CStandardObjectsShader : public CStandardShader
{
public:
	CStandardObjectsShader();
	virtual ~CStandardObjectsShader();

	float Random(float fMin, float fMax);
	float Random();
	XMFLOAT3 RandomPositionInSphere(XMFLOAT3 xmf3Center, float fRadius, int nColumn, int nColumnSpace);

	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, void *pContext = NULL);
	virtual void AnimateObjects(float fTimeElapsed);
	virtual void ReleaseObjects();
	virtual void ReleaseUploadBuffers();
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);

private:
	CGameObject**	m_ppObjects = 0;
	int							m_nObjects = 0;
};
